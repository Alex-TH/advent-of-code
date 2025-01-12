#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "input.txt"

#define MAX_TOWEL_LEN 10
#define MAX_TOWEL_COUNT 450
#define MAX_PATTERN_LEN 70
#define START_CAPACITY 100

typedef struct Towel {
  char value[MAX_TOWEL_LEN];
  int len;
} Towel;

int parse_towels(FILE *fd, Towel towels[MAX_TOWEL_COUNT]) {
  int towel_count = 0;
  char current = '\0';

  while ((current = fgetc(fd)) != '\n') {
    if (current == ',') {
      towels[towel_count].value[towels[towel_count].len] = '\0';
      towel_count++;
      // consume empty space
      fgetc(fd);
    } else {
      towels[towel_count].value[towels[towel_count].len++] = current;
    }
  }

  towels[towel_count].value[towels[towel_count].len] = '\0';
  towel_count++;
  return towel_count;
}

int starts_with(char *str, char *substr) {
  int index = 0;
  while (substr[index] != '\0') {
    if (str[index] != substr[index] || str[index] == '\0') return 0;
    index++;
  }

  return 1;
}

long long int count_patterns(char pattern[MAX_PATTERN_LEN], short pattern_len, Towel towels[MAX_TOWEL_COUNT], int towel_count, int pattern_offset, long long int *cache) {
  if (pattern_offset == pattern_len) return 1;

  long long int count = 0;
  short remaining = pattern_len - pattern_offset;
  for (int i = 0; i < towel_count; i++) {
    Towel towel = towels[i];
    if (towel.len > remaining || !starts_with(pattern + pattern_offset, towel.value)) continue;

    if (cache[remaining - towel.len] > -1) {
      count += cache[remaining - towel.len];
    } else {
      long long int current_count = count_patterns(pattern, pattern_len, towels, towel_count, pattern_offset + towel.len, cache);
      cache[remaining - towel.len] = current_count;
      count += current_count;
    }
  }

  return count;
}

int main() {
  FILE *fd;
  Towel towels[MAX_TOWEL_COUNT] = {0};
  int towel_count;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  towel_count = parse_towels(fd, towels);
  printf("%d towels found:\n", towel_count);
  for (int i = 0; i < towel_count; i++) {
    printf("%s\n", towels[i].value);
  }

  char current_pattern[MAX_PATTERN_LEN];
  long long int solution = 0;
  while (fscanf(fd, "\n%s\n", current_pattern) != EOF) {
    short pattern_len = strlen(current_pattern);
    long long int cache[pattern_len + 1];
    // keep the value for when it doesn't find too
    memset(cache, -1, sizeof(cache));
    for (int i = 0; i < pattern_len; i++) printf("%lld ", cache[i]);
    printf("\n");

    printf("validating %s with len %d\n", current_pattern, pattern_len);
    long long int result = count_patterns(current_pattern, pattern_len, towels, towel_count, 0, cache);
    solution += result;
    printf("result %lld\n", result);
  }

  printf("solution: %lld\n", solution);

  fclose(fd);
  return 0;
}
