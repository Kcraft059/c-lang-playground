#include <stdbool.h>

#include "definitions.h"

int splitString(char* inString, char delimiter, int startIndex, int stopIndex, char* outString) {
  int rIndx = 0;
  int wIndx = 0;
  int delimiterOccurence = 0;

  bool success = true;
  while (1) {
    char chr = inString[rIndx];

    if (chr == delimiter) {
      ++delimiterOccurence;

      if (delimiterOccurence == startIndex) {
        wIndx = -1;
      }
      if (delimiterOccurence == stopIndex) {
        outString[wIndx] = '\0';
        break;
      }
      // when reaching startIndex -> reset wIndex
      // when reaching stopIndex -> stop loop & write = '\0'
    }

    if (chr == '\0') {  // When reaching end of string end loop
      outString[wIndx] = '\0';
      // If startIndex isn't reached or (stopIndex is reachable (!= -1) and not at the end of string and has not been reached)
      if (delimiterOccurence < startIndex || (stopIndex != -1 && stopIndex - 1 != delimiterOccurence && delimiterOccurence < stopIndex))
        success = 0;
      break;
    } else if (wIndx != -1) {  // Prevent write at outString[-1] out of bound write -> memory corruption on stack
      outString[wIndx] = chr;
    }

    // Move read & write index
    ++rIndx;
    ++wIndx;
  };

  if (success) return 1;  // If found a matching string at index, no error

  outString[0] = '\0'; // Blank string if not found
  return 0;  // In any other case, there's an error
}