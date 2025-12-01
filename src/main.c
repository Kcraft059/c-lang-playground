#include <debug.h>
#include <array.h>
#include <snake/snake.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Allocator* all = &debugAllc;
// static Allocator* all = &(Allocator){malloc, realloc, free};

int main(int argc, char** argv) {
  board* mainBoard = snBInitBoard(150, 85, all);

  for (uint64_t i = 0; i < 500; ++i) {
    snBAddTile(mainBoard, (tile){(coords){
                              i / (mainBoard->size_y + 1),
                              i % (mainBoard->size_y + 1)}});
  }

  hashMap** exclusions = array(hashMap*, mainBoard->allc);
  array_append(&exclusions, &mainBoard->tileMap);

  coords co;
  if (snBRandomPos(mainBoard, exclusions, &co))
    printf("[coord] (%d;%d)\n", co.x, co.y);

  array_delete(exclusions);

  snBDeleteBoard(mainBoard);
  allocStatsPrint();
}