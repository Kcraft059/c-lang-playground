/**
 * Fake obj POC
 */

typedef struct {
  int x;
  int y;
  void (*move)(struct Point*, int, int);
} Point;

void move_point(Point* p, int dx, int dy) {
  p->x += dx;
  p->y += dy;
}

int main() {
  Point p = {0, 0, move_point};
  p.move(&p, 5, 7);  // “method call”
}