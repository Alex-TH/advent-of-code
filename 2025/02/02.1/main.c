#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"

typedef struct ArrayList {
  unsigned long int *values;
  unsigned int len;
  unsigned int capacity;
} ArrayList;

void initialize_array_list(ArrayList **array_list) {
  *array_list = malloc(sizeof(ArrayList));
  (*array_list)->len = 0;
  (*array_list)->capacity = 10;
  (*array_list)->values = malloc(sizeof(unsigned long int) * 10);
}

void append(ArrayList *array_list, unsigned long int value) {
  if (array_list == NULL) initialize_array_list(&array_list);

  unsigned int len = array_list->len;
  unsigned int next_len = len + 1;

  if (next_len == array_list->capacity) {
    array_list->capacity *= 2;
    array_list->values = realloc(array_list->values, sizeof(unsigned long int) * array_list->capacity);
  }

  array_list->values[len] = value;
  array_list->len = next_len;
}

short number_len(char number[20]) {
  short len = 0;
  while (number[len] != '\0') len++;

  return len;
}

int is_invalid(unsigned long int value) {
  char number[20];
  sprintf(number, "%lu", value);
  short len = number_len(number);
  short middle = len / 2;

  // printf("%s -> testing\n", number);

  if (len % 2 != 0) return 0;

  for (short i = 0; i < middle; i++) {
    if (number[i] != number[middle + i]) return 0;
  }

  // printf("%s -> invalid\n", number);
  return 1;
}

void count_invalid(unsigned long int start, unsigned long int end, ArrayList *valids) {
  for (unsigned long int i = start; i <= end; i++) {
    if (is_invalid(i)) {
      append(valids, i);
    }
  }
}

int main() {
  FILE *fd;
  ArrayList *invalids;

  fd = fopen(INPUT_FILE, "r");
  if (fd <= 0) {
    perror("Missing input file");
    return -1;
  }

  initialize_array_list(&invalids);
  unsigned long int start, end;
  while (fscanf(fd, "%lu-%lu,", &start, &end) != EOF) {
    printf("%lu-%lu\n", start, end);
    count_invalid(start, end, invalids);
  }

  unsigned long int solution = 0;
  for (unsigned int i = 0; i < invalids->len; i++) {
    // printf("invalid -> %lu\n", invalids->values[i]);
    solution += invalids->values[i];
  }

  printf("solution %lu \n", solution);

  free(invalids);
  fclose(fd);
  return 0;
}
