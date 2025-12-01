#include <stdio.h>

#define LEFT 'L'
#define RIGHT 'R'
#define MAX 99
#define MIN 0
#define INITIAL_POS 50

#define INPUT_FILE "input.txt"

int main () {
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd <= 0) {
    perror("Missing input file");
    return -1;
  }

  char direction;
  int clicks;
  int current_pos = INITIAL_POS;
  int solution = 0;

  while (fscanf(fd, "%c%d\n", &direction, &clicks) != EOF) {
    int abs_clicks = clicks % (MAX + 1);
    int rounds = clicks / (MAX + 1);

    if (direction == LEFT) {
      if (current_pos < abs_clicks && current_pos != 0) solution++;
      current_pos = current_pos >= abs_clicks ? (current_pos - abs_clicks) : (MAX + 1 + (current_pos - abs_clicks));
    }

    if (direction == RIGHT) {
      if (current_pos + abs_clicks > MAX + 1) solution++;
      current_pos = (current_pos + abs_clicks) % (MAX + 1);
    }

    solution += rounds;
    if (current_pos == 0 && abs_clicks > 0) solution++;
    printf("%c%d -> current_pos: %d solution: %d\n", direction, clicks, current_pos, solution);
  }

  printf("solution: %d\n", solution);
  return 0;
}
