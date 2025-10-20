#include <stdio.h>
#include <stdlib.h>
#include <define.h>

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    int intArg;
    if (sscanf(argv[1], "%d", &intArg) != 0) // Far better than my own implementation
    {
      printf(SEXC " \033[38;5;2mChar Buff\033[0m: %d \n", intArg);
    }
    else
    {
      sendErr("Gloup", 3);
    };
  }
  else
  {
    sendErr("Not enough arguments!", 3);
  }
}