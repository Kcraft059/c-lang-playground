#include <stdio.h>
#include <functions.h> // Is needed

int * calc_nearest_factorial(int target)
{
  int product = 1;
  int index = 0;

  while (product < target)
  {
    index++;
    product = index * product;
  }

  /* int result[2];
  result[0] = product;
  result[1] = index; */

  int result[] = {product,index};

  return result;
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
      return 1;
    }

    int *nearest = calc_nearest_factorial(num[0]);
    printf("%d \n",nearest[1]);
    // printf("Argv[1] %ld \n", num[1]);
  }
  else
  {
    perror("Not enought arguments");
    return 1;
  }
}