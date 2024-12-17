#include <stdio.h>

/* #define BOARD_WIDTH 10 */
/* #define BOARD_HEIGHT 10 */

#define BOARD_WIDTH 130
#define BOARD_HEIGHT 130

#define CELL_BLOCKED '#'
#define CELL_FREE '.'
#define CELL_USED '0'
#define CELL_POSITION '^'

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;
typedef struct Position {
  int x;
  int y;
  Direction facing;
}Position;

void build_board(FILE *fd, char board[BOARD_HEIGHT][BOARD_WIDTH], Position *current_pos) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      fscanf(fd, "%c", &board[y][x]);
      if (board[y][x] == CELL_POSITION) {
        current_pos->x = x;
        current_pos->y = y;
        current_pos->facing = UP;
      }
    }
    fgetc(fd);
  }
}

void print_board(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_HEIGHT; x++) {
      printf("%c", board[y][x]);
    }
    printf("\n");
  }
}

int is_in_bounds(Position current_pos) {
  return current_pos.x >= 0 && current_pos.y >= 0 && current_pos.x < BOARD_WIDTH && current_pos.y < BOARD_HEIGHT;
}

void print_path(Position path[BOARD_HEIGHT * BOARD_WIDTH], unsigned int len) {
  for (int i = 0; i < len; i++) {
    printf("%d.- {%d, %d} Facing -> %d\n", i + 1, path[i].x, path[i].y, path[i].facing);
  }
}

void next_position(char board[BOARD_HEIGHT][BOARD_WIDTH], Position *current_pos, int coords[4][2]) {
  char cell_value = board[current_pos->y][current_pos->x];

  if (cell_value == CELL_BLOCKED) {
    current_pos->x -= coords[current_pos->facing][0];
    current_pos->y -= coords[current_pos->facing][1];
    current_pos->facing= (current_pos->facing + 1) % 4;
  }
  current_pos->x += coords[current_pos->facing][0];
  current_pos->y += coords[current_pos->facing][1];
}

int has_loop(char board[BOARD_HEIGHT][BOARD_WIDTH], Position start_pos, Position blocker_pos, int coords[4][2]) {
  Position current_pos = {start_pos.x, start_pos.y, start_pos.facing};
  Position slow = {start_pos.x, start_pos.y, start_pos.facing};

  char cell_value = board[blocker_pos.y][blocker_pos.x];
  if (!is_in_bounds(blocker_pos) || cell_value == CELL_USED || cell_value == CELL_BLOCKED) return 0;

  board[blocker_pos.y][blocker_pos.x] = CELL_BLOCKED;
  next_position(board, &current_pos, coords);
  next_position(board, &current_pos, coords);
  printf("analizing {%d, %d} facing %d\n", start_pos.x, start_pos.y, start_pos.facing);


  int move_slow = 0;
  while (is_in_bounds(current_pos) && !(current_pos.x == slow.x && current_pos.y == slow.y && current_pos.facing == slow.facing)) {
    next_position(board, &current_pos, coords);
    if (move_slow) next_position(board, &slow, coords);
    move_slow = (move_slow + 1) % 2;
  }

  if (is_in_bounds(current_pos)) board[blocker_pos.y][blocker_pos.x] = CELL_USED;
  else board[blocker_pos.y][blocker_pos.x] = cell_value;

  return is_in_bounds(current_pos);
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

  Position blocker_pos;
  Position start_pos;
  build_board(fd, board, &start_pos);

  int solution = 0;
  print_board(board);
  board[start_pos.y][start_pos.x] = CELL_USED;
  board[start_pos.y - 1][start_pos.x] = CELL_USED;
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      blocker_pos.x = x;
      blocker_pos.y = y;
      solution += has_loop(board, start_pos, blocker_pos, coords);
    }
  }
  print_board(board);

  printf("solution %d\n", solution);

  fclose(fd);
  return 0;
}
