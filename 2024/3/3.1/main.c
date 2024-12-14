#include <stdio.h>

int main() {
  FILE *fd;
  const char *pattern = "mul";

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("No file found");
    return -1;
  }

  unsigned char pattern_index = 0;
  unsigned char pattern_len = 3;
  char current_char;
  int solution = 0;

  while ((current_char = fgetc(fd)) != EOF) {
    if (current_char != pattern[pattern_index]) {
      pattern_index = 0;
      continue;
    }
    pattern_index++;
    if (pattern_index < pattern_len) continue;

    int a, b;
    int scan_result = fscanf(fd, "(%d,%d", &a, &b);
    int is_closing = fgetc(fd) == ')';

    solution += scan_result == 2 && is_closing ? a * b : 0;
    pattern_index = 0;
  }

  printf("solution: %d\n", solution);
  fclose(fd);

  return 0;
}
