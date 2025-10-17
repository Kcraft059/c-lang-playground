#ifndef FUNC_H
#define FUNC_H

// In .h files declared functions are shared between objects files 

long exponent(int, int);
int stringToDecimal(char *string, long *outputVar); // Will be available on both components when linking files

#endif