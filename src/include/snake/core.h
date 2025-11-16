#include <stdbool.h>
/// Personnal Libs
#include <array.h>

/**
 * Snake core logic definition module
 */

/// Types
typedef struct coordinate coords;
typedef struct gameBoard board;
typedef struct snakeObj snake;
typedef struct tileObj tile;
typedef struct boardUpdate update;
typedef struct snakeUpdate Supdate;
typedef struct tileUpdate Tupdate;

struct coordinate { // Coordinate object
  int x;
  int y;
};

enum updateType {
  SNAKE_UPDT,
  TILE_UPDT
};

struct boardUpdate {
  enum updateType type;
  union { // Prevents void* casting
    Supdate* snake;
  } target;
};

struct gameBoard { // The play area
  // Core properties
  int size_x;
  int size_y;
  // Board entities
  tile* tiles;
  snake** snakes;
  // Coordinate mapping
  hashMap* snakeMap;
  hashMap* tileMap;
  // Updates

  // Additionnal data management
  void* Bdata;
  void (*SdataFree)(void* Sdata);
  void (*BdataFree)(void* Bdata);
  void (*TdataFree)(void* Tdata);
};

struct snakeObj { // Snake object with all related data
  coords* coords; // List of coords where the snake is present
  void* Sdata;
};

struct tileObj {
  coords coordinate;
  void* Tdata;
};

struct snakeUpdate { // Update info package
  enum {             // Update type
    LEAVE_POS,
    ENTER_POS,
    CHANGE_POS
  } action;
  union { // Update params
    coords pos;
  };
  snake* target; // Update target
};

struct tileUpdate {
  enum {
    ADD_POS,
    REMOVE_POS
  } action;
  union { // Update params
    coords pos;
  };
  snake* target; // Update target
};


/// Global Vars

extern board* snStdBoard; // This is the default snake board

/// Functions

// Board properties
board* snBInitBoard();                          // Should create a new board
void snBResizeBoard(board* self, int x, int y); // Resizes board limits
void snBDeleteBoard(board* self);               // Should destroy a board and all related instances

// Board objects
void snBAddTile(board* targetBoard, tile self);    // Adds a given tile to board
void snBDelTile(board* targetBoard, coords pos);   // Removes tile at coords from board
void snBAddSnake(board* targetBoard, snake* self); // Adds a snake to the board
void snBDelSnake(board* targetBoard, snake* self); // Removes the snake at index of the board

// Board pos system
coords snBRngPos(board* targetBoard);                        // Get a random coord on board
coords snBTranslatePos(board* targetBoard, coords position); // Translates a coordinate out of board
void* snBCheckSnake(board* targetBoard, coords position);    // Check for snake entity at pos
void* snBCheckTile(board* targetBoard, coords position);     // Check for snake entity at pos

// Snake
snake* snSInitSnake(coords pos);                   // Inits a snake at a given position
void snSDeleteSnake(snake* self);                  // Free snake from memory
void snSSetSize(snake* self, int size);            // Sets snake size
void snSMoveHeadPos(snake* self, coords position); // Updates the snake coordinates
int snSGetLength(snake* self);                     // Get the actual size of the snake
