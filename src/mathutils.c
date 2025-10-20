#include <functions.h>

long exponent(int value, int exponent)
{
  long returnVal = 1;
  for (int i = 0; i < exponent; i++)
  {
    returnVal = returnVal * value;
  }

  return returnVal;
}