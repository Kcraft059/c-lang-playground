#include <stdbool.h>
/// Personnal Libs
#include <array.h>

/**
 * Snake logic definition module
 */

/// Macros

#define snInitBoard(x, y) /* Init Board to stdBoard */ \
  do {                                                 \
    snStdBoard = snBInitBoard();                       \
    snBResizeBoard(snStdBoard, (x), (y));              \
  } while (0)
#define snResizeBoard(x, y) (snBResizeBoard(snStdBoard, (x), (y)))  // Resizes stdBoard

/// Types
typedef struct coordinate coords;
typedef struct board board;
typedef struct snake snake;
typedef struct tile tile;

enum movType {  // Movements avail
  MV_UP,
  MV_DOWN,
  MV_RIGHT,
  MV_LEFT,
};

enum tileType {  // Not sure of implementation
  T_FOOD,
  T_WALL
  // T_BLANK, is not a tile type
  // T_SNAKE
};

struct coordinate {  // Coordinate object
  int x;
  int y;
};

struct tile {
  coords coordinate;
  enum tileType type;
};

struct board {  // A play area
  int size_x;
  int size_y;
  bool board_warp;  // If the snake should warp around the whole board when going past the egde
  tile* tiles;      // All board tiles
  snake** snakes;   // Snakes present on the board
};

struct snake {  // Snake object with all related data
  int size;
  int score;
  // int speed;
  enum movType direction;
  coords* coords;
};

/// Global Vars

extern board* snStdBoard;

/// Functions

// Board properties
board* snBInitBoard();                           // Should create a new board
void snBResizeBoard(board* self, int x, int y);  // Resizes board limits
void snBDeleteBoard(board* self);                // Should destroy a board and all related instances

// Board pos system
bool snBCheckPos(board* targetBoard, coords position);  // Should return a different value depending on item present
                                                        // Will be a wrapper for multiple checkers :
                                                        // snSCheckSnake, snBCheckTile
// coords __snBTranslatePos(board* targetBoard, coords position);  // In case of warp, should calc new POS for a pos given outside of board limits
coords snBRngBlankPos(board* targetBoard);
/* TODO, snake board switching handling logic */

// Board tile
void snBAddTile(board* targetBoard, tile self);           // Adds a given tile to board
void snBDelTile(board* targetBoard, coords pos);          // Removes tile at coords from board
bool snBCheckTile(board*, coords pos, tile* returnTile);  // Check for tile at pos, if none return false

// Board snake
void snBAddSnake(board* targetBoard, snake* self);   // Adds a snake to the board
void snBDelSnake(board* targetBoard, size_t index);  // Removes the snake at index of the board

// Board status
// META, get all differential changes made to the board

board* snBMakeSnapshot(board* object);                 // Duplicates board in memory ( and childrens like tiles and snakes )
                                                       // This should be done before starting to make changes
board* snBDiffSnapshot(board** image, board* object);  // This should modify image to reflect only what changed maybe with an XOR MASK

/* This could be done by creating a snapshot of the board and its elements */

// Snake
void snSMoveHeadPos(snake* self, coords position);                                  // Updates the snake coordinates
bool snSCheckSnake(snake* self, coords position);                                   // Check for snake presence
void snSSetSize(snake* self, int size);                                             // Sets snake size
coords* snBSComputeNxPos(board* targetBoard, snake* self, enum movType direction);  // Computes the value of the next snake positions of the snake
