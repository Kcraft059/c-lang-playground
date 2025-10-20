#include <stdio.h>
#include <stdlib.h>
#include <functions.h>

long *calc_nearest_factorial(long target)
{
  long product = 1;
  int index = 0;

  while (product < target) // because 13! > 32 bit int limit
  {
    ++index;
    product = index * product;
  }

  long *result = malloc(sizeof(int) * 2);
  result[0] = product;
  result[1] = (long)index;

  return result; // Returns a mem adress
}

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    long num[1];
    int result = stringToDecimal(argv[1], &num[0]);
    
    if (result)
    {
      perror("First arg isn't a number");
      exit(EXIT_FAILURE);
    }
    
    if (num[0] < 0)
    {
      perror("No factorial below 0");
      exit(EXIT_FAILURE);
    }

    long *nearest = calc_nearest_factorial(num[0]); // Here is explicit conversion from long -> int; which is BAD !!
    
    printf("Product : %ld, Factorial : %ld \n", nearest[0], nearest[1]);
    free(nearest);

    exit(EXIT_SUCCESS);
  }
  else
  {
    perror("Not enought arguments");
    exit(EXIT_FAILURE);
  }
}