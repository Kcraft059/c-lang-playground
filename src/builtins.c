#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "definitions.h"

int builtins_exit(shCommand* cmd) {
  exit(cmd->args[0].value._int);
  freeCmd(cmd);
  return 1;
}

int builtins_echo(shCommand* cmd) {
  // Print the value of the first arg of echo
  printf("%s\n", cmd->args[0].value._str);
  free(cmd->args[0].value._str);
  freeCmd(cmd);
  return 1;
}

int builtins_type(shCommand* cmd) {
  // Store the keyword asked
  char* keyword = cmd->args[0].value._str;
  // Get all builtins
  builtinsInfo* bltCmd = ((builtinsInfo*)cmd->extra);

  // Check for match
  enum cmdType type = -1;

  // Check in builtins
  if (getBuiltin(keyword, bltCmd, NULL)) type = BUILTINCMD;
  char execPath[255];
  if (type == -1 && getInPath(keyword, getenv("PATH"), execPath)) type = EXEC;

  // Answer depending on matched type
  switch (type) {
    case BUILTINCMD:
      printf("%s is a shell builtin\n", keyword);
      break;
    case EXEC:
      printf("%s is %s\n", keyword, execPath);
      break;
    default:
      printf("%s: not found\n", keyword);
      break;
  }

  // Garbage collection
  free(keyword);
  freeCmd(cmd);
  return type != -1 ? 1 : 0;  // If type is -1 (not found), return 0
}

int builtins_getenv(shCommand* cmd) {
  commandArg* args = cmd->args;

  char* value = getenv(args[0].value._str);
  printf("%s=\"%s\"\n", args[0].value._str, value);

  free(args[0].value._str);
  freeCmd(cmd);
  return 1;
}

int builtins_run(shCommand* cmd) {
  commandArg* args = cmd->args;
  char cmdPath[1024];

  if (splitString(args[0].value._str, ' ', 0, 1, cmdPath) && getInPath(cmdPath, getenv("PATH"), cmdPath)) {
    execExternalProg(cmdPath, args[0].value._str);
    free(args[0].value._str);
    freeCmd(cmd);
    return 1;
  }

  free(args[0].value._str);
  freeCmd(cmd);
  return 0;
}

int builtins_pwd(shCommand* cmd) {
  // (actually better than `printf("%s",getenv("PATH"));` )
  char cwd[100];
  printf("%s\n", getcwd(cwd, sizeof(cwd)));
  return 1;
}

int builtins_cd(shCommand* cmd) {
  commandArg* args = cmd->args;
  char* dirpath = args[0].value._str;

  if (dirpath[0] == '~') {
    const char* home = getenv("HOME");
    size_t newLen = strlen(home) + strlen(dirpath);  // -1 for '~' + '/' + '\0'

    char* expanded = malloc(newLen + 1);
    sprintf(expanded, "%s%s", home, dirpath + 1); // dirpath + 1 to skip '~'
    dirpath = expanded;
  }

  struct stat s = {0};  // create an ouptut for stat

  if (stat(dirpath, &s) || !S_ISDIR(s.st_mode)) {
    printf("cd: %s: No such file or directory\n", dirpath);
    free(dirpath);
    freeCmd(cmd);
    return 0;
  }

  if (!chdir(dirpath)) {
    free(dirpath);
    freeCmd(cmd);
    return 0;
  }

  free(dirpath);
  freeCmd(cmd);
  return 1;
}