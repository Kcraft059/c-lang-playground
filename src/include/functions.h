#pragma once // Prevennts : duplicate symbol '<func>' in:

// In .h files declared functions are shared between objects files

// Maths

long exponent(int value, int exponent);

// String manipulation

int stringToDecimal(char *string, long *outputVar); // Will be available on both components when linking files

// Array Manipulation

typedef struct
{
  int *array;
  int length;
} DynamicArray;

int addToDynArray(DynamicArray *originalArray, int value, int method);