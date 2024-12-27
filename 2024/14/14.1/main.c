#include <stdio.h>

#define INPUT_FILE "input.txt"
#define GRID_WIDTH 101
#define GRID_HEIGHT 103
/* #define GRID_WIDTH 11 */
/* #define GRID_HEIGHT 7 */

#define MAX_ROBOTS 500
#define ITERATIONS 100

typedef struct Robot {
  int x;
  int y;
  int speed_x;
  int speed_y;
} Robot;

int parse_input(FILE *fd, Robot robots[MAX_ROBOTS]) {
  int len = 0;

  while (fscanf(fd, "p=%d,%d v=%d,%d\n", &robots[len].x, &robots[len].y, &robots[len].speed_x, &robots[len].speed_y) != EOF) {
    len++;
  }

  return len;
}

void print_layout(Robot robots[MAX_ROBOTS], int robots_len) {
  short layout[GRID_HEIGHT][GRID_WIDTH] = {{ 0 }};

  for (int i = 0; i < robots_len; i++) {
    printf("p=%d,%d v=%d,%d\n", robots[i].x, robots[i].y, robots[i].speed_x, robots[i].speed_y);
    layout[robots[i].y][robots[i].x]++;
  }
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      short current_value = layout[y][x];
      printf("%c", current_value == 0 ? '.' : current_value + '0');
    }
    printf("\n");
  }
}

void move_robots(Robot robots[MAX_ROBOTS], int robots_len, int times) {
  for (int i = 0; i < robots_len; i++) {
    robots[i].x = (robots[i].x + times * robots[i].speed_x) % GRID_WIDTH;
    robots[i].y = (robots[i].y + times * robots[i].speed_y) % GRID_HEIGHT;

    robots[i].x += robots[i].x < 0 ? GRID_WIDTH : 0;
    robots[i].y += robots[i].y < 0 ? GRID_HEIGHT : 0;
  }
}

int main() {
  FILE *fd;
  Robot robots[MAX_ROBOTS];
  int robots_len;
  short count_quadrants[4] = {0};

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  robots_len = parse_input(fd, robots);
  print_layout(robots, robots_len);
  move_robots(robots, robots_len, ITERATIONS);
  print_layout(robots, robots_len);

  for (int i = 0; i < robots_len; i++) {
    if (robots[i].x == GRID_WIDTH / 2 || robots[i].y == GRID_HEIGHT / 2) continue;
    short current_quadrant = robots[i].x > GRID_WIDTH / 2 ? 1 : 0;
    current_quadrant += robots[i].y > GRID_HEIGHT / 2 ? 2 : 0;

    count_quadrants[current_quadrant]++;
  }

  int solution = count_quadrants[0] * count_quadrants[1] * count_quadrants[2] * count_quadrants[3];
  printf("q1=%d, q2=%d, q3=%d, q4=%d\n", count_quadrants[0], count_quadrants[1], count_quadrants[2], count_quadrants[3]);
  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
