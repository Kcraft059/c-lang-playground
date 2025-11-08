#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/// Personnal Libs
#include <array.h>
#include <snake.h>

/// Var definition

// Global
board* snStdBoard = NULL;

// Local
Allocator allc = {
    .alloc = malloc,
    .realloc = realloc,
    .free = free};

/// Public functions

// Board properties
board* snBInitBoard() {                // Creates a board in memory
  board* self = malloc(sizeof(board)); // Allocate memory for board object
  self->snakes = array(snake*, &allc); // Inits a dynamic array of ptr for snakes in the board
  self->tiles = array(tile, &allc);    // Inits a dynamic array for all board tiles

  return self;
};

void snBResizeBoard(board* self, int x, int y) { // Sets the game board borders to a defined size
  self->size_x = x;
  self->size_y = y;
}

void snBDeleteBoard(board* self) {
  // TODO Handle snake deletion before free

  for (int sni = 0; sni < array_length(self->snakes); ++sni) { // Delete each snake on board
    snSDeleteSnake(self->snakes[sni]);
  }

  array_delete(self->snakes); // Frees dynamic arrays
  array_delete(self->tiles);

  free(self);
}

// Board pos system
coords snBRngBlankPos(board* targetBoard);

// Board tile
void snBAddTile(board* targetBoard, tile self);          // Adds a given tile to board
void snBDelTile(board* targetBoard, coords pos);         // Removes tile at coords from board
bool snBCheckTile(board*, coords pos, tile* returnTile); // Check for tile at pos, if none return false

// Board snake
void snBAddSnake(board* targetBoard, snake* self) { // Adds a snake to the board
  array_append(&(targetBoard->snakes), &self);
}

void snBDelSnake(board* targetBoard, size_t index) { // Removes the snake at index of the board
  array_remove(&(targetBoard->snakes), index);
}

// Snake
snake* snSInitSnake(coords pos) {      // Inits a snake at a given position
  snake* self = malloc(sizeof(snake)); // Alloc size for snkae struct
  self->coords = array(coords, &allc);

  // Set defaults
  self->score = 0;
  self->direction = MV_STILL;          // Not moving
  array_append(&(self->coords), &pos); // Set first item of coords to initial position

  return self;
}

void snSDeleteSnake(snake* self) { // Free snake from memory
  array_delete(self->coords);
  free(self);
}

void snSSetSize(snake* self, int size) { // Sets snake size
  int ogSize = snSGetLength(self);

  if (ogSize == size) return; // Return if no resize is necessary

  if (ogSize > size) {
    for (int i = 0; i < ogSize - size; ++i) { // Reduce length by popping last coords
      array_pop(&(self->coords));
    }
  } else {
    coords lastCoord = self->coords[ogSize - 1]; // Get last item of original coordinate list

    for (int i = 0; i < size - ogSize; ++i) { // Increase length by duplicating last item of coords list
      array_append(&(self->coords), &lastCoord);
    }
  }
}

void snSMoveHeadPos(snake* self, coords position) { // Updates the snake coordinates
  coords** coordArray = &(self->coords);
  array_add(coordArray, 0, &position); // Push new position to start of array
  array_pop(coordArray);               // Remove last value
}

int snSGetLength(snake* self) {
  return array_length(self->coords);
}

bool snSCheckSnake(snake* self, coords position) { // Check for snake presence
  int size = snSGetLength(self);
  for (int i = 0; i < size; ++i) {
    if (!memcmp(self->coords + i, &position, sizeof(coords))) return true; // Compare coords object in memory to check for a match
  }
  return false;
}

coords snSComputeHeadPos(snake* self, enum movType direction) { // Computes the value of the next snake positions of the snake
  coords headCoord = self->coords[0];
  /* INCLUDE SWITCH */
  return headCoord;
}

// Board status
board* snBMakeSnapshot(board* object);               // Deep memory duplication, returns image
                                                     // This should be done before starting to make changes
board* snBDiffSnapshot(board* image, board* object); // Get delta between image and object