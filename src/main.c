#include "snake/core.h"
#include <array.h>
#include <stdio.h>
#include <stdlib.h>

static Allocator* all = &(Allocator){
    malloc, realloc, free};

int main(int argc, char** argv) {
  board* mainBoard = snBInitBoard(12, 12, all);

  snBDeleteBoard(mainBoard);
}
