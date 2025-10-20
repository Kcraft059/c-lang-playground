#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functions.h>

int addToDynArray(DynamicArray *originalArray, int value, int method)
{ // Method 1:append 2:prepend

  if (originalArray->length == 0)
  {
    originalArray->array[0] = value;
    originalArray->length = 1;
  }
  else
  {
    int ogLength = originalArray->length++;
    int *newArray = malloc(sizeof(int) * originalArray->length);

    if (method == 1) // Append by copying the Og array to start of new array with length of the Og array
    {
      memcpy(newArray, originalArray->array, sizeof(int) * ogLength);
      newArray[ogLength] = value;
    }
    else if (method == 2) // Prepend by copying the Og array to start of new array + 1 element with length of the Og array
    {
      memcpy(newArray + 1, originalArray->array, sizeof(int) * ogLength);
      // memcpy(<destination pointer> + number of element offset,<source pointer>,<length in bytes to read> )
      newArray[0] = value;
    }

    free(originalArray->array);
    originalArray->array = newArray;
  }

  return 0;
}

int stringToDecimal(char *string, long *outputVar)
{
  int *charValue[2];
  int possibleChars[] = {(int)'0', (int)'1', (int)'2', (int)'3', (int)'4', (int)'5', (int)'6', (int)'7', (int)'8', (int)'9'}; // var[] = {} only on heap if trying to do *var = {}, not possible
  int charAssoc[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  charValue[0] = possibleChars;
  charValue[1] = charAssoc;

  DynamicArray numericChars = {malloc(sizeof(int)), 0};
  int8_t negative = 0;

  for (int strlen = 0; (int)string[strlen] != 0; strlen++)
  {
    char currentChar = string[strlen];
    int charIndex = -1;

    if (currentChar == (int)'-' && strlen == 0) // If first char is '-' make negative
    {
      negative = 1;
    }
    else
    {
      for (int i = 0; i < 10; i++) // Check every valid chars + store index if valid
      {
        if (charValue[0][i] == (int)currentChar)
        {
          charIndex = i;
          break;
        }
      };

      if (charIndex != -1)
      {
        addToDynArray(&numericChars, charIndex, 2); // Found valid number, with indexed value of charIndex
      }
      else
      {
        free(numericChars.array);
        return 1;
      }
    }
  };

  long result = 0;
  for (int i = 0; i < numericChars.length; i++)
  {
    int charIndex = numericChars.array[i];
    int charValue = charAssoc[charIndex];

    result += (long)charValue * exponent(10, i);
  }
  if (negative)
    result = -result;

  *outputVar = result;

  free(numericChars.array);
  return 0;
}