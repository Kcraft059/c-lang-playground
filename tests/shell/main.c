#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "definitions.h"

int storeArgValue(commandArg* arg, char* argString) {
  // Scan appropriatly for each type
  switch ((*arg).type) {
    case ARG_INT:
      if (!sscanf(argString, "%d", &((*arg).value._int))) {
        return 0;
      };
      break;
    case ARG_FLOAT:
      if (!sscanf(argString, "%f", &((*arg).value._float))) {
        return 0;
      };
      break;
    case ARG_STRING:
      // (*arg).value._str = malloc((strlen(argStringBuff) + 1) * sizeof(char));
      // sscanf(argStringBuff, "%s", (*arg).value._str);
      (*arg).value._str = strdup(argString);  // same thing but better.
      break;
  }
  return 1;
}

int getBuiltin(char* cmdKeyword, builtinsInfo* builtins, shCommand* currentCmd) {
  // Cycles through all buitins and check for a keyword match
  for (int i = 0; i < builtins->count; ++i) {
    if (!strcmp(cmdKeyword, builtins->commands[i].keyword)) {
      if (currentCmd != NULL) {
        // If keyword is in builtins
        // Create a new instance of shCommand with new args
        // currentCmd = malloc(sizeof(shCommand));
        *currentCmd = builtins->commands[i];

        // Create a new instance of commandArg
        commandArg* currentArgs = malloc(currentCmd->argc * sizeof(commandArg));
        memcpy(currentArgs, currentCmd->args, currentCmd->argc * sizeof(commandArg));
        currentCmd->args = currentArgs;
      }
      return 1;
    }
  }
  return 0;
}

int getInPath(char* cmdKeyword, char* pathEnv, char* cmdPath) {
  char* pathBuff = strdup(pathEnv);  // getenv() always send a pointer from same mem adress, if overwritten, it will, affect all new runs
  char* currentDir = strtok(pathBuff, ":");
  // For each item in path
  while (currentDir != NULL) {
    // Make full path to path/cmdkeyword
    char resolvedPath[1024] = "";
    sprintf(resolvedPath, "%s/%s", currentDir, cmdKeyword);
    if (!access(resolvedPath, F_OK | X_OK)) {
      // If executable exists there then store path
      if (cmdPath != NULL) memcpy(cmdPath, resolvedPath, sizeof(char) * (strlen(resolvedPath) + 1));
      return 1;
    }
    // Get next item in path
    currentDir = strtok(NULL, ":");
  }
  return 0;
}

int mapInput(char* userInput, shCommand* cmd) {
  // Map command inputs & check if all inputs are passed
  for (int i = 0; i < cmd->argc; ++i) {
    char argStringBuff[2048];
    commandArg* arg = &(cmd->args[i]);  // Modifies the real structure

    // Get arg string
    int strEnd = arg->multiWord ? -1 : i + 2;

    if (splitString(userInput, ' ', i + 1, strEnd, argStringBuff)) {
      if (!storeArgValue(arg, argStringBuff)) {
        printf("Invalid argument for %s\n", cmd->keyword);
        return 0;
        break;
      }
    } else {
      printf("Missing argument for %s\n", cmd->keyword);
      return 0;
      break;
    }
  }
  return 1;
}

int execExternalProg(char* path, char* argString) {
  char** argv = malloc(sizeof(char*));
  int argvSize = 0;
  // argv[0] = path;

  char* argStringN = strdup(argString);
  char* token = strtok(argStringN, " ");

  while (token != NULL) {
    argv = realloc(argv, (argvSize + 1) * sizeof(char*));
    argv[argvSize++] = strdup(token);
    token = strtok(NULL, " ");
  };

  argv = realloc(argv, (argvSize + 1) * sizeof(char*));
  argv[argvSize] = NULL;

  /* char *argv[] = {"ls","-a","-l",NULL}; */

  pid_t pid = fork();  // Literally duplicates the program;

  if (pid == 0) {
    // If we are the forked program
    execv(path, argv);
  } else if (pid > 0) {
    // Else we are the parent
    int status;
    // We wait for the child
    waitpid(pid, &status, 0);
  } else {
    perror("fork");
    exit(1);
  }
  return 1;
}

void freeCmd(shCommand* cmd) {
  free(cmd->args);
  free(cmd);
}

int main(int argc, char* argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  // TODO: Uncomment the code below to pass the first stage
  while (1) {
    // Print prompt
    printf("$ ");

    // Initiate userInput buffer & get user input from stdin (on "\n")
    int readLength = 100;
    char userInput[readLength];
    fgets(userInput, readLength, stdin);

    // Remove trailing '\n' (shortens the string by adding '\0' one posisition prior)
    userInput[strlen(userInput) - 1] = '\0';

    /**
     *
     * Command parsing
     *
     */

    static commandArg echo_args[] = {{.type = ARG_STRING, .multiWord = true}};
    static commandArg exit_args[] = {{.type = ARG_INT, .multiWord = false}};
    static commandArg type_args[] = {{.type = ARG_STRING, .multiWord = false}};
    static commandArg run_args[] = {{.type = ARG_STRING, .multiWord = true}};
    static commandArg getenv_args[] = {{.type = ARG_STRING, .multiWord = false}};
    static commandArg cd_args[] = {{.type = ARG_STRING, .multiWord = true}};

    static shCommand builtinsCmdArray[] = {
        {"echo", 1, echo_args, builtins_echo},
        {"type", 1, type_args, builtins_type},
        {"exit", 1, exit_args, builtins_exit},
        {"run", 1, run_args, builtins_run},
        {"cd", 1, run_args, builtins_cd},
        {"getenv", 1, getenv_args, builtins_getenv},
        {.keyword = "pwd", .argc = 0, .function = builtins_pwd}};

    static builtinsInfo builtins = {builtinsCmdArray, 7};

    builtins.commands[1].extra = &builtins;

    // Get command first arg (command keyword itself)
    char cmdKeywordBuff[32];
    if (!splitString(userInput, ' ', 0, 1, cmdKeywordBuff)) continue;

    // Check command type

    enum cmdType type = -1;

    shCommand* currentCmd = malloc(sizeof(shCommand));
    if (getBuiltin(cmdKeywordBuff, &builtins, currentCmd)) type = BUILTINCMD;
    char execPath[1024];
    if (type == -1 && getInPath(cmdKeywordBuff, getenv("PATH"), execPath)) type = EXEC;

    switch (type) {
      case BUILTINCMD:
        // Get inputs for command
        if (!mapInput(userInput, currentCmd)) {
          freeCmd(currentCmd);
          continue;
        }
        int return_value = currentCmd->function(currentCmd);
        break;
      case EXEC:
        execExternalProg(execPath, userInput);
        break;
      default:
        printf("%s: command not found\n", cmdKeywordBuff);
        free(currentCmd);
        continue;
        break;
    }
  }

  return 0;
}
