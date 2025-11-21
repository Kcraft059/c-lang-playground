#include "snake/core.h"
#include <snake/snake.h> // Reference
// ----------------------------
#include <array.h> // Personnal lib
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Private functions def

hash __hashCoordMap(void* coos);                                 // Maps coordinate to a prehash as a hash key
hash __hashUpdtType(void* updtType);                             // Maps updtType to hash
bool __hashItemFreeUpdtHandler(bucketItem* self, void* extData); // Frees given handler object
void __freeTile(board* targetBoard, tile* self);                 // Free tile of board
tile* __allocTile(board* targetBoard);                           // Free tile of board

// Board properties
board* snBInitBoard(int size_x, int size_y, Allocator* a) { // Should create a new board
  board* self = a->alloc(sizeof(board));

  if (self) { // Allocates memory for different objects of the board
    self->size_x = size_x;
    self->size_y = size_y;
    self->allc = a;
    self->tiles = array(tile*, a);
    self->snakes = array(snake*, a);
    self->updates = array(update, a);
    self->snakeMap = hashmap(__hashCoordMap, a);
    self->tileMap = hashmap(__hashCoordMap, a);
    self->updateHandlers = hashmap(__hashUpdtType, a);
    self->addData = self->dataFree = NULL; // Board init doesn't handle this
  }

  return self;
}

void snBDeleteBoard(board* self) { // Should destroy a board and all related instances
  if (!self) return;

  Allocator* a = self->allc;

  // Object deletion isn't needed here, because tiles are in the array, they're not referenced in the array
  for (int i = 0; i < array_length(self->tiles); ++i) { // Free each tile in board
    __freeTile(self, self->tiles[i]);                   // Free tile from memory
  }
  array_delete(self->tiles);
  hashmap_delete(self->tileMap);

  /* for (int i = 0; i < array_length(self->snakes); ++i) { // Free each snake in board
    snSDeleteSnake(self->snakes[i]);
  } */
  array_delete(self->snakes);
  hashmap_delete(self->snakeMap);

  snBUClear(self); // Clears all updates & frees them from mem
  array_delete(self->updates);

  hashmap_actOnEach(self->updateHandlers, __hashItemFreeUpdtHandler, self->allc);
  hashmap_delete(self->updateHandlers);

  a->free(self); // Finaly free board object when all sub componentns were freed
}

void snBResizeBoard(board* self, int size_x, int size_y) { // Resizes board limits
  self->size_x = size_x;                                   // Resizes Board
  self->size_y = size_y;

  bResizeUpdt* updt = self->allc->alloc(sizeof(bResizeUpdt)); // Prepare update payload
  updt->size_x = size_x;
  updt->size_y = size_y;

  snBUAdd(self, &(update){.type = BOARD_RESIZE_UPDT, // Sends board resize Update
                          .payload = updt,
                          self->allc->free});
}

// Board Update
void snBUClear(board* targetBoard) {                             // Reset all board updates
  for (int i = 0; i < array_length(targetBoard->updates); ++i) { // Free each udpate payload
    update crtUpdt = targetBoard->updates[i];
    if (crtUpdt.free) crtUpdt.free(crtUpdt.payload);
  }

  array_resize(&targetBoard->updates, 0); // Set size of updates to 0 = clear updates
}

void snBUAdd(board* targetBoard, update* self) { // Adds a new update to the board
  array_append(&targetBoard->updates, self);     // Can directly pass self since it's copied and does not go out of scope
}

bool snBURemove(board* targetBoard, size_t index) {  // Remove update from list of updates in board
  return array_remove(&targetBoard->updates, index); // Return deletion success
}

bool snBURegisterHandler(board* targetBoard, updateHandler handler, uint64_t targetType) { // Adds an update type and related infos / handler
  updateHandler* self = targetBoard->allc->alloc(sizeof(updateHandler));                   // Alloc mem for handler
  *self = handler;

  if (!self) return false;

  hashmap_add(targetBoard->updateHandlers, self, &targetType); // Todo handle replacement warning
  return true;
}

bool snBURemoveHandler(board* targetBoard, uint64_t targetType) { // Removes the associated handler for type
  updateHandler* handler;
  if ((handler = snBUGetHandler(targetBoard, targetType)))
    targetBoard->allc->free(handler);

  return hashmap_remove(targetBoard->updateHandlers, &targetType);
}

updateHandler* snBUGetHandler(board* targetBoard, uint64_t targetType) { // Get the associated handler for an updatetype
  return hashmap_get(targetBoard->updateHandlers, &targetType);
}

// Board pos system
coords snBRandomPos(board* targetBoard);                     // Get a random coord on board
coords snBTranslatePos(board* targetBoard, coords position); // Translates a coordinate out of board
void* snBCheckSnake(board* targetBoard, coords position);    // Check for snake entity at pos
void* snBCheckTile(board* targetBoard, coords position);     // Check for snake entity at pos

// Board objects
bool snBAddTile(board* targetBoard, tile self) {                         // Adds a given tile to board
  if (hashmap_get(targetBoard->tileMap, &self.coordinate)) return false; // Can't add to an already allocated pos

  tile* newTile = __allocTile(targetBoard);
  *newTile = self;

  array_append(targetBoard->tiles, &newTile);                       // Adds the tile to the baord tiles
  hashmap_add(targetBoard->tileMap, newTile, &newTile->coordinate); //  Register tile at pos

  tileStatusUpdt* updt = targetBoard->allc->alloc(sizeof(tileStatusUpdt)); // Prepare update payload
  updt->targetTile = newTile;

  snBUAdd(targetBoard, &(update){.type = TILE_STATUS_UPDT, // Sends tile status update
                                 .payload = updt,
                                 targetBoard->allc->free});

  return true;
}

bool snBDelTile(board* targetBoard, coords pos) {             // Removes tile at coords from board
  if (!hashmap_get(targetBoard->tileMap, &pos)) return false; // Can't delete if nothing at pos

  for (int i = 0; i < array_length(targetBoard->tiles); ++i) {
    if (memcmp(&(targetBoard->tiles[i]->coordinate), // If positions match
               &(pos),
               sizeof(coords)) == 0) {
      __freeTile(targetBoard, targetBoard->tiles[i]); // Free tile from memory
      array_remove(&targetBoard->tiles, i);           // Delete pointer from tile list

      tileRemoveUpdt* updt = targetBoard->allc->alloc(sizeof(tileRemoveUpdt)); // Prepare update payload
      updt->pos = pos;

      snBUAdd(targetBoard, &(update){.type = TILE_REMOVE_UPDT, // Sends tile remove update
                                     .payload = updt,
                                     targetBoard->allc->free});

      return true;
    }
  };

  return false;
};

void snBAddSnake(board* targetBoard, snake* self); // Adds a snake to the board
void snBDelSnake(board* targetBoard, snake* self); // Removes the snake at index of the board

// Snake
snake* snSInitSnake(coords pos);                   // Inits a snake at a given position
void snSDeleteSnake(snake* self);                  // Free snake from memory
void snSSetSize(snake* self, int size);            // Sets snake size
void snSMoveHeadPos(snake* self, coords position); // Updates the snake coordinates
int snSGetSize(snake* self);                       // Get the actual size of the snake

/// Private helpers

hash __hashCoordMap(void* coos) { // Maps coordinate to a prehash as a hash key
  coords* coo = coos;
  return (uint64_t)coo->x << 32 | (uint32_t)coo->y;
}

hash __hashUpdtType(void* updtType) { // Maps updtType to hash
  return *(hash*)updtType;
}

bool __hashItemFreeUpdtHandler(bucketItem* self, void* extData) { // Frees given handler object
  ((Allocator*)extData)->free(self->ptr);
  return true;
}

void __freeTile(board* targetBoard, tile* self) { // Free tile of board
  targetBoard->allc->free(self);
}

tile* __allocTile(board* targetBoard) { // Free tile of board
  return targetBoard->allc->alloc(sizeof(tile));
}