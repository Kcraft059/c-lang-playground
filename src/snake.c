#include <stdbool.h>
#include <stdlib.h>
/// Personnal Libs
#include <array.h>
#include <snake.h>

/// Global Var definition

board* snStdBoard = NULL;

/// Public functions

board* snBInitBoard() {  // Creates a board in memory
  board* self = malloc(sizeof(board));
  return self;
};

void snBResizeBoard(board* self, int x, int y) {  // Sets the game board borders to a defined size
  self->size_x = x;
  self->size_y = y;
}

/// Private functions