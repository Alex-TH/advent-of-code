#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STONES 1000000
#define ITERATIONS 25

typedef struct Stone {
  char digits[20];
  int len;
} Stone;

int parse_input(FILE *fd, Stone stones[MAX_STONES]) {
  int len = 0;

  while (!feof(fd)) {
    Stone current;
    /* current.digits = malloc(sizeof(char) * 20); */
    int stone_len = 0;
    char current_char = fgetc(fd);
    while (current_char >= '0' && current_char <= '9') {
      current.digits[stone_len++] = current_char;
      current_char = fgetc(fd);
    }

    if (stone_len > 0) {
      current.len = stone_len;
      stones[len++] = current;
    }
  }

  return len;
}

unsigned long long int stone_to_number(Stone stone) {
  unsigned long long int result = 0;
  unsigned long long int exp = 1;

  for (int i = stone.len - 1; i >= 0; i--) {
    result += exp * (stone.digits[i] - '0');
    exp *= 10;
  }

  return result;
}

Stone number_to_stone(unsigned long long int number) {
  Stone result;
  result.len = 0;

  while (number != 0) {
    result.digits[result.len++] = (number % 10) + '0';
    number /= 10;
  }

  // reverse digits
  for (int i = 0; i < result.len / 2; i++) {
    char temp = result.digits[i];
    result.digits[i] = result.digits[result.len - 1 -i];
    result.digits[result.len - 1 - i] = temp;
  }

  return result;
}

void print_stones(Stone *stones, int total_stones) {
  printf("%d stones\n", total_stones);
  for (int i = 0; i < total_stones; i++) {
    for (int j = 0; j < stones[i].len; j++) {
      printf("%c", stones[i].digits[j]);
    }
    printf(" ");
  }
  printf("\n");
}

int blink(Stone *stones, int total_stones) {
  int new_len = total_stones;
  for (int i = 0; i < new_len; i++) {
    Stone current = stones[i];

    if (current.len == 1 && current.digits[0] == '0') {
      stones[i].digits[0] = '1';
      continue;
    }
    if (current.len % 2 == 0) {
      memmove(stones + i + 1, stones + i, sizeof(Stone) * (new_len - i));
      stones[i].len /= 2;

      int right_len = 0;
      int skip_zero = 1;
      for (int j = stones[i + 1].len / 2; j < stones[i + 1].len; j++) {
        if (skip_zero && stones[i].digits[j] == '0') continue;
        /* printf("%c", stones[i].digits[j]); */
        stones[i + 1].digits[right_len++] = stones[i].digits[j];
        skip_zero = 0;
      }
      stones[i + 1].len = right_len;

      if (right_len == 0) {
        stones[i + 1].digits[0] = '0';
        stones[i + 1].len = 1;
      }

      i++;
      new_len++;
      continue;
    }
    unsigned long long int new_number = stone_to_number(current) * 2024;
    stones[i] = number_to_stone(new_number);
  }

  return new_len;
}

int main() {
  FILE *fd;
  Stone *stones;
  int total_stones = 0;

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }
  stones = malloc(sizeof(Stone) * MAX_STONES);

  printf("parsing\n");
  total_stones = parse_input(fd, stones);
  printf("Initial arrangement \n");
  print_stones(stones, total_stones);
  
  for (int i = 0; i < ITERATIONS; i++) {
    total_stones = blink(stones, total_stones);
    printf("After blinking %d times\n", i + 1);
    printf("%d stones\n", total_stones);
    /* print_stones(stones, total_stones); */
  }

  printf("solution: %d\n", total_stones);

  free(stones);
  fclose(fd);
  return 0;
}
