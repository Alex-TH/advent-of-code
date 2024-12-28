#include <stdio.h>

/* #define INPUT_FILE "chunk.txt" */
/* #define MAP_WIDTH 10 */
/* #define MAP_HEIGHT 10 */

#define INPUT_FILE "input.txt"
#define MAP_WIDTH 50
#define MAP_HEIGHT 50

#define ROBOT '@'
#define BOX 'O'
#define WALL '#'
#define FREE '.'

#define UP '^'
#define RIGHT '>'
#define DOWN 'v'
#define LEFT '<'

typedef struct Position {
  int x;
  int y;
} Position;

Position build_map(FILE *fd, char map[MAP_HEIGHT][MAP_WIDTH]) {
  Position start;

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      map[y][x] = fgetc(fd);
      if (map[y][x] == ROBOT) {
        start.x = x;
        start.y = y;
      }
    }
    fgetc(fd);
  }

  return start;
}

void print_map(char map[MAP_HEIGHT][MAP_WIDTH]) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      printf("%c", map[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position pos) {
  return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH && pos.x < MAP_HEIGHT;
}

void move_robot(char map[MAP_HEIGHT][MAP_WIDTH], Position *robot, char direction) {
  int delta_x = 0;
  int delta_y = 0;

  switch (direction) {
    case UP: {
      delta_y = -1;
      break;
    }
    case RIGHT: {
      delta_x = 1;
      break;
    }
    case DOWN: {
      delta_y = 1;
      break;
    }
    case LEFT: {
      delta_x = -1;
      break;
    }
    default: return;
  }

  Position entities[MAP_WIDTH];
  Position current;
  int entities_len;

  entities_len = 0;
  current.x = robot->x;
  current.y = robot->y;

  while (is_in_bounds(current) && map[current.y][current.x] != WALL && map[current.y][current.x] != FREE) {
    entities[entities_len].x = current.x;
    entities[entities_len++].y = current.y;

    current.x += delta_x;
    current.y += delta_y;
  }

  // cannot move
  if (!is_in_bounds(current) || map[current.y][current.x] == WALL) return;

  while (entities_len > 0) {
    Position entity = entities[--entities_len];
    map[entity.y + delta_y][entity.x + delta_x] = map[entity.y][entity.x];
    map[entity.y][entity.x] = FREE;
  }

  robot->x += delta_x;
  robot->y += delta_y;
}

int main() {
  FILE *fd;
  char map[MAP_HEIGHT][MAP_WIDTH];
  Position robot;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  robot = build_map(fd, map);
  printf("Robot in {%d, %d}\n", robot.x, robot.y);
  print_map(map);

  char next_move;
  while ((next_move = fgetc(fd)) != EOF) {
    move_robot(map, &robot, next_move);
  }

  print_map(map);

  int solution = 0;
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x] != BOX) continue;

      solution += (100 * y) + x;
    }
  }
  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
