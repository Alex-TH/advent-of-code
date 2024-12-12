#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 1000

void insert_ordered(int value, int list[ARRAY_SIZE], int current_len) {
  if (current_len == 0) list[current_len] = value;

  int start = 0;
  int end = current_len;

  while (start < end) {
    int chunk_size = end - start;
    int half = start + (chunk_size / 2);

    if (value > list[half]) {
      start = half + 1;
    }
    if (value < list[half]) {
      end = half - 1;
    }
    if (value == list[half]) {
      start = half;
      end = half;
    }
  }


  if (start >= current_len) list[current_len] = value;
  else {
    int index = start;
    index += list[index] < value ? 1 : 0;
    // Note: move extra chunk to the right
    memmove(list + index + 1, list + index, (current_len - index) * sizeof(int));
    list[index] = value;
  }
}

int parse_input(int list_a[ARRAY_SIZE], int list_b[ARRAY_SIZE]) {
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

    insert_ordered(value_a, list_a, index);
    insert_ordered(value_b, list_b, index);
    index++;
  }

  return 0;
}

int abs (int value) {
  return value >= 0 ? value : -value;
}

int main () {
  int list_a[ARRAY_SIZE], list_b[ARRAY_SIZE];

  int result = parse_input(list_a, list_b);
  if (result < 0) return result;

  int prev_a = 0, prev_b = 0;
  int solution = 0;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (prev_a > list_a[i] || prev_b > list_b[i]) {
      printf("mal ordenados: prev_a: %d <= list_a: %d prev_b: %d <= list_b: %d\n", prev_a, list_a[i], prev_b, list_b[i]);
      return -1;
    }

    prev_a = list_a[i];
    prev_b = list_b[i];
    solution += abs(list_a[i] - list_b[i]);
  }

  printf("solution: %d\n", solution);

  return 0;
}
