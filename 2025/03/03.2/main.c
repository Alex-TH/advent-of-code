#include <stdio.h>

#define INPUT_FILE "input.txt"
#define STR_LEN 102
#define JOLTAGE_LEN 12

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

unsigned long long int pow_10(short exp) {
  unsigned long long int value = 1;
  for (short i = 1; i < exp; i++) {
    value *= 10;
  }

  return value;
}

unsigned long long int get_highest_2(char joltages[STR_LEN]) {
  short battery[JOLTAGE_LEN] = {0};
  short indexes[JOLTAGE_LEN] = {0};
  unsigned long long int value = 0;

  for (short i = 0; i < JOLTAGE_LEN; i++) {
    short start = i == 0 ? 0 : (indexes[i - 1] + 1);
    short delta = JOLTAGE_LEN - 1 - i;

    for (short j = start; joltages[j + delta] != '\n'; j++) {
      if (joltages[j] > battery[i]) {
        battery[i] = joltages[j];
        indexes[i] = j;
      }
    }

    printf("%c", battery[i]);
    value += pow_10(JOLTAGE_LEN - i) * (battery[i] - '0');
  }

  printf(" ");
  return value;
}

int main() {
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd <= 0) {
    perror("Missing input file");
    return -1;
  }

  char joltages[STR_LEN];
  unsigned long long int solution = 0;
  while (fgets(joltages, STR_LEN, fd) != NULL) {
    printf("%s", joltages);
    unsigned long long highest_joltage = get_highest_2(joltages);
    printf("%llu\n", highest_joltage);
    solution += highest_joltage;
  }

  printf("solution %llu\n", solution);
  fclose(fd);
  return 0;
}
