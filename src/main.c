#include <ncurses.h>  // Terminal control utilities
#include <stdbool.h>  // Meh
#include <stdio.h>    // Std i/o control
#include <stdlib.h>   // Memory management
/// Personnal Libs
#include <array.h>  // Dynamic array implementation
#include <snake.h>  // Snake logic implementation

/**
 * Project SNAKEUH
 */

int main(int argc, char** argv) {
  // Board init + Set of properties
  board* main = snBInitBoard();
  snBResizeBoard(main, 16, 16);

  coords pos = {8, 8};
  snake* mysnake = snSInitSnake(pos);

  snBAddSnake(main, mysnake);

  snSSetSize(main->snakes[0], 3);
  pos.x = 7;
  array_append(&(main->snakes[0]->coords), &pos);
  snSSetSize(main->snakes[0], snSGetLength(main->snakes[0]) + 1);

  pos.x = 9;
  pos.y = 8;
  snSMoveHeadPos(mysnake, pos);
  snSSetSize(main->snakes[0], snSGetLength(main->snakes[0]) + 1);

  // Print array
  for (int i = 0; i < array_length(main->snakes[0]->coords); ++i) {
    coords crt = main->snakes[0]->coords[i];
    if (i == 0) printf("[");
    printf("(%d;%d)", crt.x, crt.y);
    if (i == array_length(main->snakes[0]->coords) - 1) {
      printf("]\n");
    } else {
      printf(",");
    }
  }
  
  if (snSCheckSnake(mysnake, pos)) printf("Here\n");

  snBDeleteBoard(main);  // Delete once not used anymore
}
