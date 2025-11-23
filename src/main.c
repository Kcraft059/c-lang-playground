#include "snake/core.h"
#include <array.h>
#include <snake/snake.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int allocs = 0; // Debug
int entropy = 0;

void* fmalloc(size_t size) {
  void* self = malloc(size);
  printf("[mllc] P: %p S:%ld\n", self, size);
  allocs++;
  entropy++;
  return self;
};

void* frealloc(void* ptr, size_t size) {
  void* self = realloc(ptr, size);
  printf("[rllc] P: %p NP:%p NS:%ld\n", ptr, self, size);
  entropy++;
  return self;
};

void ffree(void* ptr) {
  free(ptr);
  printf("[free] P: %p\n", ptr);
  allocs--;
}

static Allocator* all = &(Allocator){
    // fmalloc, frealloc, ffree};
    malloc, realloc, free};

int main(int argc, char** argv) {
  board* mainBoard = snBInitBoard(150, 85, all);

  for (uint64_t i = 0; i < 1; ++i) {
    snBAddTile(mainBoard, (tile){(coords){
                              i / (mainBoard->size_y + 1),
                              i % (mainBoard->size_y + 1)}});
  }

  hashMap** exclusions = array(hashMap*, mainBoard->allc);
  array_append(&exclusions, &mainBoard->tileMap);

  coords co;
  if (snBRandomPos(mainBoard, exclusions, &co))
    printf("%d;%d\n", co.x, co.y);

  array_delete(exclusions);

  snBDeleteBoard(mainBoard);
  printf("Remaining Allocs: %d Entropy: %d\n", allocs, entropy);
}