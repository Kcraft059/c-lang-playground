#include <stdio.h>
#include <test.h>

int main(int argc, char **argv)
{
  if (argc > 1) {
    char fakeString[] = {'A','H'}; // UB since not ending with \0
    argv[1] = fakeString;
    printf("Arg[1] : \"%s\" \n",argv[1]); // -> possible without knowing size since strings end with NULL
  }
}