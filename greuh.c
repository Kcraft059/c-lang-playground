
/**
 *
 * THIS WHOLE SECTION IS HORRIBLE
 *
 */

/* int UIinit() {
  if (!initscr()) return 0;           // Start curses mode
  if (raw()) return 0;                // Listen for key input while not sending any SIGNAL (no ctrl-c)
  if (noecho()) return 0;             // Do not show keypresses
  if (keypad(stdscr, true)) return 0; // Do not show keypresses
  curs_set(0);
  return 1;
}

int UIend() {
  endwin();
  return 0;
}

int shittyWindowResize(WINDOW* win) {
  int tx, ty;
  getmaxyx(stdscr, ty, tx);
  wresize(win, ty, tx);
  getmaxyx(win, ty, tx);
  ty -= 2;
  tx -= 2;
  int x, y;
  y = 0;
  for (x = 0; x < tx; x++) {
    for (y = 0; y < ty; y++) {
      mvwaddch(win, y + 1, x + 1, ' ');
    }
  }
  box(win, 0, 0);
  wrefresh(win);
}

int main(int argc, char** argv) {  // Implementation example
  if (!UIinit()) exit(1);

  // Window
  int heigth = LINES;
  int width = COLS;
  WINDOW* scoreWin = newwin(heigth, width, 0, 0);
  box(scoreWin, 0, 0);

  while (1) {
    int key_pressed = getch();

    switch (key_pressed) {
      case 410:
        shittyWindowResize(scoreWin);
        break;
      case (int)'q':
        endwin();
        exit(0);
        break;

      default:
        mvwprintw(scoreWin, 1, 1, "Key code \"%d\" \"%c\" ", key_pressed, (char)key_pressed);
        break;
    }
    wrefresh(scoreWin);
  };

  return 0;
}
 */