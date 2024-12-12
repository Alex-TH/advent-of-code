#include <stdio.h>

#define ARRAY_SIZE 1000
#define PSEUDO_HASH_SIZE 100000

int parse_input(int list_a[ARRAY_SIZE], unsigned char hash_b[PSEUDO_HASH_SIZE]) {
  FILE *fd;
  int value_a, value_b;
  int index = 0;

  fd = fopen("input.txt", "r");
  if (fd <= 0) {
    printf("Missing input file\n");
    return -1;
  }

  while (fscanf(fd, "%d %d\n", &value_a, &value_b) != EOF) {
    if (index == ARRAY_SIZE) {
      printf("We have more data than expected\n");
      return -1;
    }

    list_a[index] = value_a;
    hash_b[value_b] += 1;
    index++;
  }

  return 0;
}

int main () {
  int list_a[ARRAY_SIZE];
  unsigned char hash_b[PSEUDO_HASH_SIZE] = {0};

  int result = parse_input(list_a, hash_b);
  if (result < 0) return result;

  unsigned int solution = 0;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    unsigned int multiplication = (unsigned int) list_a[i] * (unsigned int) hash_b[list_a[i]];
    solution += multiplication;
  }

  printf("solution: %u\n", solution);

  return 0;
}
