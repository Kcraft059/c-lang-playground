#include <errno.h>
#include <ncurses.h>
#include <stdlib.h>

struct coordinate {
  int x;
  int y;
};

struct line {
  float xcoef;
  float ycoef;
  float xoffset;
  float yoffset;
};

struct line calcLine(struct coordinate ptA, struct coordinate ptB) {
  struct line self;

  self.xcoef = (float)(ptA.y - ptB.y) / (float)(ptA.x - ptB.x);
  self.xoffset = (float)(ptA.y - ptA.x * self.xcoef);
  self.ycoef = (float)(ptA.x - ptB.x) / (float)(ptA.y - ptB.y);
  self.yoffset = (float)(ptA.x - ptA.y * self.ycoef);

  return self;
}

float getYLine(struct line self, int xpos) {
  float m = self.xcoef;
  float p = self.xoffset;

  float result = xpos * m + p;
  return result;
}

float getXLine(struct line self, int ypos) {
  float m = self.ycoef;
  float p = self.yoffset;

  float result = ypos * m + p;
  return result;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    perror("Invalid args count");
    exit(EXIT_FAILURE);
  };

  struct coordinate cooA;
  struct coordinate cooB;
  int succesA = (sscanf(argv[1], "%d,%d", &(cooA.x), &(cooA.y)) == 2);
  int succesB = (sscanf(argv[2], "%d,%d", &(cooB.x), &(cooB.y)) == 2);

  if (!(succesA && succesB)) {
    perror("Invalid args");
    exit(EXIT_FAILURE);
  };

  struct line myline = calcLine(cooA, cooB);
  // printf("Y : %d\n", (int)getYLine(myline, 4));

  // ncurses
  initscr();
  curs_set(0);
  cbreak();

  mvwaddch(stdscr, cooA.y, cooA.x, 'x');
  mvwaddch(stdscr, cooB.y, cooB.x, 'x');

  int start;
  int destination;

  if (abs(cooA.x - cooB.x) > abs(cooA.y - cooB.y)) {
    if (cooA.x < cooB.x) {
      start = cooA.x;
      destination = cooB.x;
    } else {
      start = cooB.x;
      destination = cooA.x;
    }

    for (int i = start + 1; i < destination; ++i) {
      mvwaddch(stdscr, (int)getYLine(myline, i), i, 'o');
    }
  } else {
    if (cooA.y < cooB.y) {
      start = cooA.y;
      destination = cooB.y;
    } else {
      start = cooB.y;
      destination = cooA.y;
    }

    for (int i = start + 1; i < destination; ++i) {
      mvwaddch(stdscr, i, (int)getXLine(myline, i), 'o');
    }
  }

  refresh();

  getch();
  endwin();
  return 0;
}