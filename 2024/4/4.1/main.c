#include <stdio.h>

#define BOARD_WIDTH 140
#define BOARD_HEIGHT 140

typedef enum { LEFT, RIGTH, UP, DOWN, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT } Direction;

void build_board(FILE *fd, char board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = fgetc(fd);
    }
    // advance line break
    fgetc(fd);
  }
}

int validate(char board[BOARD_HEIGHT][BOARD_WIDTH], int x, int y, Direction direction) {
  switch (direction) {
    case LEFT: return board[y][x - 1] == 'M' && board[y][x - 2] == 'A' && board[y][x - 3] == 'S';
    case RIGTH: return board[y][x + 1] == 'M' && board[y][x + 2] == 'A' && board[y][x + 3] == 'S';
    case DOWN: return board[y + 1][x] == 'M' && board[y + 2][x] == 'A' && board[y + 3][x] == 'S';
    case UP: return board[y - 1][x] == 'M' && board[y - 2][x] == 'A' && board[y - 3][x] == 'S';
    case DOWN_LEFT: return board[y + 1][x - 1] == 'M' && board[y + 2][x - 2] == 'A' && board[y + 3][x - 3] == 'S';
    case DOWN_RIGHT: return board[y + 1][x + 1] == 'M' && board[y + 2][x + 2] == 'A' && board[y + 3][x + 3] == 'S';
    case UP_LEFT: return board[y - 1][x - 1] == 'M' && board[y - 2][x - 2] == 'A' && board[y - 3][x - 3] == 'S';
    case UP_RIGHT: return board[y - 1][x + 1] == 'M' && board[y - 2][x + 2] == 'A' && board[y - 3][x + 3] == 'S';
    default: return 0;
  }
}

int main() {
  FILE *fd;
  char board[BOARD_HEIGHT][BOARD_WIDTH];

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }
  build_board(fd, board);

  int solution = 0;
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      if (board[y][x] != 'X') continue;

      int has_space_left = x >= 3;
      int has_space_right = x < BOARD_WIDTH - 3;
      int has_space_up = y >= 3;
      int has_space_down = y < BOARD_HEIGHT - 3;

      if (has_space_left && validate(board, x, y, LEFT)) solution++;
      if (has_space_right && validate(board, x, y, RIGTH)) solution++;
      if (has_space_up && validate(board, x, y, UP)) solution++;
      if (has_space_down && validate(board, x, y, DOWN)) solution++;

      if (has_space_left && has_space_up && validate(board, x, y, UP_LEFT)) solution++;
      if (has_space_left && has_space_down && validate(board, x, y, DOWN_LEFT)) solution++;
      if (has_space_right && has_space_up && validate(board, x, y, UP_RIGHT)) solution++;
      if (has_space_right && has_space_down && validate(board, x, y, DOWN_RIGHT)) solution++;
    }
  }

  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
