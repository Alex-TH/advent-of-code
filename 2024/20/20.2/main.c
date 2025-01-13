#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "input.txt"
/* #define WIDTH 15 */
/* #define HEIGHT 15 */
#define WIDTH 141
#define HEIGHT 141

#define START_PRIORITY_LEN 100
#define MAX_STACK_LEN 8000

#define WALL '#'
#define FREE '.'
#define START 'S'
#define END 'E'

typedef struct Position {
  int x;
  int y;
} Position;

typedef struct PriorityValue {
  int priority;
  Position coords;
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

void parse_input(FILE *fd, char map[HEIGHT][WIDTH], Position *start, Position *end) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      map[y][x] = fgetc(fd);

      if (map[y][x] == START) {
        start->x = x;
        start->y = y;
      }

      if (map[y][x] == END) {
        end->x = x;
        end->y = y;
      }
    }
    fgetc(fd);
  }
}

void print_map(char map[HEIGHT][WIDTH]) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      printf("%c", map[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position pos) {
  return pos.x >= 0 && pos.y >= 0 && pos.x < WIDTH && pos.y < HEIGHT;
}

Position deltas[4] = {
  { .x = 0, .y = -1 },
  { .x = 1, .y = 0 },
  { .x = 0, .y = 1 },
  { .x = -1, .y = 0 },
};
void dijkstra(char map[HEIGHT][WIDTH], unsigned int costs[HEIGHT][WIDTH], Position start, Position end) {
  PriorityQueue queue = initialize_queue();
  PriorityValue initial = { .coords = start, .priority = 0 };

  insert_queue(&queue, initial);
  costs[start.y][start.x] = 0;

  while (queue.len > 0) {
    PriorityValue current = dequeue(&queue);

    if (current.coords.x == end.x && current.coords.y == end.y) {
      continue;
    }

    int next_cost = current.priority + 1;
    for (int i = 0; i < 4; i++) {
      Position next_pos;
      next_pos.x = current.coords.x + deltas[i].x;
      next_pos.y = current.coords.y + deltas[i].y;

      if (is_in_bounds(next_pos) && map[next_pos.y][next_pos.x] != WALL && costs[next_pos.y][next_pos.x] > next_cost) {
        /* printf("next cost = %d\n", next_cost); */
        PriorityValue next = { .coords = next_pos, .priority = next_cost };
        insert_queue(&queue, next);
        costs[next_pos.y][next_pos.x] = next_cost;
      }
    }
  }

  free(queue.values);
}

typedef struct CheatProgress {
  Position pos;
  short progress;
} CheatProgress;

// do a mini dijkstra
int apply_n_cheat(char map[HEIGHT][WIDTH], unsigned int costs[HEIGHT][WIDTH], int n, Position pos) {
  PriorityQueue queue = initialize_queue();
  PriorityValue initial = { .coords = pos, .priority = 0 };
  unsigned short cheat_costs[HEIGHT][WIDTH];
  int result = 0;

  memset(cheat_costs, -1, sizeof(cheat_costs));
  insert_queue(&queue, initial);

  while (queue.len > 0) {
    if (queue.len > 500) printf("queue len -> %d\n", queue.len);
    PriorityValue current = dequeue(&queue);

    /* if (visited[current.coords.y][current.coords.x]) continue; */
    /* visited[current.pos.y][current.pos.x] = 1; */

    if (current.priority == n) {
      // add pos to result
      if (map[current.coords.y][current.coords.x] != WALL) {
        int saving_found = costs[pos.y][pos.x] - costs[current.coords.y][current.coords.x] - n;
        printf("Found cost saving of %d from {%d, %d} to {%d, %d}\n", saving_found, pos.x, pos.y, current.coords.x, current.coords.y);
        if (saving_found >= 100) result++;
      }
      continue;
    }

    int next_priority = current.priority + 1;
    for (int i = 0; i < 4; i++) {
      Position next_pos = { .x = current.coords.x + deltas[i].x, .y = current.coords.y + deltas[i].y };
      if (is_in_bounds(next_pos) && cheat_costs[next_pos.y][next_pos.x] > next_priority) {
        PriorityValue next;
        next.coords = next_pos;
        next.priority = next_priority;
        insert_queue(&queue, next);
        cheat_costs[next_pos.y][next_pos.x] = next_priority;
      }
    }
  }

  free(queue.values);
  return result;
}


int apply_cheats(char map[HEIGHT][WIDTH], unsigned int costs[HEIGHT][WIDTH], Position start, Position end) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (map[y][x] == WALL) printf("#     ");
      else printf("%.4d  ", costs[y][x]);
    }
    printf("\n");
  }

  short visited[HEIGHT][WIDTH] = {{ 0 }};
  Position stack[MAX_STACK_LEN];
  int stack_len = 1;
  int result = 0;

  stack[0] = end;

  while(stack_len > 0) {
    Position current = stack[--stack_len];

    if (visited[current.y][current.x] || (current.x == start.x && current.y == start.y)) {
      continue;
    }
    visited[current.y][current.x] = 1;

    /* int i = 6; */
    for (int i = 2; i <= 20; i++) {
      printf("trying n -> %d\n", i);
      int found = apply_n_cheat(map, costs, i, current);
      result += found;
    }

    // get next
    for (int i = 0; i < 4; i++) {
      Position next = { .x = current.x + deltas[i].x, .y = current.y + deltas[i].y };
      if (!is_in_bounds(next)) continue;

      // apply cheat
      /* Position next_after = { .x = next.x + deltas[i].x, .y = next.y + deltas[i].y }; */
      /* if (map[next.y][next.x] == WALL && !visited[next.y][next.x] && is_in_bounds(next_after) && map[next_after.y][next_after.x] != WALL) { */
      /*   visited[next.y][next.x] = 1; */
      /*   int saving_found = (costs[current.y][current.x] - costs[next_after.y][next_after.x] - 2); */
      /*   printf("found cost saving of %d - %d - 2 = %d\n", costs[current.y][current.x], costs[next_after.y][next_after.x], saving_found); */
      /*   if (saving_found >= 100) { */
      /*     result++; */
      /*   } */
      /* } */

      if (map[next.y][next.x] != WALL) {
        stack[stack_len++] = next;
      }
    }
  }

  return result;
}

int main() {
  FILE *fd;
  char map[HEIGHT][WIDTH];
  Position start;
  Position end;
  // Setting to -1 will set to the highest value
  unsigned int costs[HEIGHT][WIDTH];

  memset(costs, -1, sizeof(costs));
  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  parse_input(fd, map, &start, &end);
  printf("Start {%d, %d} End{%d, %d}\n", start.x, start.y, end.x, end.y);
  print_map(map);
  dijkstra(map, costs, start, end);
  printf("solution %d\n", apply_cheats(map, costs, start, end));

  fclose(fd);
  return 0;
}
