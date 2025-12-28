#include <stdio.h>
// #include <stdlib.h>

// #define ULL_SIZE 64
// #define BITMAP_SIZE 288230376151711740

#define INPUT_FILE "input.txt"
#define MAX_RANGES 1000

typedef unsigned long long int u64;

typedef struct Range {
  u64 start;
  u64 end;
} Range;

// typedef struct Bitmap {
//   unsigned long long _values[BITMAP_SIZE];
// } Bitmap;

// short bitmap_get(Bitmap *b, unsigned long long int index) {
//   b->_values[index / ULL_SIZE] |= 1 << (index % ULL_SIZE);
// }

// void bitmap_set(Bitmap *b, unsigned long long int index) {

// }

int parse_ranges(FILE *fd, Range ranges[MAX_RANGES]) {
  int count = 0;
  char line_buf[100];

  while(fgets(line_buf, 100, fd) != NULL && sscanf(line_buf, "%llu-%llu\n", &ranges[count].start, &ranges[count].end) == 2) {
    count++;
  }

  return count;
}

void print_ranges(Range ranges[MAX_RANGES], int count) {
  for (int i = 0; i < count; i++) {
    printf("%llu-%llu\n", ranges[i].start, ranges[i].end);
  }
}

short in_range(u64 value, Range ranges[MAX_RANGES], int range_count) {
  for (int i = 0; i < range_count; i++) {
    if (ranges[i].start <= value && value <= ranges[i].end) {
      printf("found range %llu-%llu\n", ranges[i].start, ranges[i].end);
      return 1;
    }
  }

  return 0;
}

int main() {
  FILE *fd;
  Range ranges[MAX_RANGES];

  fd = fopen(INPUT_FILE, "r");

  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  // b = calloc(1, sizeof(b));
  // if (b == NULL) printf("error\n");
  // printf("%lu\n", sizeof(b));
  int range_count = parse_ranges(fd, ranges);
  print_ranges(ranges, range_count);

  u64 current_value;
  int solution = 0;
  while(fscanf(fd, "%llu\n", &current_value) != EOF) {
    short fresh = in_range(current_value, ranges, range_count);
    printf("%llu -> fresh: %d\n", current_value, fresh);
    solution += fresh;
  }

  printf("solution %d\n", solution);

  // free(b);
  fclose(fd);
  return 0;
}
