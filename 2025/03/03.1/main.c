#include <stdio.h>

#define INPUT_FILE "input.txt"
#define STR_LEN 102

short get_highest(char joltages[STR_LEN]) {
  short index = 0;
  short decimal_index = 0;
  short decimal = 0;
  short unit = 0;

  while (joltages[index + 1] != '\n') {
    if (joltages[index] > decimal) {
      decimal = joltages[index];
      decimal_index = index;
    }
    index++;
  }
  index = decimal_index + 1;

  while (joltages[index] != '\n') {
    if (joltages[index] > unit) {
      unit = joltages[index];
    }

    index++;
  }

  return ((decimal - '0') * 10) + (unit - '0');
}

int main() {
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd <= 0) {
    perror("Missing input file");
    return -1;
  }

  char joltages[STR_LEN];
  unsigned int solution = 0;
  while (fgets(joltages, STR_LEN, fd) != NULL) {
    printf("%s", joltages);
    short highest_joltage = get_highest(joltages);
    printf("%d\n", highest_joltage);
    solution += highest_joltage;
  }

  printf("solution %u\n", solution);
  fclose(fd);
  return 0;
}
