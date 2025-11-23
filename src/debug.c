#include <debug.h>
#include <stdio.h>
#include <stdlib.h>

int allocs = 0;
int entropy = 0;
enum debugLevel debugLvl = NONE;

void* fmalloc(size_t size) {
  void* self = malloc(size);
  if (debugLvl == VERBOSE) printf("[mllc] P: %p S:%ld\n", self, size);
  allocs++;
  entropy++;
  return self;
};

void* frealloc(void* ptr, size_t size) {
  void* self = realloc(ptr, size);
  if (debugLvl == VERBOSE) printf("[rllc] P: %p NP:%p NS:%ld\n", ptr, self, size);
  entropy++;
  return self;
};

void ffree(void* ptr) {
  free(ptr);
  if (debugLvl == VERBOSE) printf("[free] P: %p\n", ptr);
  allocs--;
}

Allocator debugAllc = (Allocator){
    fmalloc, frealloc, ffree};

void allocStatsPrint() {
  printf("[stat] Remaining allocs: %d, Entropy generated: %d\n", allocs, entropy);
}