#include <stdio.h>

#define INPUT_FILE_PATH "input.txt"
/* #define GARDEN_WIDTH 10 */
/* #define GARDEN_HEIGHT 10 */
#define GARDEN_WIDTH 140
#define GARDEN_HEIGHT 140

typedef struct Plot {
  char value;
  int area;
  int perimeter;
} Plot;

typedef struct Position {
  short x;
  short y;
} Position;

void build_garden(FILE *fd, short garden[GARDEN_HEIGHT][GARDEN_WIDTH]) {
  for (int y = 0; y < GARDEN_HEIGHT; y++) {
    for (int x = 0; x < GARDEN_WIDTH; x++) {
      garden[y][x] = fgetc(fd);
    }
    fgetc(fd);
  }
}

void print_garden(short garden[GARDEN_HEIGHT][GARDEN_WIDTH]) {
  for (int y = 0; y < GARDEN_HEIGHT; y++) {
    for (int x = 0; x < GARDEN_WIDTH; x++) {
      printf("%c", garden[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position position) {
  return position.x >= 0 && position.y >= 0 && position.x < GARDEN_WIDTH && position.y < GARDEN_HEIGHT;
}

Plot build_plot(short garden[GARDEN_HEIGHT][GARDEN_WIDTH], Position start, short visited[GARDEN_HEIGHT][GARDEN_WIDTH]) {
  Plot result;
  Position stack[GARDEN_WIDTH * GARDEN_HEIGHT];
  int stack_len;

  stack[0] = start;
  stack_len = 1;
  result.value = garden[start.y][start.x];
  result.area = 0;
  result.perimeter = 0;

  printf("Plot starting in {%d, %d}\n", start.x, start.y);
  while (stack_len > 0) {
    Position current = stack[--stack_len];
    if (visited[current.y][current.x]) continue;
    visited[current.y][current.x] = 1;

    result.area++;
    Position up = { .x = current.x, .y = current.y - 1 };
    Position down = { .x = current.x, .y = current.y + 1 };
    Position left = { .x = current.x - 1, .y = current.y };
    Position right = { .x = current.x + 1, .y = current.y };

    if (is_in_bounds(up) && garden[up.y][up.x] == result.value) stack[stack_len++] = up;
    else result.perimeter++;

    if (is_in_bounds(down) && garden[down.y][down.x] == result.value) stack[stack_len++] = down;
    else result.perimeter++;

    if (is_in_bounds(left) && garden[left.y][left.x] == result.value) stack[stack_len++] = left;
    else result.perimeter++;

    if (is_in_bounds(right) && garden[right.y][right.x] == result.value) stack[stack_len++] = right;
    else result.perimeter++;
  }

  return result;
}

int main() {
  FILE *fd;
  short garden[GARDEN_HEIGHT][GARDEN_WIDTH];
  short visited[GARDEN_HEIGHT][GARDEN_WIDTH] = {{0}};
  Plot plots[100];
  short plot_len = 0;

  fd = fopen(INPUT_FILE_PATH, "r");
  if (fd == NULL) {
    perror("Missing input file\n");
    return -1;
  }

  build_garden(fd, garden);
  print_garden(garden);

  for (int y = 0; y < GARDEN_HEIGHT; y++) {
    for (int x = 0; x < GARDEN_WIDTH; x++) {
      if (visited[y][x]) continue;

      Position start = { .x = x, .y = y };
      plots[plot_len++] = build_plot(garden, start, visited);
    }
  }

  int solution = 0;
  for (int i = 0; i < plot_len; i++) {
    int value = plots[i].area * plots[i].perimeter;
    printf("value -> %c: %d * %d = %d +\n", plots[i].value, plots[i].area, plots[i].perimeter, value);
    solution += value;
  }
  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
