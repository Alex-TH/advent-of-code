#include <stdio.h>
#include <stdint.h>

/* #define INPUT_FILE "chunk.txt" */
/* #define MAP_WIDTH 10 */
/* #define MAP_HEIGHT 10 */
/* #define MAX_BOXES 100 */

#define INPUT_FILE "input.txt"
#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define MAX_BOXES 1500

#define ROBOT '@'
#define BOX 'O'
#define WALL '#'
#define FREE '.'
#define WIDE_BOX_LEFT '['
#define WIDE_BOX_RIGHT ']'

#define UP '^'
#define RIGHT '>'
#define DOWN 'v'
#define LEFT '<'

typedef struct Position {
  int x;
  int y;
} Position;


typedef struct Box {
  Position start;
  Position end;
  uint16_t id;
} Box;
typedef struct MapResult {
  Position robot;
  int boxes_count;
} MapResult;

MapResult build_map(FILE *fd, uint32_t map[MAP_HEIGHT][MAP_WIDTH * 2], Box boxes[MAX_BOXES]) {
  MapResult result;
  result.boxes_count = 0;

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH * 2; x += 2) {
      char current = fgetc(fd);
      switch (current) {
        case ROBOT: {
          result.robot.x = x;
          result.robot.y = y;
          map[y][x] = ROBOT;
          map[y][x + 1] = FREE;

          break;
        }
        case FREE: {
          map[y][x] = FREE;
          map[y][x + 1] = FREE;
          break;
        }
        case WALL: {
          map[y][x] = WALL;
          map[y][x + 1] = WALL;
          break;
        }
        case BOX: {
          uint32_t id_component = ((result.boxes_count + 1) << 8) & 0xFFFFFF00;

          map[y][x] = WIDE_BOX_LEFT;
          map[y][x + 1] = WIDE_BOX_RIGHT;
          map[y][x] |= id_component;
          map[y][x + 1] |= id_component;

          boxes[result.boxes_count].start.x = x;
          boxes[result.boxes_count].start.y = y;
          boxes[result.boxes_count].end.x = x + 1;
          boxes[result.boxes_count].end.y = y;
          boxes[result.boxes_count].id = result.boxes_count + 1;
          result.boxes_count++;

          break;
        }
      }
    }
    fgetc(fd);
  }

  return result;
}

MapResult build_map_big(FILE *fd, uint32_t map[MAP_HEIGHT][MAP_WIDTH * 2], Box boxes[MAX_BOXES]) {
  MapResult result;
  result.boxes_count = 0;

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH * 2; x += 1) {
      char current = fgetc(fd);
      switch (current) {
        case ROBOT: {
          result.robot.x = x;
          result.robot.y = y;
          map[y][x] = ROBOT;

          break;
        }
        case FREE: {
          map[y][x] = FREE;
          break;
        }
        case WALL: {
          map[y][x] = WALL;
          map[y][x + 1] = WALL;
          break;
        }
        case WIDE_BOX_LEFT: {
          uint32_t id_component = ((result.boxes_count + 1) << 8) & 0xFFFFFF00;

          map[y][x] = WIDE_BOX_LEFT;
          map[y][x + 1] = WIDE_BOX_RIGHT;
          map[y][x] |= id_component;
          map[y][x + 1] |= id_component;

          boxes[result.boxes_count].start.x = x;
          boxes[result.boxes_count].start.y = y;
          boxes[result.boxes_count].end.x = x + 1;
          boxes[result.boxes_count].end.y = y;
          boxes[result.boxes_count].id = result.boxes_count + 1;
          result.boxes_count++;

          break;
        }
      }
    }
    fgetc(fd);
  }

  return result;
}

void print_map(uint32_t map[MAP_HEIGHT][MAP_WIDTH * 2]) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH * 2; x++) {
      printf("%c", map[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position pos) {
  return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH && pos.x < MAP_HEIGHT;
}
int is_box_in_bounds(Box box) {
  return is_in_bounds(box.start) && is_in_bounds(box.end);
}

void move_robot(uint32_t map[MAP_HEIGHT][MAP_WIDTH * 2], MapResult *map_result, Box boxes[MAX_BOXES], char direction) {
  int delta_x = 0;
  int delta_y = 0;

  printf("moving -> %c\n", direction);
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

  int box_ids[MAX_BOXES];
  Box box_queue[MAX_BOXES];
  short visited_boxes[MAX_BOXES] = {0};

  int ids_len = 0;
  int queue_len = 1;
  int queue_start = 0;

  box_queue[0].start.x = map_result->robot.x;
  box_queue[0].start.y = map_result->robot.y;
  box_queue[0].end.x = map_result->robot.x;
  box_queue[0].end.y = map_result->robot.y;
  box_queue[0].id = 0;

  Box current;
  printf("Starting at {%d, %d} with delta {%d, %d}\n", map_result->robot.x, map_result->robot.y, delta_x, delta_y);
  while (queue_len > 0) {
    current = box_queue[queue_start++];
    queue_len--;
    if (current.id > 0 && visited_boxes[current.id - 1] > 0) {
      continue;
    }

    if (map[current.start.y][current.start.x] != ROBOT) {
      printf("adding -> {%d, %d}, %d\n", current.start.x, current.start.y, current.id);
      box_ids[ids_len++] = current.id;
      visited_boxes[current.id - 1] = 1;
    }
    char start_next = map[current.start.y + delta_y][current.start.x + delta_x];
    uint16_t start_next_id = map[current.start.y + delta_y][current.start.x + delta_x] >> 8;
    printf("start next %c, %d -> 0x%.4X\n", start_next, start_next_id, map[current.start.y + delta_y][current.start.x + delta_x]);

    if (start_next == WIDE_BOX_LEFT || start_next == WIDE_BOX_RIGHT) {
      box_queue[(queue_len + queue_start)] = boxes[start_next_id - 1];
      queue_len++;
    }
    // Note(Alex): Stop the whole process if there is a WALL
    char end_next = map[current.end.y + delta_y][current.end.x + delta_x];
    uint16_t end_next_id = map[current.end.y + delta_y][current.end.x + delta_x] >> 8;
    printf("end next %c, %d -> 0x%.4X\n", end_next, end_next_id, map[current.end.y + delta_y][current.end.x + delta_x]);

    if ((end_next == WIDE_BOX_LEFT || end_next == WIDE_BOX_RIGHT) && start_next_id != end_next_id && start_next != current.id) {
      box_queue[(queue_len + queue_start)] = boxes[end_next_id - 1];
      queue_len++;
    }

    if (start_next == WALL || end_next == WALL) {
      printf("Not moving start {%d, %d} end {%d, %d}\n", current.start.x, current.start.y, current.end.x, current.end.y);
      return;
    }
  }

  printf("Done in {%d, %d} = %c\n", current.start.x, current.start.y, map[current.start.y][current.start.x]);

  while (ids_len > 0) {
    int id = box_ids[--ids_len];
    if (id > map_result->boxes_count || id <= 0) {
      printf("Something went wrong! %d\n", id);
    }

    Box box = boxes[id - 1];

    printf("Movig box: %d start => {%d, %d}, end => {%d, %d}\n", id, box.start.x, box.start.y, box.end.x, box.end.y);
    if (direction != RIGHT) {
      map[box.start.y + delta_y][box.start.x + delta_x] = (id << 8) | WIDE_BOX_LEFT;
      map[box.start.y][box.start.x] = FREE;
      map[box.end.y + delta_y][box.end.x + delta_x] = (id << 8) | WIDE_BOX_RIGHT;
      map[box.end.y][box.end.x] = FREE;
    }

    if (direction == RIGHT) {
      map[box.end.y + delta_y][box.end.x + delta_x] = (id << 8) | WIDE_BOX_RIGHT;
      map[box.end.y][box.end.x] = FREE;
      map[box.start.y + delta_y][box.start.x + delta_x] = (id << 8) | WIDE_BOX_LEFT;
      map[box.start.y][box.start.x] = FREE;
    }

    box.start.x += delta_x;
    box.start.y += delta_y;
    box.end.x += delta_x;
    box.end.y += delta_y;
    boxes[id - 1] = box;
  }

  map[map_result->robot.y][map_result->robot.x] = FREE;
  map_result->robot.x += delta_x;
  map_result->robot.y += delta_y;
  map[map_result->robot.y][map_result->robot.x] = ROBOT;
}

int main() {
  FILE *fd;
  uint32_t map[MAP_HEIGHT][MAP_WIDTH * 2];
  Box boxes[MAX_BOXES];
  Position robot;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  MapResult result = build_map(fd, map, boxes);
  /* MapResult result = build_map_big(fd, map, boxes); */
  robot = result.robot;
  printf("There are %d boxes\n", result.boxes_count);
  printf("Robot in {%d, %d}\n", robot.x, robot.y);
  print_map(map);

  char next_move;
  int moves = 0;
  while ((next_move = fgetc(fd)) != EOF) {
    printf("Movement %d\n", moves++);
    move_robot(map, &result, boxes, next_move);
    /* print_map(map); */
    /* scanf("%c", &next_move); */
  }

  print_map(map);

  uint32_t solution = 0;
  for (int i = 0; i < result.boxes_count; i++) {
    solution += 100 * boxes[i].start.y + boxes[i].start.x;
  }
  printf("solution: %u\n", solution);

  fclose(fd);
  return 0;
}
