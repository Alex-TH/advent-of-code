#include <stdio.h>
#include <string.h>

/* #define INPUT_FILE "chunk.txt" */
/* #define MAP_WIDTH 17 */
/* #define MAP_HEIGHT 17 */
#define INPUT_FILE "input.txt"
#define MAP_WIDTH 141
#define MAP_HEIGHT 141

#define WALL '#'
#define START 'S'
#define END 'E'

#define MAX_PRIORITY_VALUES 100

typedef enum { NORTH, EAST, SOUTH, WEST } Facing;
typedef struct Position {
  int x;
  int y;
  Facing facing;
} Position;

typedef struct PriorityValue {
  int priority;
  Position coords;
} PriorityValue;

typedef struct PriorityQueue {
  PriorityValue values[MAX_PRIORITY_VALUES];
  int len;
} PriorityQueue;

PriorityQueue initialize_queue() {
  PriorityQueue queue;
  queue.len = 0;

  return queue;
}

void heapify_up(PriorityQueue *queue, int index) {
  int parent = (index - 1) / 2;

  while (index > 0 && queue->values[index].priority < queue->values[parent].priority) {
    PriorityValue temp = queue->values[index];
    queue->values[index] = queue->values[parent];
    queue->values[parent] = temp;

    index = parent;
    parent = (index - 1) / 2;
  }
}

void heapify_down(PriorityQueue *queue, int index) {
  if (index >= queue->len) return;

  int left_index = index * 2 + 1;
  int right_index = index * 2 + 2;
  int lowest = index;

  if (left_index < queue->len && queue->values[left_index].priority < queue->values[lowest].priority) {
    lowest = left_index;
  }

  if (right_index < queue->len && queue->values[right_index].priority < queue->values[lowest].priority) {
    lowest = right_index;
  }

  if (lowest != index) {
    PriorityValue temp = queue->values[index];
    queue->values[index] = queue->values[lowest];
    queue->values[lowest] = temp;

    heapify_down(queue, lowest);
  }
}

void insert_queue(PriorityQueue *queue, PriorityValue value) {
  queue->values[queue->len++] = value;
  heapify_up(queue, queue->len - 1);
}

PriorityValue dequeue(PriorityQueue *queue) {
  PriorityValue value = queue->values[0];
  queue->values[0] = queue->values[queue->len - 1];
  queue->len--;
  heapify_down(queue, 0);

  return value;
}

void print_queue(PriorityQueue queue) {
  printf("Queue size -> %d\n", queue.len);
  for (int i = 0; i < queue.len; i++) {
    PriorityValue value = queue.values[i];
    printf("Index %d -> priority %d -> coords {%d, %d}\n", i, value.priority, value.coords.x, value.coords.y);
  }
}

void build_map(FILE *fd, char map[MAP_HEIGHT][MAP_WIDTH]) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      map[y][x] = fgetc(fd);
    }
    fgetc(fd);
  }
}


void print_map(char map[MAP_HEIGHT][MAP_WIDTH]) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      printf("%c", map[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(int x, int y) {
  return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}

int abs(int a) {
  return a < 0 ? -a : a;
}

void dijkstra(char map[MAP_HEIGHT][MAP_WIDTH], Position start) {
  PriorityQueue queue = initialize_queue();
  PriorityValue start_value = { .coords = start, .priority = 0 };
  int costs[MAP_HEIGHT][MAP_WIDTH];
  memset(costs, -1, sizeof(costs));
  // needed to redo the path
  Position comes_from[MAP_HEIGHT][MAP_WIDTH];
  short deltas[4][2] = {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
  };

  insert_queue(&queue, start_value);
  costs[start.y][start.x] = 0;

  while (queue.len > 0) {
    PriorityValue current = dequeue(&queue);
    printf("Current {%d, %d}\n", current.coords.x, current.coords.y);
    if (map[current.coords.y][current.coords.x] == END) {
      break;
    }

    int current_cost = costs[current.coords.y][current.coords.x];
    for (Facing i = NORTH; i <= WEST; i++) {
      short next_x = current.coords.x + deltas[i][0];
      short next_y = current.coords.y + deltas[i][1];

      if (!is_in_bounds(next_x, next_y) || map[next_y][next_x] == WALL) {
        continue;
      }
      // calculate new cost
      PriorityValue next;
      next.priority = current_cost + 1;
      next.coords.x = next_x;
      next.coords.y = next_y;
      next.coords.facing = i;
      Facing direction_delta = abs(current.coords.facing - i);

      // turning clockwise
      if (direction_delta != 0 /*|| direction_delta == 1 || direction_delta == 3 */) {
        next.priority += 1000;
      }

      // Moving to the oposite side (Probably shouldn't add this)
      /* if (direction_delta == 2) { */
      /*   next.priority += 2000; */
      /* } */

      /* if (costs[next_y][next_x] != -1) { */
      printf("current cost to {%d, %d} -> %d, founded cost -> %d\n", next_x, next_y, costs[next_y][next_x], next.priority);
      /* } */
      short next_next_x = next_x + deltas[i][0];
      short next_next_y = next_y + deltas[i][1];

      if (costs[next_y][next_x] == -1 || next.priority < costs[next_y][next_x] || (is_in_bounds(next_next_x, next_next_y) && next.priority + 1 < costs[next_next_y][next_next_x])) {
        insert_queue(&queue, next);
        costs[next_y][next_x] = next.priority;
        comes_from[next_y][next_x] = current.coords;
      }
    }
  }

  printf("founded cost %d\n", costs[1][MAP_WIDTH - 2]);

  int current_x = MAP_WIDTH - 2;
  int current_y = 1;
  int moves = 0;
  int directions = 0;

  printf("start position {%d, %d}\n", start.x, start.y);
  while (!(current_x == start.x && current_y == start.y)) {
    Position current = comes_from[current_y][current_x];
    switch (current.facing) {
      case NORTH:
        map[current.y][current.x] = '^';
        break;
      case EAST:
        map[current.y][current.x] = '>';
        break;
      case SOUTH:
        map[current.y][current.x] = 'v';
        break;
      case WEST:
        map[current.y][current.x] = '<';
        break;
    }
    if (map[current.y][current.x] != map[current_y][current_x]) directions++;
    current_x = current.x;
    current_y = current.y;
    moves++;
  }

  printf("moves %d, directions %d\n", moves, directions);
}

int main() {
  FILE *fd;
  char map[MAP_HEIGHT][MAP_WIDTH];
  Position start = { .x = 1, .y = MAP_HEIGHT - 2, .facing = EAST };

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file\n");
    return -1;
  }

  build_map(fd, map);
  print_map(map);
  dijkstra(map, start);
  print_map(map);

  fclose(fd);
  return 0;
}
