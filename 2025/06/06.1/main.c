#include<stdio.h>
#include<string.h>

#define INPUT_FILE "input.txt"
#define MAX_WIDTH 1000
#define MAX_NUM_LINES 4
#define BUFFER_SIZE 4000

#define PLUS '+'
#define MULTIPLY '*'

typedef unsigned long long int u64;

u64 apply_operation(u64 left, u64 right, char operator) {
  if (operator == PLUS) return left + right;

  return left * right;
}

int main() {
  FILE *fd;
  u64 numbers[MAX_NUM_LINES][MAX_WIDTH];
  char operators[MAX_WIDTH];
  char line[BUFFER_SIZE];

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  short lines_count = 0;
  int number_count = 0;
  while (fgets(line, BUFFER_SIZE, fd) != NULL) {
    int current_count = 0;
    char *token = strtok(line, " ");
    // sscanf(token, "%llu ", &numbers[lines_count][number_count]);
    // printf("%llu\n", numbers[lines_count][number_count]);

    while (token != NULL) {
      if (sscanf(token, "%llu ", &numbers[lines_count][current_count]) != 1) {
        sscanf(token, "%c ", &operators[current_count]);
        if (operators[current_count] != PLUS && operators[current_count] != MULTIPLY) {
          token = strtok(NULL, " ");
          continue;
        }
        printf("%c ", operators[current_count]);
      } else {
        printf("%llu ", numbers[lines_count][current_count]);
      }

      current_count++;
      token = strtok(NULL, " ");
    }

    printf("\n");
    if (current_count > 0) {
      lines_count++;
      // printf("Prev count: %d, current count: %d\n", number_count, current_count);
      number_count = current_count;
    }
  }
  printf("\ntotal lines: %d each line has %d numbers\n", lines_count, number_count);

  u64 solution = 0;
  for (int x = 0; x < number_count; x++) {
    u64 partial = numbers[0][x];
    char operator = operators[x];

    for (int y = 1; y < lines_count - 1; y++) {
      partial = apply_operation(partial, numbers[y][x], operator);

      if (y == lines_count - 2) {
        printf("%llu %c %llu = %llu\n", partial, operator, numbers[y][x], partial);
      }
    }

    // printf("Partial %llu + %llu\n", solution, partial);
    solution += partial;
  }

  printf("\nsolution: %llu\n", solution);

  fclose(fd);
  return 0;
}
