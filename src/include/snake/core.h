/**-----------------------------------------------------------------------*
 * Snake core logic definition module by @kcraft059
 *
 * This module defines the behaviour of the core logic for the snake game,
 * it implements all board management related functions, entity control, 
 * update logic etc on which an engine can relly to implement game behaviour
 *
 * No copyright - Last updated: 16/11/2025 
 *-----------------------------------------------------------------------**/
 
#ifndef SNAKE_CORE_H
#define SNAKE_CORE_H

#include <array.h> // Personnal array lib
#include <stdbool.h>

/// Definitions
// Update types
#define BOARD_RESIZE_UPDT (*(uint64_t*)"BRD_RES\0")
#define TILE_STATUS_UPDT (*(uint64_t*)"TIL_STS\0")
#define TILE_REMOVE_UPDT (*(uint64_t*)"TIL_RMV\0")

/// Types
typedef struct coordinate coords;
typedef struct gameBoard board;
typedef struct snakeObj snake;
typedef struct tileObj tile;
typedef struct boardUpdate update;
typedef struct updateHandler updateHandler;
typedef struct snakeUpdate Supdate;
typedef struct tileUpdate Tupdate;

struct coordinate { // Coordinate object
  int x;
  int y;
};

struct gameBoard { // The play area
  // Core properties
  int size_x;
  int size_y;
  Allocator* allc;

  // Board entities
  tile** tiles;
  snake** snakes;

  // Coordinate mapping
  hashMap* snakeMap;
  hashMap* tileMap;

  // Updates
  update* updates;
  hashMap* updateHandlers; // Maps updateType as key

  // Additionnal data management
  void* addData;
  void (*dataFree)(void* Sdata);
};

struct snakeObj { // Snake object with all related data
  coords* coords; // List of coords where the snake is present
  void* addData;
  void (*dataFree)(void* Bdata);
};

struct tileObj {
  coords coordinate;
  void* addData;
  void (*dataFree)(void* Tdata);
};

// Updates Structs
struct boardUpdate {
  uint64_t type; // Associate a key for handler etc… is defined by a num representing a "string" like : "SNK_ENT\0" \0 being a identifier per module
  void* payload;
  void (*free)(void* payload); // Destructor
};

struct updateHandler {
  void (*apply)(void* self, board* targetBoard);
};

typedef struct { // Board resize info
  int size_x;
  int size_y;
} bResizeUpdt;

typedef struct { // Tile type change + tile spawning
  tile* targetTile;
} tileStatusUpdt;

typedef struct { // Tile removal from board
  coords pos;
} tileRemoveUpdt;

/// Functions
// Board properties
board* snBInitBoard(int size_x, int size_y, Allocator* a); // Should create a new board
void snBResizeBoard(board* self, int size_x, int size_y);  // Resizes board limits
void snBDeleteBoard(board* self);                          // Should destroy a board and all related instances

// Board Update
void snBUClear(board* targetBoard);                                                    // Reset all board updates
void snBUAdd(board* targetBoard, update* self);                                        // Adds a new update to the board
void snBURemove(board* targetBoard, update* self);                                     // Remove update from list of updates in board
bool snBURegisterHandler(board* targetBoard, updateHandler handler, char* targetType); // Adds an update type and related infos / handler
bool snBURemoveHandler(board* targetBoard, char* targetType);                          // Removes the associated handler for type
updateHandler* snBUGetHandler(board* targetBoard, char* targetType);                   // Get the associated handler for an updatetype

// Board pos system
coords snBRandomPos(board* targetBoard);                     // Get a random coord on board
coords snBTranslatePos(board* targetBoard, coords position); // Translates a coordinate out of board
void* snBCheckSnake(board* targetBoard, coords position);    // Check for snake entity at pos
void* snBCheckTile(board* targetBoard, coords position);     // Check for snake entity at pos

// Board objects
bool snBAddTile(board* targetBoard, tile self);    // Adds a given tile to board
bool snBDelTile(board* targetBoard, coords pos);   // Removes tile at coords from board
void snBAddSnake(board* targetBoard, snake* self); // Adds a snake to the board
void snBDelSnake(board* targetBoard, snake* self); // Removes the snake at index of the board

// Snake
snake* snSInitSnake(coords pos);                   // Inits a snake at a given position
void snSDeleteSnake(snake* self);                  // Free snake from memory
void snSSetSize(snake* self, int size);            // Sets snake size
void snSMoveHeadPos(snake* self, coords position); // Updates the snake coordinates
int snSGetSize(snake* self);                       // Get the actual size of the snake

#endif