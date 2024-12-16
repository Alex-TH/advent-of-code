#include <stdio.h>

#define BOARD_WIDTH 140
#define BOARD_HEIGHT 140

void build_board(FILE *fd, char board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = fgetc(fd);
    }
    // advance line break
    fgetc(fd);
  }
}

int validate(char board[BOARD_HEIGHT][BOARD_WIDTH], int x, int y) {
  int diagonal_a = (board[y - 1][x - 1] == 'M' && board[y + 1][x + 1] == 'S') || (board[y - 1][x - 1] == 'S' && board[y + 1][x + 1] == 'M');
  int diagonal_b = (board[y - 1][x + 1] == 'M' && board[y + 1][x - 1] == 'S') || (board[y - 1][x + 1] == 'S' && board[y + 1][x - 1] == 'M');

  return diagonal_a && diagonal_b;
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
      if (board[y][x] != 'A') continue;

      if (x >= 1 && x < BOARD_WIDTH - 1 && y >= 1 && y < BOARD_HEIGHT - 1 && validate(board, x, y)) solution++;
    }
  }

  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
