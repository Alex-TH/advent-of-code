#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"

#define WIDTH 71
#define HEIGHT 71
#define MAX_QUEUE_LEN 5041

/* #define WIDTH 7 */
/* #define HEIGHT 7 */
/* #define MAX_QUEUE_LEN 49 */

#define FREE '.'
#define WALL '#'
#define VISITED 'O'

typedef struct Position {
  short x;
  short y;
} Position;
typedef struct SearchResult {
  Position pos;
  int path_len;
} SearchResult;

int process_input(FILE *fd, Position *corrupted_positions) {
  Position pos;
  int bytes_count = 0;

  while(fscanf(fd, "\n%hd,%hd\n", &pos.x, &pos.y) != EOF) {
    printf("%d,%d\n", pos.x, pos.y);
    corrupted_positions[bytes_count++] = pos;
  }

  return bytes_count;
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

SearchResult get_min_len(char map[HEIGHT][WIDTH], Position start, Position end) {
  SearchResult result;
  SearchResult queue[MAX_QUEUE_LEN];
  int queue_start = 0;
  int queue_len = 1;
  short deltas[4][2] = {
    { 0, -1 }, // UP
    { 1, 0 }, // RIGTH
    { 0, 1 }, // DOWN
    { -1, 0 }, // LEFT
  };

  result.path_len = -1;
  queue[0].pos = start;
  queue[0].path_len = 0;

  while (queue_len > 0) {
    SearchResult current = queue[queue_start];
    queue_start = (queue_start + 1) % MAX_QUEUE_LEN;
    queue_len--;

    if (map[current.pos.y][current.pos.x] == VISITED) continue;
    map[current.pos.y][current.pos.x] = VISITED;
    if (current.pos.x == end.x && current.pos.y == end.y) {
      result = current;
      break;
    }

    int next_len = current.path_len + 1;

    for (int i = 0; i < 4; i++) {
      short delta_x = deltas[i][0];
      short delta_y = deltas[i][1];
      Position next = { .x = current.pos.x + delta_x, .y = current.pos.y + delta_y };

      if (is_in_bounds(next) && map[next.y][next.x] == FREE) {
        int index = (queue_start + queue_len) % MAX_QUEUE_LEN;
        queue[index].pos = next;
        queue[index].path_len = next_len;
        queue_len++;
      }
    }
  }

  return result;
}

int main() {
  FILE *fd;
  char map[HEIGHT][WIDTH];
  SearchResult result;
  Position start_pos = { .x = 0, .y = 0 };
  Position end_pos = { .x = WIDTH - 1, .y = HEIGHT - 1 };
  Position *corrupted_positions;
  int bytes_count;

  printf("holis\n");
  fd = fopen(INPUT_FILE, "r");
  corrupted_positions = malloc(sizeof(Position) * MAX_QUEUE_LEN);

  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  bytes_count = process_input(fd, corrupted_positions);

  int start = 0;
  int end = bytes_count - 1;
  Position result_pos;
  
  while (start < end) {
    int middle = (start + end) / 2;
    memset(map, FREE, sizeof(map));
    for (int i = 0; i <= middle; i++) {
      Position current = corrupted_positions[i];
      map[current.y][current.x] = WALL;
    }

    printf("trying %d {%d, %d} -> start: %d, end %d\n", middle, corrupted_positions[middle].x, corrupted_positions[middle].y, start, end);
    print_map(map);
    result = get_min_len(map, start_pos, end_pos);

    // Not founded? go to the left half
    if (result.path_len == -1) {
      end = middle;
      result_pos = corrupted_positions[middle];
    } else {
      start = middle + 1;
    }
  }

  printf("Solution %d,%d\n", result_pos.x, result_pos.y);

  free(corrupted_positions);
  fclose(fd);
  return 0;
}
