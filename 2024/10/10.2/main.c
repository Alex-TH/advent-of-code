#include <stdio.h>

/* #define BOARD_WIDTH 8 */
/* #define BOARD_HEIGHT 8 */

#define BOARD_WIDTH 54
#define BOARD_HEIGHT 54

typedef struct Position {
  unsigned short height;
  short x;
  short y;
} Position;

void build_board(FILE *fd, unsigned short board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      board[y][x] = fgetc(fd) - '0';
    }
    fgetc(fd);
  }
}

void print_board(unsigned short board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      printf("%hu", board[y][x]);
    }
    printf("\n");
  }
}

unsigned short build_start_positions(unsigned short board[BOARD_HEIGHT][BOARD_WIDTH], Position start_positions[BOARD_HEIGHT * BOARD_WIDTH]) {
  unsigned short len = 0;
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      if (board[y][x] == 0) {
        Position position;
        position.height = 0;
        position.x = x;
        position.y = y;
        start_positions[len++] = position;
      }
    }
  }

  return len;
}

int is_in_bounds(Position pos) {
  return pos.x >= 0 && pos.y >= 0 && pos.x < BOARD_WIDTH && pos.x < BOARD_HEIGHT;
}

int calculate_score(unsigned short board[BOARD_HEIGHT][BOARD_WIDTH], Position start) {
  Position stack[BOARD_HEIGHT * BOARD_WIDTH];
  unsigned short len = 1;
  int score = 0;
  stack[0] = start;

  while (len > 0) {
    Position current = stack[--len];
    if (current.height == 9){
      score++;
      continue;
    }

    unsigned short next_height = current.height + 1;
    Position up = { .x = current.x, .y = current.y - 1, .height = next_height };
    Position down = { .x = current.x, .y = current.y + 1, .height = next_height };
    Position left = { .x = current.x - 1, .y = current.y, .height = next_height };
    Position right = { .x = current.x + 1, .y = current.y, .height = next_height };

    if (is_in_bounds(up) && board[up.y][up.x] == next_height) stack[len++] = up;
    if (is_in_bounds(down) && board[down.y][down.x] == next_height) stack[len++] = down;
    if (is_in_bounds(left) && board[left.y][left.x] == next_height) stack[len++] = left;
    if (is_in_bounds(right) && board[right.y][right.x] == next_height) stack[len++] = right;
  }

  return score;
}

int main() {
  FILE *fd;
  unsigned short board[BOARD_HEIGHT][BOARD_WIDTH];
  Position start_positions[BOARD_WIDTH * BOARD_HEIGHT];
  unsigned short positions_len;

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  build_board(fd, board);
  print_board(board);
  positions_len = build_start_positions(board, start_positions);

  int solution = 0;
  for (int i = 0; i < positions_len; i++) {
    solution += calculate_score(board, start_positions[i]);
  }
  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
