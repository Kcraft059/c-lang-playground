#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 
 * This is a piece of my own hideous code (im a piece of shit at coding / in general)
 * Enjoy ;) (at least it's commented… a bit)
 * 
 */

enum ArgType {
  ARG_INT,
  ARG_FLOAT,
  ARG_STRING
};

struct argument {
  enum ArgType type;
  // bool optional;
  bool multiWord;
  union {
    char* _str;
    int _int;
    float _float;
  } value;
};

struct command {  // will onyl be sizeof(pointer) * 3 + sizeof(int)
  char* keyword;  // Since those are memory adresses, we don't care about size and it can be dynamic
  int argc;
  struct argument* args;
  int (*function)(struct command*);
  void* extra;
};

struct builtins {
  struct command* commands;
  int count;
};

int splitString(char* inString, char delimiter, int startIndex, int stopIndex, char* outString) {
  int rIndx = 0;
  int wIndx = 0;
  int delimiterOccurence = 0;

  bool success = true;
  while (1) {
    char chr = inString[rIndx];

    if (chr == delimiter) {
      ++delimiterOccurence;

      if (delimiterOccurence == startIndex) {
        wIndx = -1;
      }
      if (delimiterOccurence == stopIndex) {
        outString[wIndx] = '\0';
        break;
      }
      // when reaching startIndex -> reset wIndex
      // when reaching stopIndex -> stop loop & write = '\0'
    }

    if (chr == '\0') {  // When reaching end of string end loop
      outString[wIndx] = '\0';
      // If startIndex isn't reached or (stopIndex is reachable (!= -1) and not at the end of string and has not been reached)
      if (delimiterOccurence < startIndex || (stopIndex != -1 && stopIndex - 1 != delimiterOccurence && delimiterOccurence < stopIndex))
        success = 0;
      break;
    } else if (wIndx != -1) {  // Prevent write at outString[-1] out of bound write -> memory corruption on stack
      outString[wIndx] = chr;
    }

    // Move read & write index
    ++rIndx;
    ++wIndx;
  };

  if (success) {  // If found a matching string at index, no error
    return 1;
  };

  return 0;  // In any other case, there's an error
}

int storeArgValue(struct argument* arg, char* argString) {
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

int builtins_exit(struct command* cmd) {
  exit(cmd->args[0].value._int);
  return 1;
}

int builtins_echo(struct command* cmd) {
  // Print the value of the first arg of echo
  printf("%s\n", cmd->args[0].value._str);
  free(cmd->args[0].value._str);
  return 1;
}

int builtins_type(struct command* cmd) {
  // Store the keyword asked 
  char* cmdKeyword = cmd->args[0].value._str;
  // Get all builtins
  struct builtins *bltCmd = ((struct builtins*)cmd->extra);

  // Check for match
  for (int i = 0; i < bltCmd->count; ++i) {
    if (!strcmp(cmdKeyword, bltCmd->commands[i].keyword)) {
      printf("%s is a shell builtin\n", cmdKeyword);
      return 1;
    }
  }

  // If no match break
  printf("%s: not found\n", cmdKeyword);
  return 0;
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

    static struct argument echo_args[] = {{.type = ARG_STRING, .multiWord = true}};
    static struct argument exit_args[] = {{.type = ARG_INT, .multiWord = false}};
    static struct argument type_args[] = {{.type = ARG_STRING, .multiWord = false}};
    static struct command builtinsCmdArray[] = {
        {"echo", 1, echo_args, builtins_echo},
        {"type", 1, type_args, builtins_type},
        {"exit", 1, exit_args, builtins_exit}};
    static struct builtins bltCmd = {builtinsCmdArray, 3};

    bltCmd.commands[1].extra = &bltCmd;

    // Get command first arg (command keyword itself)
    char cmdKeywordBuff[32];
    if (!splitString(userInput, ' ', 0, 1, cmdKeywordBuff)) continue;

    // Check if command is in builtins
    bool sucess = false;
    struct command currentCmd;
    for (int i = 0; i < bltCmd.count; ++i) {
      if (!strcmp(cmdKeywordBuff, bltCmd.commands[i].keyword)) {
        currentCmd = bltCmd.commands[i];  // Copies cmd to not mess with static builtins, tho args are not copied properly
        sucess = true;
        break;
      }
    }

    if (!sucess) {
      printf("%s: command not found\n", cmdKeywordBuff);
      continue;
    }

    // Map command inputs & check if all inputs are passed
    sucess = true;
    for (int i = 0; i < currentCmd.argc; ++i) {
      char argStringBuff[2048];
      struct argument* arg = &(currentCmd.args[i]);  // Modifies the real structure

      // Get arg string
      int strEnd = arg->multiWord ? -1 : i + 2;

      if (splitString(userInput, ' ', i + 1, strEnd, argStringBuff)) {
        if (!storeArgValue(arg, argStringBuff)) {
          printf("Invalid argument for %s\n", currentCmd.keyword);
          sucess = false;
          break;
        }
      } else {
        printf("Missing argument for %s\n", currentCmd.keyword);
        sucess = false;
        break;
      }
    }

    if (!sucess) continue;

    /**
     *
     * Command execution
     *
     */

    currentCmd.function(&currentCmd);
  }

  return 0;
}
