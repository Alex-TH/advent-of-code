#include <stdio.h>
#include <stdlib.h>
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
#define MARK '0'

#define START_PRIORITY_LEN 100
#define MAX_PATH 500

typedef enum { NORTH, EAST, SOUTH, WEST } Facing;
typedef struct Position {
  int x;
  int y;
  Facing facing;
} Position;

typedef struct PriorityValue {
  int priority;
  Position coords;
  Position path[MAX_PATH];
  int path_len;
} PriorityValue;

typedef struct PriorityQueue {
  PriorityValue *values;
  int len;
  int _capacity;
} PriorityQueue;

PriorityQueue initialize_queue() {
  PriorityQueue queue;
  queue.values = (PriorityValue *) malloc(sizeof(PriorityValue) * START_PRIORITY_LEN);
  queue.len = 0;
  queue._capacity = START_PRIORITY_LEN;

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
  if (queue->len + 1 == queue->_capacity) {
    queue->_capacity *= 2;
    queue->values = realloc(queue->values, queue->_capacity * sizeof(PriorityValue));
  }
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
  PriorityValue start_value = { .coords = start, .priority = 0, .path_len = 0 };
  int costs[MAP_HEIGHT][MAP_WIDTH];
  memset(costs, -1, sizeof(costs));
  short deltas[4][2] = {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
  };

  insert_queue(&queue, start_value);
  costs[start.y][start.x] = 0;

  int tile_count = 1;
  while (queue.len > 0) {
    PriorityValue current = dequeue(&queue);
    int current_cost = current.priority;
    /* printf("Current {%d, %d} queue len -> %d \n", current.coords.x, current.coords.y, queue.len); */
    if (map[current.coords.y][current.coords.x] == END) {
      /* printf("Founded path with len -> %d and cost -> %d\n", current.path_len, current_cost); */
      for (int i = 0; i < current.path_len; i++) {
        Position pos = current.path[i];
        if (map[pos.y][pos.x] != MARK) {
          map[pos.y][pos.x] = MARK;
          tile_count++;
        }
      }
      printf("Current tile count %d\n", tile_count);
      /* print_map(map); */
      continue;
    }

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
      next.path_len = current.path_len + 1;

      memcpy(next.path, current.path, sizeof(Position) * (current.path_len));
      next.path[current.path_len].x = current.coords.x;
      next.path[current.path_len].y = current.coords.y;
      next.path[current.path_len].facing = current.coords.facing;

      Facing direction_delta = abs(current.coords.facing - i);

      // turning clockwise
      if (direction_delta != 0) {
        next.priority += 1000;
      }

      short next_next_x = next_x + deltas[i][0];
      short next_next_y = next_y + deltas[i][1];

      if (costs[next_y][next_x] == -1 || next.priority <= costs[next_y][next_x] || (is_in_bounds(next_next_x, next_next_y) && next.priority + 1 <= costs[next_next_y][next_next_x])) {
        insert_queue(&queue, next);
        costs[next_y][next_x] = next.priority;
      }
    }
  }

  printf("founded cost %d\n", costs[1][MAP_WIDTH - 2]);
  printf("Tiles count %d\n", tile_count);
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
