#include <_time.h>
#include <ncurses.h> // Terminal control utilities
#include <stdbool.h> // Meh
#include <stdio.h>   // Std i/o control
#include <stdlib.h>  // Memory management
#include <time.h>    // Time
/// Personnal Libs
#include <array.h> // Dynamic array implementation
#include <snake.h> // Snake logic implementation
#include <string.h>
#include <unistd.h>

/**
 * Project SNAKEUH
 */

board* mainBoard;
snake* mainSnake;

enum exitReason {
  EX_QUIT = 1,
  EX_LOOSE
};

// Helpers
void gameInit() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  srand(time(NULL));
}

void gameStop() {
  endwin();
}

void updateBoard(board* self) {
  box(stdscr, 0, 0);

  // Board clear
  coords targetPos = {0, 0};
  for (int x = 0; x <= self->size_x; ++x) {
    for (int y = 0; y <= self->size_y; ++y) {
      targetPos.x = x;
      targetPos.y = y;
      targetPos = snBGetAbsPos(self, targetPos);
      mvaddch(targetPos.y, targetPos.x, ' ');
    }
  }

  // Snake update
  snake* targetSnake = snBGetSnake(self);
  while ((targetSnake = snBGetSnake(NULL))) {
    for (int co = 0; co < array_length(targetSnake->coords); ++co) {
      coords pos = snBGetAbsPos(self, targetSnake->coords[co]);
      mvaddch(pos.y, pos.x, co ? 'x' : 'o');
    }
  }

  for (int i = 0; i < array_length(self->tiles); ++i) {
    coords pos = snBGetAbsPos(self, self->tiles[i].coordinate);
    mvaddch(pos.y, pos.x, 'f');
  }

  refresh();
}

void moveSnake(board* targetBoard, snake* self, coords nextPos) {
  if (memcmp(&(self->coords[0]), &nextPos, sizeof(coords))) {
    nextPos = snBTranslatePos(targetBoard, nextPos);
    snSMoveHeadPos(self, nextPos);
  }
}

bool checkNextPos(board* targetBoard, snake* targetSnake, enum movType direction, coords* pos) { // Should calc nextPos and return true if not occupied by snake already
  bool result = true;

  // Prevent going backward
  if (mainSnake->direction == MV_UP && direction == MV_DOWN) direction = 0;
  if (mainSnake->direction == MV_DOWN && direction == MV_UP) direction = 0;
  if (mainSnake->direction == MV_LEFT && direction == MV_RIGHT) direction = 0;
  if (mainSnake->direction == MV_RIGHT && direction == MV_LEFT) direction = 0;
  if (direction) mainSnake->direction = direction;

  // Position control
  coords nextPos = snSComputeHeadPos(targetSnake, direction);           // Predicts next pos
  snake nextSnake = {.coords = array_dup(coords, targetSnake->coords)}; // Copy snake object

  snSMoveHeadPos(&nextSnake, // Calc new pos
                 snBTranslatePos(targetBoard, nextPos));

  array_remove(&(nextSnake.coords), 0); // Do not account for sanke head

  if (snSCheckSnake(&nextSnake, nextPos) && targetSnake->direction != MV_STILL) result = false;

  array_delete(nextSnake.coords);

  *pos = nextPos;
  return result;
}

bool setRandomTile(board* targetBoard, tile* self) {
  for (int i = 0; i < 100; ++i) {
    self->coordinate = snBRngPos(targetBoard);

    // Availability check
    bool pass = false;
    if (snBCheckTile(targetBoard, self->coordinate, NULL)) continue;
    for (int j = 0; j < array_length(targetBoard->snakes); ++j) {
      if (snSCheckSnake(targetBoard->snakes[j], self->coordinate)) {
        pass = true;
        break;
      }
    }

    if (!pass) return true;
  }
  return false;
}

enum exitReason tileInteract(board* targetBoard, snake* self, tile* tileObj) { // Do action depending on tile
  switch (tileObj->type) {
  case T_FOOD:
    snSSetSize(self, snSGetLength(self) + 1); // Add 1 to snake length

    // Add a new food tile to the board
    snBDelTile(targetBoard, tileObj->coordinate);
    tile newTile = {.type = T_FOOD};
    if (setRandomTile(targetBoard, &newTile))
      snBAddTile(targetBoard, newTile);
    break;

  case T_WALL:
    return EX_LOOSE;
    break;

  default:
    break;
  }
  return 0;
}

// Main loop
int main(int argc, char** argv) {
  gameInit();

  /// Init board + snake
  mainBoard = snBInitBoard(1, 1);
  snBResizeBoard(mainBoard, getmaxx(stdscr) - 3, getmaxy(stdscr) - 3);

  coords initPos = {mainBoard->size_x / 2, mainBoard->size_y / 2};
  mainSnake = snSInitSnake(initPos); // Inits a snake at the middle of the board

  snBAddSnake(mainBoard, mainSnake); // Add sanke to board

  nodelay(stdscr, TRUE);  // Setup non blocking key listener
  updateBoard(mainBoard); // Update Board

  // Def
  int loopIteration = 0;
  enum exitReason exitloop = 0;
  struct timespec updateFreq = {0, 50000000};
  int key;
  enum movType direction = 0; // Needs to be initialized

  // Game loop
  while (!exitloop) {
    nanosleep(&updateFreq, NULL); // Respect game updateFreq

    key = getch(); // fetch key press from user

    tile foodTile = {.coordinate = {0, 0}, .type = T_FOOD};

    switch (key) {
    case 'q':
      exitloop = EX_QUIT;
      break;
    case '+':
      snSSetSize(mainSnake, snSGetLength(mainSnake) + 1);
      break;
    case '-':
      snSSetSize(mainSnake, snSGetLength(mainSnake) - 1);
      break;
    case KEY_LEFT:
      direction = MV_LEFT;
      break;
    case KEY_RIGHT:
      direction = MV_RIGHT;
      break;
    case KEY_UP:
      direction = MV_UP;
      break;
    case KEY_DOWN:
      direction = MV_DOWN;
      break;
    case 's':
      direction = MV_STILL;
      break;
    case 't':
      if (setRandomTile(mainBoard, &foodTile))
        snBAddTile(mainBoard, foodTile);
      break;
    case 410:
      snBResizeBoard(mainBoard, getmaxx(stdscr) - 3, getmaxy(stdscr) - 3);
    }
    if (exitloop) break;

    // Key check triggers

    // Move snake
    if (!(loopIteration % 2)) { // Every 2 iterations
      coords nextPos;
      bool moveOk;

      // Calc next pos for snake
      moveOk = checkNextPos(mainBoard, mainSnake, direction, &nextPos);
      if (!moveOk) exitloop = EX_LOOSE;

      // Snake Update
      moveSnake(mainBoard, mainSnake, nextPos);
      direction = 0;

      // Check for tile presence
      tile nextTile;
      int result = 0;
      if (snBCheckTile(mainBoard, nextPos, &nextTile))
        result = tileInteract(mainBoard, mainSnake, &nextTile);
      if (result) exitloop = result;
    }

    // Check if loop should exit
    if (exitloop) break;

    // Update loop index
    loopIteration += loopIteration >= 20 ? -loopIteration : 1;

    // Update board render
    updateBoard(mainBoard);
  }

  switch (exitloop) {
  case EX_LOOSE:
    mvprintw(0, (getmaxx(stdscr) - strlen("You lost")) / 2, "You lost");
    refresh();
    nodelay(stdscr, false);
    getch();
    break;
  default:
    break;
  }

  snBDeleteBoard(mainBoard);
  gameStop();

  return 0;
}
