#include "snake/core.h"
#include <snake/snake.h> // Reference
// ----------------------------
#include <array.h> // Personnal lib
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_u_int64_t.h>
#include <time.h>

/// Private functions def

// Pre-hashing fuctions
hash __hashCoordMap(void* coos);                                 // Maps coordinate to a prehash as a hash key
hash __hashUpdtType(void* updtType);                             // Maps updtType to hash
bool __hashItemFreeUpdtHandler(bucketItem* self, void* extData); // Frees given handler object

// Tile related functions
void __freeTile(board* targetBoard, tile* self); // Free tile of board
tile* __allocTile(board* targetBoard);           // Free tile of board

// Pos functions
bool __posCheckValidity(board* targetBoard, coords pos);              // Check if pos is in board
uint64_t __rngIntPosEncoded(board* targetBoard, uint64_t excludeNum); // Sends back a random int (x * bx + y) corresponding to a pos on board
uint64_t __IntPosEncode(board* targetBoard, coords pos);
coords __IntPosDecode(board* targetBoard, uint64_t intPos);

/// Public function implementation
// Board properties
board* snBInitBoard(int size_x, int size_y, Allocator* a) { // Should create a new board
  board* self = a->alloc(sizeof(board));

  if (self) { // Allocates memory for different objects of the board
    self->size_x = size_x;
    self->size_y = size_y;
    self->rngSeed = time(NULL);
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
  for (int i = 0; i < array_length(self->tiles); ++i) // Free each tile in board
    __freeTile(self, self->tiles[i]);                 // Free tile from memory

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

bool snBURemove(board* targetBoard, size_t index) { // Remove update from list of updates in board
  if (index <= array_length(targetBoard->updates)) {
    update crtUpdt = targetBoard->updates[index];
    array_remove(&targetBoard->updates, index);
    if (crtUpdt.free) crtUpdt.free(crtUpdt.payload);
    return true;
  }
  return false;
}

bool snBURegisterHandler(board* targetBoard, updateHandler handler, uint64_t targetType) { // Adds an update type and related infos / handler
  updateHandler* self = targetBoard->allc->alloc(sizeof(updateHandler));                   // Alloc mem for handler
  *self = handler;

  if (!self) return false;

  hashmap_add(targetBoard->updateHandlers, self, &targetType); // Passinf '&' ref is fine, since key is copied
  // Todo handle replacement warning
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

/**
 *
 * Warning this still needs much improvement (is not made to handle huge quantity of tiles)
 *
 */

/// ------ Random POS implementation ------
struct addDataStoreInt {
  hashMap* excludedPos; // Ensures data uniqueness
  uint64_t exclusionC;
  board* targetBoard;
};

struct addDataRemapInt {
  hashMap* excludedPos; // Ensures data uniqueness
  uint64_t exclusionC;
  uint64_t searchIdx;
  board* targetBoard;
};

static hash prehashIntPos(void* key) {
  return *(hash*)key;
}

static uint8_t DUMMY_VALUE_PTR = 0;

static bool storeIntPos(bucketItem* self, void* extData) { // Store pos of item in hashmap into excluded position hashmap
  struct addDataStoreInt* data = extData;
  uint64_t intPos = __IntPosEncode(data->targetBoard,
                                   (coords){// Refer to __hashCoordMap(void* coos)
                                            (uint32_t)(self->cached_prehash >> 32),
                                            (uint32_t)self->cached_prehash});

  if (!hashmap_get(data->excludedPos, &intPos)) { // If pos isn't already excluded
    hashmap_add(data->excludedPos,
                &DUMMY_VALUE_PTR, &intPos); // No data, just to prevent NULL ptr
    data->exclusionC++;
  }

  return true;
}

bool remapIntPos(bucketItem* self, void* extData) {
  struct addDataRemapInt* data = extData;
  uint64_t idxMax = (data->targetBoard->size_x + 1) * (data->targetBoard->size_y + 1); // Total number of possible values

  if (self->cached_prehash < idxMax - data->exclusionC) { // If self is in range of chosable values
    bool valid = false;
    while (!valid) {
      if (!hashmap_get(data->excludedPos, &data->searchIdx)) { // Try until search idx is a blank pos (not in excldued pos)
        self->ptr = data->targetBoard->allc->alloc(sizeof(uint64_t));
        *(uint64_t*)self->ptr = data->searchIdx; // Set mapping to a value blank value (searchIdx)
        valid = true;
      }
      data->searchIdx++; // Else try next search Index
    }
  } /* else {
    // Should plan bucket removal
  } */

  return true;
}

bool freeIntPos(bucketItem* self, void* extData) {
  if (self->ptr != &DUMMY_VALUE_PTR) ((Allocator*)extData)->free(self->ptr);
  return true;
}

void snBRegenPosMap(board* targetBoard) {}
void snBUpdatePosMap(board* targetBoard) {}

bool snBRandomPos(board* targetBoard, hashMap** exclusions, coords* storePos) { // Get a random coord on board not present in exclusion maps
  if (!exclusions) {
    *storePos = __IntPosDecode(targetBoard, __rngIntPosEncoded(targetBoard, 0));
    return true;
  }

  struct addDataStoreInt adtStint = (struct addDataStoreInt){
      hashmap(prehashIntPos, targetBoard->allc),
      0,
      targetBoard};
  for (int map = 0; map < array_length(exclusions); map++)
    hashmap_actOnEach(exclusions[map], storeIntPos, &adtStint);

  if ((targetBoard->size_x + 1) * (targetBoard->size_y + 1) - adtStint.exclusionC <= 0) return false;

  struct addDataRemapInt adtRmpInt = (struct addDataRemapInt){
      adtStint.excludedPos,
      adtStint.exclusionC,
      (targetBoard->size_x + 1) * (targetBoard->size_y + 1) - adtStint.exclusionC,
      targetBoard};
  hashmap_actOnEach(adtRmpInt.excludedPos, remapIntPos, &adtRmpInt);

  uint64_t rngValue = __rngIntPosEncoded(targetBoard, adtRmpInt.exclusionC);
  void* newRngValue;
  if ((newRngValue = hashmap_get(adtRmpInt.excludedPos, &rngValue)))
    rngValue = *(uint64_t*)newRngValue;

  hashmap_actOnEach(adtRmpInt.excludedPos, freeIntPos, targetBoard->allc);
  hashmap_delete(adtRmpInt.excludedPos);

  *storePos = __IntPosDecode(targetBoard, rngValue);
  return true;
}

/// ------ End Random POS implementation ------

coords snBTranslatePos(board* targetBoard, coords position) { // Translates a coordinate out of board
  coords p = position;
  int lx = targetBoard->size_x + 1;
  int ly = targetBoard->size_y + 1;

  // Modify new pos to account board limits
  p.x = (p.x >= 0) ? p.x % lx : (p.x % (lx) == 0 ? -lx : p.x % (lx)) + lx;
  p.y = (p.y >= 0) ? p.y % ly : (p.y % (ly) == 0 ? -ly : p.y % (ly)) + ly;

  return p;
}

inline void* snBCheckSnake(board* targetBoard, coords pos); // Check for snake entity at pos

inline void* snBCheckTile(board* targetBoard, coords pos) { // Check for snake entity at pos
  return hashmap_get(targetBoard->tileMap, &pos);
}

// Board objects
bool snBAddTile(board* targetBoard, tile self) { // Adds a given tile to board
  if (snBCheckTile(targetBoard, self.coordinate) ||
      !__posCheckValidity(targetBoard, self.coordinate)) return false; // Can't add to an already allocated pos

  tile* newTile = __allocTile(targetBoard);
  *newTile = self;

  array_append(&targetBoard->tiles, &newTile);                      // Adds the tile to the baord tiles
  hashmap_add(targetBoard->tileMap, newTile, &newTile->coordinate); //  Register tile at pos

  tileStatusUpdt* updt = targetBoard->allc->alloc(sizeof(tileStatusUpdt)); // Prepare update payload
  updt->targetTile = newTile;

  snBUAdd(targetBoard, &(update){.type = TILE_STATUS_UPDT, // Sends tile status update
                                 .payload = updt,
                                 targetBoard->allc->free});

  return true;
}

bool snBDelTile(board* targetBoard, coords pos) {    // Removes tile at coords from board
  if (!snBCheckTile(targetBoard, pos)) return false; // Can't delete if nothing at pos

  hashmap_remove(targetBoard->tileMap, &pos);

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
snake* snSInitSnake(coords pos);              // Inits a snake at a given position
void snSDeleteSnake(snake* self);             // Free snake from memory
void snSSetSize(snake* self, int size);       // Sets snake size
void snSMoveHeadPos(snake* self, coords pos); // Updates the snake coordinates
int snSGetSize(snake* self);                  // Get the actual size of the snake

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

bool __posCheckValidity(board* targetBoard, coords pos) { // Is given pos in board limits ?
  return pos.x <= targetBoard->size_x && pos.x >= 0 &&
         pos.y <= targetBoard->size_y && pos.y >= 0;
}

uint64_t __rngIntPosEncoded(board* targetBoard, uint64_t excludeNum) { // Sends back a random int (x * bx + y) corresponding to a pos on board
  srand(targetBoard->rngSeed);

  uint64_t posIndexMax = (targetBoard->size_x + 1) * (targetBoard->size_y + 1) - excludeNum;
  return ((uint64_t)rand() << 32 | rand()) % posIndexMax;
}

inline uint64_t __IntPosEncode(board* targetBoard, coords pos) {
  return pos.x * (targetBoard->size_y + 1) + pos.y;
}

inline coords __IntPosDecode(board* targetBoard, uint64_t intPos) { // Decode a intPos to standard coordinates
  return (coords){intPos / (targetBoard->size_y + 1),               // inverse of doing x * by + y
                  intPos % (targetBoard->size_y + 1)};
}