#ifndef ALLC_DEBUG_H
#define ALLC_DEBUG_H
#include <array.h>

enum debugLevel{
  NONE,
  VERBOSE
};

extern Allocator debugAllc;

extern enum debugLevel debugLvl;
extern int allocs;
extern int entropy;

void allocStatsPrint();

#endif