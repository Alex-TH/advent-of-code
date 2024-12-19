#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_BUFFER_SIZE 1024
#define NUMBERS_SIZE 20

typedef enum { SUM, MUL, CONCAT } Operator;

unsigned int pow_(unsigned int number, unsigned int power) {
  unsigned int result = 1;
  for (int i = 0; i < power; i++) {
    result *= number;
  }

  return result;
}

unsigned long int concat_numbers(unsigned long int a, unsigned long int b) {
  unsigned long int result;
  char number_str[25];

  sprintf(number_str, "%lu%lu", a, b);
  sscanf(number_str, "%lu", &result);

  return result;
}

unsigned long long int validate(unsigned long long int value, unsigned int numbers[NUMBERS_SIZE], unsigned char len) {

  unsigned int cache_height = pow_(3, len - 1);
  unsigned long int **cache = malloc(sizeof(unsigned long int) * cache_height);
  for (int y = 0; y < cache_height; y++) {
    cache[y] = malloc(sizeof(unsigned long int) * len);
  }

  int chunk_size = cache_height;
  for (int x = 0; x < len; x++) {
    int chunks = cache_height / chunk_size;

    for (int chunk = 0; chunk < chunks; chunk++) {
      int chunk_offset = chunk * chunk_size;
      for (int y = chunk_offset; y < chunk_offset + chunk_size; y++) {
        Operator operator = chunk % 3;
        if (x > 0 && cache[y][x - 1] > value) {
          // mark and skip
          cache[y][x] = -1;
          continue;
        }
        switch (operator) {
          case SUM: {
            cache[y][x] = x > 0 ? cache[y][x - 1] + numbers[x] : numbers[x];
            break;
          }
          case MUL: {
            cache[y][x] = x > 0 ? cache[y][x - 1] * numbers[x] : numbers[x];
            break;
          }
          case CONCAT:
            cache[y][x] = x > 0 ? concat_numbers(cache[y][x - 1], numbers[x]) : numbers[x];
            break;
        }
      }
    }

    chunk_size /= 3;
  }
  
  for (int y = 0; y < cache_height; y++) {
    if (cache[y][len - 1] == value) return value;
  }

  free(cache);
  return 0;
}

int main() {
  FILE *fd;
  char line[LINE_BUFFER_SIZE];

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  unsigned long long int solution = 0;
  while (fgets(line, LINE_BUFFER_SIZE, fd) != NULL) {
    unsigned long long int value;
    unsigned int numbers[NUMBERS_SIZE];
    unsigned char len = 0;
    char *token;

    token = strtok(line, " ");
    sscanf(token, "%llu", &value);
    token = strtok(NULL, " ");

    while (token != NULL) {
      sscanf(token, "%u", &numbers[len++]);
      token = strtok(NULL, " ");
    }

    solution += validate(value, numbers, len);
  }

  printf("solution: %llu\n", solution);

  fclose(fd);
  return 0;
}
