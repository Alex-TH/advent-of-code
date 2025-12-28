#include <stdio.h>

#define INPUT_FILE "input.txt"
#define MAX_WIDTH 150
#define MAX_HEIGHT 150
#define MAX_BEAMS 22500

#define START 'S'
#define SPLITER '^'
#define EMPTY '.'
#define BEAM '|'

typedef unsigned int u16;
typedef struct Board {
  char values[MAX_HEIGHT][MAX_WIDTH];
  short width;
  short height;
} Board;
typedef struct Position {
  short x;
  short y;
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

int main() {
  FILE *fd;
  Board board;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file\n");
    return -1;
  }

  parse_board(&board, fd);
  print_board(board);

  Position start = { .x = 0, .y = 0 };
  while (is_in_bounds(start, board) && board.values[start.y][start.x] != START) {
    start.x++;
  }

  printf("Starting at {%d,%d}\n", start.x, start.y);
  Position beams[MAX_BEAMS];
  int beam_len = 1;
  int beams_start = 0;
  int beams_end = 1;
  beams[0] = start;

  u16 solution = 0;
  while (beam_len > 0) {
    Position current = beams[beams_start];
    beams_start = (beams_start + 1) % MAX_BEAMS;

    printf("Current at {%d,%d} -> beam len = %d\n", current.x, current.y, beam_len);
    beam_len--;
    if (!is_in_bounds(current, board) || board.values[current.y][current.x] == BEAM) continue;
    
    char value = board.values[current.y][current.x];

    if (value == EMPTY || value == START) {
      if (value == EMPTY) board.values[current.y][current.x] = BEAM;

      beams[beams_end].x = current.x;
      beams[beams_end].y = current.y + 1;
      beams_end = (beams_end + 1) % MAX_BEAMS;
      beam_len++;
    } else {
      solution++;

      beams[beams_end].x = current.x - 1;
      beams[beams_end].y = current.y;
      beams_end = (beams_end + 1) % MAX_BEAMS;
      beam_len++;

      beams[beams_end].x = current.x + 1;
      beams[beams_end].y = current.y;
      beams_end = (beams_end + 1) % MAX_BEAMS;
      beam_len++;
    }

  }

  print_board(board);
  printf("Solution %u\n", solution);

  fclose(fd);
  return 0;
}
