#include <array.h>
#include <snake/snake.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fmalloc, frealloc, ffree};
// malloc, realloc, free};

void testHandler(void* self, board* targetBoard) {
  printf("Int : %d\n", *(int*)self);
}

void resizeHandler(void* self, board* targetBoard) {
  bResizeUpdt* updt = (bResizeUpdt*)self;
  printf("Resized to %d,%d\n", updt->size_x, updt->size_y);
}

int main(int argc, char** argv) {
  board* mainBoard = snBInitBoard(12, 12, all);
  printf("Board init end\n");

  for (int i = 0; i < 2; ++i) {
    int* ptr = all->alloc(sizeof(int));
    *ptr = i;
    snBUAdd(mainBoard, &(update){
                           *(uint64_t*)"TST_UP\1",
                           ptr,
                           all->free});
  }

  snBAddTile(mainBoard, (tile){(coords){2, 3}, NULL});
  snBAddTile(mainBoard, (tile){(coords){3, 3}, NULL});
  snBAddTile(mainBoard, (tile){(coords){5, 3}, NULL});

  snBDelTile(mainBoard, (coords){2, 3});

  snBResizeBoard(mainBoard, 45, 32);

  snBURegisterHandler(mainBoard,
                      (updateHandler){resizeHandler},
                      *(uint64_t*)"BRD_RES\0");

  snBURegisterHandler(mainBoard,
                      (updateHandler){testHandler},
                      *(uint64_t*)"TST_UP\1");

  updateHandler* handler;
  for (int i = 0; i < array_length(mainBoard->updates); ++i)
    if ((handler = snBUGetHandler(mainBoard,
                                  mainBoard->updates[i].type)) &&
        handler->apply)
      handler->apply(mainBoard->updates[i].payload, mainBoard);

  printf("Start board clear\n");
  snBDeleteBoard(mainBoard);
  printf("Remaining Allocs: %d Entropy: %d\n", allocs, entropy);
}