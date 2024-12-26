#include <stdio.h>

#define INPUT_FILE "input.txt"

#define COST_A 3
#define COST_B 1
/* #define DELTA_PRIZE 0; */
#define DELTA_PRIZE 10000000000000;

typedef struct Position {
  long long int x;
  long long int y;
} Position;
typedef Position Button;

long long int min(long long int a, long long int b) {
  return a > b ? b : b;
}

long long int calculate_min_tokens(Button button_a, Button button_b, Position prize) {
  // solving the following equation system:
  // 1.- button_a.x * tokens_a + button_b.x * tokens_b = prize.x
  // 2.- button_a.y * tokens_a + button_b.y * tokens_b = prize.y

  long long int tokens_a = ((button_b.x * prize.y) - (button_b.y * prize.x)) / ((button_b.x * button_a.y) - (button_a.x * button_b.y));
  long long int tokens_b = (prize.x - button_a.x * tokens_a) / button_b.x;

  if ((button_a.x * tokens_a + button_b.x * tokens_b) == prize.x && (button_a.y * tokens_a + button_b.y * tokens_b) == prize.y) {
    printf("A -> %lld B -> %lld\n", tokens_a, tokens_b);
    return COST_A * tokens_a + COST_B * tokens_b;
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

  long long int solution = 0;
  while (!feof(fd)) {
    Button button_a, button_b;
    Position prize;

    fscanf(fd, "Button A: X+%lld, Y+%lld\n", &button_a.x, &button_a.y);
    fscanf(fd, "\nButton B: X+%lld, Y+%lld", &button_b.x, &button_b.y);
    fscanf(fd, "\nPrize: X=%lld, Y=%lld", &prize.x, &prize.y);

    prize.x += DELTA_PRIZE;
    prize.y += DELTA_PRIZE;

    printf("Button A: X+%lld, Y+%lld\n", button_a.x, button_a.y);
    printf("Button B: X+%lld, Y+%lld\n", button_b.x, button_b.y);
    printf("Prize: X=%lld, Y=%lld\n\n", prize.x, prize.y);

    solution += calculate_min_tokens(button_a, button_b, prize);

    fgetc(fd);
    fgetc(fd);
  }

  printf("solution: %lld\n", solution);

  fclose(fd);
  return 0;
}
