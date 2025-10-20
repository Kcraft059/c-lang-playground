#include <define.h>
#include <stdlib.h>
#include <stdio.h>

void sendErr(char *errMsg, int errCode)
{
  char *finalMsg;
  asprintf(&finalMsg, SCAC " \033[38;5;1mError\033[0m: %s \n" SQES " Code: %d \n", errMsg, errCode); // Could have been shorter but good actual examlple of string formating.
  printf("%s",finalMsg);
  free(finalMsg);
  exit(errCode);
};