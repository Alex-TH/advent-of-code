#include <stdio.h>
#include <string.h>

#define LINE_BUFFER_SIZE 1024
#define NUMBERS_SIZE 20

unsigned long long int validate(unsigned long long int value, unsigned int numbers[NUMBERS_SIZE], unsigned char len) {
  unsigned int cache_height = 1 << (len - 1);
  unsigned long int cache[cache_height][len];
  for (int x = 0; x < len; x++) {
    int chunk_size = cache_height >> x;
    int chunks = cache_height / chunk_size;

    for (int chunk = 0; chunk < chunks; chunk++) {
      int chunk_offset = chunk * chunk_size;
      for (int y = chunk_offset; y < chunk_offset + chunk_size; y++) {
        if (chunk % 2 == 0) {
          cache[y][x] = x > 0 ? cache[y][x - 1] + numbers[x] : numbers[x];
        } else {
          cache[y][x] = x > 0 ? cache[y][x - 1] * numbers[x] : numbers[x];
        }
      }
    }
  }

  for (int y = 0; y < cache_height; y++) {
    if (cache[y][len - 1] == value) return value;
  }

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
