#include <stdio.h>
#include <string.h>

#define LINE_BUFFER_SIZE 1024
#define MAX_LINE_SIZE 10

int abs (int value) {
  return value >= 0 ? value : -value;
}

int is_safe (char line[LINE_BUFFER_SIZE]) {
  int numbers[MAX_LINE_SIZE];
  unsigned char len = 0;
  char *token;

  printf("%s", line);
  token = strtok(line, " \t");

  while (token != NULL) {
    sscanf(token, "%d", &numbers[len]);
    len++;
    token = strtok(NULL, " \t");
  }
  char desc;
  for (int i = 1; i < len; i++) {
    char diff = numbers[i] - numbers[i - 1];
    unsigned char abs_diff = abs(diff);
    char current_desc = diff < 0;

    if (i == 1) desc = current_desc;
    if (desc != current_desc || abs_diff == 0 || abs_diff > 3) return 0;
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
    solution += is_safe(line);
  }

  printf("solution: %u\n", solution);

  fclose(fd);
  return 0;
}
