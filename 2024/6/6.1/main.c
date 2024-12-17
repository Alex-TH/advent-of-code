#include <stdio.h>

#define BOARD_WIDTH 130
#define BOARD_HEIGHT 130

#define CELL_BLOCKED '#'
#define CELL_FREE '.'
#define CELL_VISITED 'X'
#define CELL_POSITION '^'

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

void build_board(FILE *fd, char board[BOARD_HEIGHT][BOARD_WIDTH], int current_pos[2]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_HEIGHT; x++) {
      fscanf(fd, "%c", &board[y][x]);
      if (board[y][x] == CELL_POSITION) {
        current_pos[0] = x;
        current_pos[1] = y;
      }
    }
    fgetc(fd);
  }
}

void print_board(char board[BOARD_HEIGHT][BOARD_HEIGHT]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_HEIGHT; x++) {
      printf("%c", board[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(int current_pos[2]) {
  return current_pos[0] >= 0 && current_pos[1] >= 0 && current_pos[0] < BOARD_WIDTH && current_pos[1] < BOARD_HEIGHT;
}

int main() {
  char board[BOARD_HEIGHT][BOARD_WIDTH];
  int coords[4][2] = {
    { 0, -1 }, // UP,
    { 1, 0 }, // RIGHT,
    { 0, 1 }, // DOWN,
    { -1, 0 }, // LEFT,
  };
  FILE *fd;

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  int current_pos[2];
  build_board(fd, board, current_pos);

  int solution = 0;
  char cell_value = CELL_POSITION;
  Direction direction = UP;
  while (is_in_bounds(current_pos)) {
    cell_value = board[current_pos[1]][current_pos[0]];
    board[current_pos[1]][current_pos[0]] = CELL_POSITION;

    if (cell_value == CELL_BLOCKED) {
      board[current_pos[1]][current_pos[0]] = cell_value;
      current_pos[0] -= coords[direction][0];
      current_pos[1] -= coords[direction][1];
      direction = (direction + 1) % 4;
    } else if (cell_value == CELL_FREE || cell_value == CELL_POSITION) {
      solution++;
    }

    board[current_pos[1]][current_pos[0]] = CELL_VISITED;
    // move to direction
    current_pos[0] += coords[direction][0];
    current_pos[1] += coords[direction][1];
  }

  printf("solution %d\n", solution);

  fclose(fd);
  return 0;
}
