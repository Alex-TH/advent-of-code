#include <stdio.h>
#include <string.h>

#define LINE_BUFFER_SIZE 1024
#define MAX_LINE_SIZE 10
#define MAX_TOLERANCE 1

int abs (int value) {
  return value >= 0 ? value : -value;
}

int is_safe (int numbers[MAX_LINE_SIZE], unsigned char len, unsigned char tolerance) {
  char desc;

  for (int i = 1; i < len; i++) {
    char diff = numbers[i] - numbers[i - 1];
    unsigned char abs_diff = abs(diff);
    char current_desc = diff < 0;

    if (i == 1) desc = current_desc;
    if (desc != current_desc || abs_diff == 0 || abs_diff > 3) {
      if (tolerance == 0) return 0;
      int numbers_copy[len - 1];

      for (int j = 0; j <= i; j++) {
        if (j > 0) memcpy(numbers_copy, numbers, j * sizeof(int));
        memcpy(numbers_copy + j, numbers + j + 1, (len - j - 1) * sizeof(int));
        if (is_safe(numbers_copy, len -1, tolerance - 1)) return 1;
      }

      return 0;
    }
  }

  return 1;
}

int main () {
  FILE *fd;
  char line[LINE_BUFFER_SIZE];
  unsigned int solution = 0;

  fd = fopen("input.txt", "r");
  if (fd <= 0) {
    printf("Missing input file\n");
    return -1;
  }

  while (fgets(line, LINE_BUFFER_SIZE, fd) != NULL) {
    int numbers[MAX_LINE_SIZE];
    unsigned char len = 0;
    char *token;

    token = strtok(line, " \t");

    while (token != NULL) {
      sscanf(token, "%d", &numbers[len]);
      len++;
      token = strtok(NULL, " \t");
    }

    int safe = is_safe(numbers, len, MAX_TOLERANCE);
    solution += safe;
  }

  printf("solution: %u\n", solution);

  fclose(fd);
  return 0;
}
