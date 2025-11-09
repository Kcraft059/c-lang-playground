#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/// Personnal Libs
#include <array.h>
#include <snake.h>

/// Definition

// External
board* snStdBoard = NULL;

// Local
board* __lastGetSnakeBoard; // Last checked board
int __NextGetSnakeIndex;    // Last index reached

const Allocator allc = {
    .alloc = malloc,
    .realloc = realloc,
    .free = free};

void __snBDelSnake(board* targetBoard, size_t index); // Removes the snake at index of the board
/// Public functions

// Board properties
board* snBInitBoard(int absx, int absy) { // Creates a board in memory
  board* self = malloc(sizeof(board));    // Allocate memory for board object
  self->snakes = array(snake*, &allc);    // Inits a dynamic array of ptr for snakes in the board
  self->tiles = array(tile, &allc);       // Inits a dynamic array for all board tiles

  coords absPos = {absx, absy};
  self->absPos = absPos;

  return self;
};

void snBResizeBoard(board* self, int x, int y) { // Sets the game board borders to a defined size
  self->size_x = x;
  self->size_y = y;
}

void snBDeleteBoard(board* self) {
  // TODO Handle snake deletion before free

  snake* ptr;
  snBGetSnake(self);                // Init target board
  while ((ptr = snBGetSnake(NULL))) // For each snake in board
    snSDeleteSnake(ptr);

  array_delete(self->snakes); // Frees dynamic arrays
  array_delete(self->tiles);

  free(self);
}

// Board pos system
coords snBGetAbsPos(board* targetBoard, coords position) { // Get the real position of a coord on screen
  position.x += targetBoard->absPos.x;
  position.y += targetBoard->absPos.y;
  return position;
}

coords snBRngPos(board* targetBoard) { // Get a random coord on board
  coords rngCoord;

  rngCoord.x = rand() % targetBoard->size_x + 1; // Give a random x value between 0, and board limit
  rngCoord.y = rand() % targetBoard->size_y + 1; // "" y

  return rngCoord;
}

coords snBTranslatePos(board* targetBoard, coords position) { // Translates a coordinate out of board
  coords p = position;
  int lx = targetBoard->size_x;
  int ly = targetBoard->size_y;

  // Modify new pos to account board limits
  p.x += (p.x >= 0) ? -(lx + 1) * ((p.x - 1) / lx) : (lx + 1) * ((p.x / lx) + (p.x % lx ? 1 : 0));
  p.y += (p.y >= 0) ? -(ly + 1) * ((p.y - 1) / ly) : (ly + 1) * ((p.y / ly) + (p.y % ly ? 1 : 0));

  return p;
}

// Board tile
void snBAddTile(board* targetBoard, tile self) { // Adds a given tile to board
  array_append(&(targetBoard->tiles), &self);
}

void snBDelTile(board* targetBoard, coords pos) { // Removes tile at coords from board
  tile** tilearray = &(targetBoard->tiles);

  for (int i = 0; i < array_length(*tilearray); ++i) {
    if (!memcmp(&((*tilearray)[i].coordinate), &pos, sizeof(coords))) { // If matching coords
      array_remove(tilearray, i);
      return;
    }
  }
}

bool snBCheckTile(board* targetBoard, coords pos, tile* returnTile) { // Check for tile at pos, if none return false
  tile* tilearray = targetBoard->tiles;

  for (int i = 0; i < array_length(tilearray); ++i) {
    if (!memcmp(&(tilearray[i].coordinate), &pos, sizeof(coords))) { // If matching coords
      if (returnTile) *returnTile = tilearray[i];
      return true;
    }
  }

  return false;
};

// Board snake
void snBAddSnake(board* targetBoard, snake* self) { // Adds a snake to the board
  array_append(&(targetBoard->snakes), &self);
}

snake* snBGetSnake(board* targetBoard) { // Gets all the snakes on the board
  snake* snakePtr;

  if (targetBoard) { // If there's a targetBoard specified, set as iterated board
    __lastGetSnakeBoard = targetBoard;
    __NextGetSnakeIndex = 0;

  } else if (__lastGetSnakeBoard && // If there's a board specified and that we did not reach the end yet
             __NextGetSnakeIndex < array_length(__lastGetSnakeBoard->snakes)) {
    return __lastGetSnakeBoard->snakes[__NextGetSnakeIndex++];

  } else { // Reset
    __lastGetSnakeBoard = NULL;
    __NextGetSnakeIndex = 0;
  }

  return NULL;
}

void snBDelSnake(board* targetBoard, snake* self) {
  for (int i = 0; i < array_length(targetBoard); ++i) {
    if (self == targetBoard->snakes[i]) __snBDelSnake(targetBoard, i);
  }
}

void __snBDelSnake(board* targetBoard, size_t index) { // Removes the snake at index of the board
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
  direction = direction ? direction : self->direction;

  // Set new coords
  switch (direction) {
  case MV_RIGHT:
    headCoord.x += 1;
    break;
  case MV_LEFT:
    headCoord.x -= 1;
    break;
  case MV_DOWN:
    headCoord.y += 1;
    break;
  case MV_UP:
    headCoord.y -= 1;
    break;
  default:
    break;
  }

  return headCoord;
}

// Board status
board* snBMakeSnapshot(board* object);               // Deep memory duplication, returns image
                                                     // This should be done before starting to make changes
board* snBDiffSnapshot(board* image, board* object); // Get delta between image and object