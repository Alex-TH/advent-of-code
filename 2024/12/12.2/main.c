#include <stdio.h>

#define INPUT_FILE_PATH "input.txt"
/* #define GARDEN_WIDTH 10 */
/* #define GARDEN_HEIGHT 10 */
#define GARDEN_WIDTH 140
#define GARDEN_HEIGHT 140

typedef struct Plot {
  char value;
  int area;
  int sides;
} Plot;

// Masks
typedef enum { UP = 1, RIGHT = 2, DOWN = 4, LEFT = 8 } Direction;
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

void print_border(short border[GARDEN_HEIGHT][GARDEN_WIDTH]) {
  for (int y = 0; y < GARDEN_HEIGHT; y++) {
    for (int x = 0; x < GARDEN_WIDTH; x++) {
      printf("0x%X ", border[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position position) {
  return position.x >= 0 && position.y >= 0 && position.x < GARDEN_WIDTH && position.y < GARDEN_HEIGHT;
}

Plot build_plot(short garden[GARDEN_HEIGHT][GARDEN_WIDTH], Position start, short visited[GARDEN_HEIGHT][GARDEN_WIDTH], short border_visited[GARDEN_HEIGHT][GARDEN_WIDTH]) {
  Plot result;
  Position stack[GARDEN_WIDTH * GARDEN_HEIGHT * 2];
  Direction masks[4] = { UP, DOWN, LEFT, RIGHT };
  Position exhaust_diffs[4] = {
    { .x = 1, .y = 0 },
    { .x = 1, .y = 0 },
    { .x = 0, .y = 1 },
    { .x = 0, .y = 1 }
  };
  int stack_len;

  stack[0] = start;
  stack_len = 1;
  result.value = garden[start.y][start.x];
  result.area = 0;
  result.sides = 0;

  printf("Plot starting in {%d, %d}\n", start.x, start.y);
  while (stack_len > 0) {
    Position current = stack[--stack_len];
    if (visited[current.y][current.x]) continue;
    visited[current.y][current.x] = 1;

    result.area++;
    Position positions[4] = {
      { .x = current.x, .y = current.y - 1 },
      { .x = current.x, .y = current.y + 1 },
      { .x = current.x - 1, .y = current.y },
      { .x = current.x + 1, .y = current.y }
    };

    for (int i = 0; i < 4; i++) {
      Position pos = positions[i];

      if (is_in_bounds(pos) && garden[pos.y][pos.x] == result.value) stack[stack_len++] = pos;
      else if ((border_visited[current.y][current.x] & masks[i]) != masks[i]) {
        result.sides++;
        Position exhaust_diff = exhaust_diffs[i];
        Position exhaust_pos;
        Position border_pos;

        // exhaust to one side
        exhaust_pos.x = current.x;
        exhaust_pos.y = current.y;
        border_pos.x = pos.x;
        border_pos.y = pos.y;

        printf("Exhausting for %d in {%d, %d}\n", i, current.x, current.y);
        while ((is_in_bounds(exhaust_pos) && garden[exhaust_pos.y][exhaust_pos.x] == result.value) && (!is_in_bounds(border_pos) || garden[border_pos.y][border_pos.x] != result.value)) {
          border_visited[exhaust_pos.y][exhaust_pos.x] |= masks[i];

          exhaust_pos.x += exhaust_diff.x;
          exhaust_pos.y += exhaust_diff.y;
          border_pos.x += exhaust_diff.x;
          border_pos.y += exhaust_diff.y;
        }

        // exhaust to the ohter side
        exhaust_pos.x = current.x;
        exhaust_pos.y = current.y;
        border_pos.x = pos.x;
        border_pos.y = pos.y;

        while ((is_in_bounds(exhaust_pos) && garden[exhaust_pos.y][exhaust_pos.x] == result.value) && (!is_in_bounds(border_pos) || garden[border_pos.y][border_pos.x] != result.value)) {
          border_visited[exhaust_pos.y][exhaust_pos.x] |= masks[i];

          exhaust_pos.x -= exhaust_diff.x;
          exhaust_pos.y -= exhaust_diff.y;
          border_pos.x -= exhaust_diff.x;
          border_pos.y -= exhaust_diff.y;
        }
        /* print_border(border_visited); */
        printf("current sides %d\n", result.sides);
      }
    }
  }

  return result;
}

int main() {
  FILE *fd;
  short garden[GARDEN_HEIGHT][GARDEN_WIDTH];
  short visited[GARDEN_HEIGHT][GARDEN_WIDTH] = {{0}};
  short border_visited[GARDEN_HEIGHT][GARDEN_WIDTH] = {{0}};
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
      plots[plot_len++] = build_plot(garden, start, visited, border_visited);
    }
  }

  int solution = 0;
  for (int i = 0; i < plot_len; i++) {
    int value = plots[i].area * plots[i].sides;
    printf("value -> %c: %d * %d = %d +\n", plots[i].value, plots[i].area, plots[i].sides, value);
    solution += value;
  }
  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
