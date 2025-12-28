#include <stdio.h>
#include <string.h>

// #define INPUT_FILE "test.txt"
// #define BOARD_LEN 10
#define INPUT_FILE "input.txt"
#define BOARD_LEN 136
#define EMPTY '.'
#define ROLL '@'
#define ACCESSIBLE 'X'

void parse_board(FILE *fd, char board[BOARD_LEN][BOARD_LEN]) {
  for (int y = 0; y < BOARD_LEN; y++) {
    for (int x = 0; x < BOARD_LEN; x++) {
      board[y][x] = fgetc(fd);
    }
    fgetc(fd);
  }
}

void print_board(char board[BOARD_LEN][BOARD_LEN]) {
  for (int y = 0; y < BOARD_LEN; y++) {
    for (int x = 0; x < BOARD_LEN; x++) {
      printf("%c", board[y][x]);
    }
    printf("\n");
  }
}

short is_out_of_bounds(short x, short y) {
  return x < 0 || x >= BOARD_LEN || y < 0 || y >= BOARD_LEN;
}

short is_accesible(char board[BOARD_LEN][BOARD_LEN], short x, short y) {
  const short deltas[8][2] = {
    { 0, 1 },   // RIGHT
    { 1, 1 },   // RIGHT-DOWN
    { 1, 0 },   // DOWN
    { 1, -1 },  // DOWN-LEFT
    { 0, -1 },  // LEFT
    { -1, -1 }, // LEFT-UP
    { -1, 0 },  // UP
    { -1, 1 },  // RIGHT-UP
  };
  short count = 0;

  for (short i = 0; i < 8; i++) {
    short next_x = x + deltas[i][0];
    short next_y = y + deltas[i][1];


    if (is_out_of_bounds(next_x, next_y)) continue;
    // printf("delta -> {%d,%d} = {%d,%d} -> %c\n", deltas[i][0], deltas[i][1], next_x, next_y, board[next_y][next_x]);
    // if (next_x < 0 || next_y < 0 || board[next_y][next_x] == EMPTY) continue;
    // printf("{%d, %d} -> %c\n", next_x, next_y, board[next_y][next_x]);
    count += board[next_y][next_x] == ROLL;
  }

  // printf("postion {%d,%d} -> count %d\n", x, y, count);

  return count < 4;
}


int main() {
  FILE *fd;
  char board[BOARD_LEN][BOARD_LEN];
  short deltas[8][2] = {
    { 0, 1 },   // RIGHT
    { 1, 1 },   // RIGHT-DOWN
    { 1, 0 },   // DOWN
    { 1, -1 },  // DOWN-LEFT
    { 0, -1 },  // LEFT
    { -1, -1 }, // LEFT-UP
    { -1, 0 },  // UP
    { -1, 1 },  // RIGHT-UP
  };

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  parse_board(fd, board);
  printf("Before processing:\n");
  print_board(board);

  int solution = 0;
  int prev_solution = 0;
  int current_solution = -1;

  short stack[BOARD_LEN * BOARD_LEN][2];
  short visited[BOARD_LEN][BOARD_LEN];

  while (prev_solution != current_solution) {
    prev_solution = current_solution;
    current_solution = 0;

    for (int y = 0; y < BOARD_LEN; y++) {
      for (int x = 0; x < BOARD_LEN; x++) {
        int stack_len = 1;

        memset(stack, 0, sizeof(stack));
        memset(visited, 0, sizeof(visited));
        stack[0][0] = x;
        stack[0][1] = y;

        while (stack_len > 0 ) {
          short current_x = stack[stack_len - 1][0];
          short current_y = stack[stack_len - 1][1];
          stack_len--;

          if (visited[current_y][current_x]) continue;
          visited[current_y][current_x] = 1;
          if (board[current_y][current_x] != ROLL || !is_accesible(board, current_x, current_y)) continue;

          printf("{%d, %d} -> accessible\n", current_x, current_y);
          current_solution++;
          solution++;
          board[current_y][current_x] = ACCESSIBLE;

          for (short i = 0; i < 8; i++) {
            short next_x = current_x + deltas[i][0];
            short next_y = current_y + deltas[i][1];

            if (is_out_of_bounds(next_x, next_y)) continue;

            stack[stack_len][0] = next_x;
            stack[stack_len][1] = next_y;
            stack_len++;
          }
        }
        // print_board(board);
      }
    }
  }

  printf("After processing:\n");
  print_board(board);
  printf("Solution %d\n", solution);

  fclose(fd);
  return 0;
}
