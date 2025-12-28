#include <stdio.h>
#include <string.h>

#define INPUT_FILE "input.txt"
#define MAX_WIDTH 150
#define MAX_HEIGHT 150
#define MAX_BEAMS 22500

#define START 'S'
#define SPLITER '^'
#define EMPTY '.'
#define BEAM '|'

typedef unsigned long long int u64;
typedef enum Direction { Left, Right } Direction;

typedef struct Board {
  char values[MAX_HEIGHT][MAX_WIDTH];
  short width;
  short height;
} Board;
typedef struct Position {
  short x;
  short y;
  Direction direction;
} Position;

void parse_board(Board *board, FILE *fd) {
  // char line[MAX_WIDTH];
  board->width = 0;
  board->height = 0;
  int x = 0;
  char current;

  while ((current = fgetc(fd)) != EOF) {
    if (current == '\n') {
      board->height++;
      board->width = x;
      x = 0;

      continue;
    }

    board->values[board->height][x] = current;
    x++;
  }
};

short is_in_bounds(Position position, Board board) {
  return position.x >= 0 && position.y >= 0 && position.x < board.width && position.y < board.height;
}

void print_board(Board board) {
  for (int y = 0; y < board.height; y++) {
    for (int x = 0; x < board.width; x++) {
      printf("%c", board.values[y][x]);
    }
    printf("\n");
  }
}

u64 traversal(Board *board, Position position, u64 cache[MAX_HEIGHT][MAX_WIDTH]) {
  if (!is_in_bounds(position, *board)) return 0;
  if (cache[position.y][position.x] > 0) {
    printf("Cache hit!\n");
    return cache[position.y][position.x];
  }

  if (position.y == board->height - 1) return 1;

  char value = board->values[position.y][position.x];

  if (value != SPLITER) {
    if (value == EMPTY) board->values[position.y][position.x] = BEAM;

    Position down = { .x = position.x, .y = position.y + 1 };
    u64 result = traversal(board, down, cache);
    cache[position.y][position.x] = result;

    return result;
  }

  Position left = { .x = position.x - 1, .y = position.y };
  Position right = { .x = position.x + 1, .y = position.y };

  u64 result = traversal(board, left, cache) + traversal(board, right, cache);
  cache[position.y][position.x] = result;

  return result;
}

int main() {
  FILE *fd;
  Board board;
  u64 cache[MAX_HEIGHT][MAX_WIDTH];

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file\n");
    return -1;
  }

  parse_board(&board, fd);
  print_board(board);
  memset(cache, 0, sizeof(cache));

  Position start = { .x = 0, .y = 0, .direction = Left };
  while (is_in_bounds(start, board) && board.values[start.y][start.x] != START) {
    start.x++;
  }

  printf("Starting at {%d,%d}\n", start.x, start.y);
  // Position beams[MAX_BEAMS];
  // int beam_len = 1;
  // int beams_end = 1;
  // beams[0] = start;

  u64 solution = traversal(&board, start, cache);
  // while (beam_len > 0) {
  //   int next_beams_end = beams_end == 0 ? MAX_BEAMS - 1 : beams_end - 1;
  //   Position current = beams[beams_end - 1];

  //   printf("Current at {%d,%d} -> beam len = %d\n", current.x, current.y, beam_len);
  //   if (!is_in_bounds(current, board)) {
  //     beams_end = next_beams_end;
  //     beam_len--;
  //     continue;
  //   }
    
  //   char value = board.values[current.y][current.x];

  //   if (value != SPLITER) {
  //   // if (value == EMPTY || value == START) {
  //     if (value == EMPTY) board.values[current.y][current.x] = BEAM;

  //     beams_end = next_beams_end;
  //     beam_len--;

  //     beams[beams_end].x = current.x;
  //     beams[beams_end].y = current.y + 1;
  //     beams[beams_end].direction = Left;

  //     beams_end = (beams_end + 1) % MAX_BEAMS;
  //     beam_len++;
  //   }

  //   if (value == SPLITER) {
  //     if (current.direction == Left) {
  //       beams[beams_end - 1].direction = Right;
  //       beams[beams_end].x = current.x - 1;
  //       beams[beams_end].y = current.y;
  //       beams[beams_end].direction = Left;

  //       beams_end = (beams_end + 1) % MAX_BEAMS;
  //       beam_len++;
  //     } else {
  //       beams_end = next_beams_end;
  //       beam_len--;

  //       beams[beams_end].x = current.x + 1;
  //       beams[beams_end].y = current.y;
  //       beams[beams_end].direction = Left;

  //       beams_end = (beams_end + 1) % MAX_BEAMS;
  //       beam_len++;
  //     }
  //   }

  //   if (current.y == board.height - 1) {
  //     solution++;
  //     printf("current solution %llu\n", solution);
  //   }
  // }

  print_board(board);
  printf("Solution %llu\n", solution);

  fclose(fd);
  return 0;
}
