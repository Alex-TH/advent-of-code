#include<stdio.h>
#include<string.h>

#define INPUT_FILE "input.txt"
#define MAX_WIDTH 1000
#define MAX_NUM_LINES 6
#define BUFFER_SIZE 4000

#define PLUS '+'
#define MULTIPLY '*'

typedef unsigned long long int u64;
typedef struct Board {
  char values[MAX_NUM_LINES][BUFFER_SIZE];
  int width;
  int height;
} Board;

u64 apply_operation(u64 left, u64 right, char operator) {
  if (operator == PLUS) return left + right;

  return left * right;
}

void parse_board(FILE *fd, Board *board) {
  board->width = 0;
  board->height = 0;

  while (fgets(board->values[board->height], BUFFER_SIZE, fd) != NULL) {
    int len = strlen(board->values[board->height]) - 1;

    printf("%s", board->values[board->height]);
    board->width = len > board->width ? len : board->width;
    board->height++;
  }
}

void print_board(Board board) {
  for (int y = 0; y < board.height; y++) {
    for (int x = 0; x < board.width; x++) {
      printf("%c", board.values[y][x]);
    }
    printf("\n");
  }
}

int pow_10(short exp) {
  int value = 1;
  for (short i = 1; i < exp; i++) {
    value *= 10;
  }

  return value;
}

short is_number(char value) {
  return value >= '0' && value <= '9';
}

int parse_number(Board board, int x) {
  int exp = 1;
  int number = 0;
  for (int y = board.height - 2; y > -1; y--) {
    char value = board.values[y][x];
    // printf("\"%c\"", value);
    if (!is_number(value)) continue;

    number += pow_10(exp++) * (value - '0');
  }
  // printf("\n");

  return number;
}

int main() {
  FILE *fd;
  Board board;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  parse_board(fd, &board);
  printf("%dX%d\n", board.width, board.height);
  print_board(board);

  u64 solution = 0;
  u64 partial = 0;
  char operator = 0;
  for (int x = 0; x < board.width; x++) {
    char last_row = board.values[board.height - 1][x];
    if ((last_row == PLUS || last_row == MULTIPLY)) {
      printf("Partial %llu + %llu\n", partial, solution);
      solution += partial;
      operator = last_row;
      partial = 0;
    }

    int number = parse_number(board, x);
    if (number == 0) continue;

    printf("%llu %c %d\n", partial, operator, number);
    partial = partial == 0 ? number : apply_operation(partial, number, operator);
  }
  printf("Partial %llu + %llu\n", partial, solution);
  solution += partial;

  printf("solution: %llu\n", solution);

  fclose(fd);
  return 0;
}
