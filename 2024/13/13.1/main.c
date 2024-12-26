#include <stdio.h>

#define INPUT_FILE "input.txt"
#define COST_A 3
#define COST_B 1

typedef struct Position {
  int x;
  int y;
} Position;
typedef Position Button;

int min(int a, int b) {
  return a > b ? b : b;
}

int calculate_min_tokens(Button button_a, Button button_b, Position prize) {
  int iterations_a = min(prize.x / button_a.x, prize.y / button_a.y);
  Position current = { .x = 0, .y = 0 };

  for (int i = 0; i <= iterations_a; i++) {
    Position delta;
    delta.x = prize.x - current.x;
    delta.y = prize.y - current.y;
    current.x += button_a.x;
    current.y += button_a.y;

    if (delta.x % button_b.x != 0 || delta.y % button_b.y != 0) continue;
    int iterations_x = delta.x / button_b.x;
    int iterations_y = delta.y / button_b.y;

    if (iterations_x == iterations_y) {
      /* printf("In X -> %d * %d + %d * %d\n", i, button_a.x, iterations_x, button_b.x); */
      /* printf("In Y -> %d * %d + %d * %d\n", i, button_a.y, iterations_x, button_b.y); */
      return (i * COST_A) + (iterations_x *  COST_B);
    }
  }

  return 0;
}

int main() {
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  int solution = 0;
  while (!feof(fd)) {
    Button button_a, button_b;
    Position prize;

    fscanf(fd, "Button A: X+%d, Y+%d\n", &button_a.x, &button_a.y);
    fscanf(fd, "\nButton B: X+%d, Y+%d", &button_b.x, &button_b.y);
    fscanf(fd, "\nPrize: X=%d, Y=%d", &prize.x, &prize.y);

    /* printf("Button A: X+%d, Y+%d\n", button_a.x, button_a.y); */
    /* printf("Button B: X+%d, Y+%d\n", button_b.x, button_b.y); */
    /* printf("Prize: X=%d, Y=%d\n\n", prize.x, prize.y); */

    solution += calculate_min_tokens(button_a, button_b, prize);

    fgetc(fd);
    fgetc(fd);
  }

  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
