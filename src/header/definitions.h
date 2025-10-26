#include <stdbool.h>

/// Types

enum cmdType {
  BUILTINCMD,
  EXEC
};

enum ArgType {
  ARG_INT,
  ARG_FLOAT,
  ARG_STRING
};

typedef struct {
  enum ArgType type;
  bool multiWord;
  union {
    char* _str;
    int _int;
    float _float;
  } value;
} commandArg;

// Declare as a type here
typedef struct shCommand shCommand;  // Forward definition, act as placeholder for the compiler, needed to refer the structure itself in the structure

// Set properties here
struct shCommand {  // will onyl be sizeof(pointer) * 3 + sizeof(int)
  char* keyword;    // Since those are memory adresses, we don't care about size and it can be dynamic
  int argc;
  commandArg* args;
  int (*function)(shCommand*);
  void* extra;
};

typedef struct {
  shCommand* commands;
  int count;
} builtinsInfo;

/// Functions

int splitString(char*, char, int, int, char*);

int storeArgValue(commandArg*, char*);
int getBuiltin(char*, builtinsInfo*, shCommand*);
int getInPath(char*, char*, char*);
int mapInput(char*, shCommand*);
int execExternalProg(char*, char*);
void freeCmd(shCommand*);

/// Builtins

int builtins_exit(shCommand*);
int builtins_echo(shCommand*);
int builtins_type(shCommand*);
int builtins_getenv(shCommand*);
int builtins_run(shCommand*);
int builtins_pwd(shCommand*);
int builtins_cd(shCommand*);