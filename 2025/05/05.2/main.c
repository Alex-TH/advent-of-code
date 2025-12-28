#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "input.txt"
#define MAX_RANGES 1000

typedef unsigned long long int u64;
typedef long long int i64;

typedef struct Range {
  i64 start;
  i64 end;
} Range;

int parse_ranges(FILE *fd, Range ranges[MAX_RANGES]) {
  int count = 0;
  char line_buf[100];

  while(fgets(line_buf, 100, fd) != NULL && sscanf(line_buf, "%llu-%llu\n", &ranges[count].start, &ranges[count].end) == 2) {
    // if ()
    count++;
  }

  return count;
}

void print_ranges(Range ranges[MAX_RANGES], int count) {
  for (int i = 0; i < count; i++) {
    printf("%lld-%lld\n", ranges[i].start, ranges[i].end);
  }
}

short in_range(u64 value, Range range) {
  return range.start <= value && value <= range.end;
}

int minimize_ranges(Range ranges[MAX_RANGES], int count) {
  int new_count = count;

  for (int i = 1; i < new_count; i++) {
    Range current = ranges[i];
    int j = i - 1;
    printf("Comparing %lld-%lld to %lld-%lld\n", current.start, current.end, ranges[j].start, ranges[j].end);
    // for (int j = i - 1; j > -1; j--) {
      // range already included, remove it
      if (in_range(current.start, ranges[j]) && in_range(current.end, ranges[j])) {
        // printf("canibilizing\n");
        memmove(ranges + i, ranges + i + 1, sizeof(Range) * (new_count - i));
        i--;
        new_count--;
      }

      // printf("Included -> %d, End > %d\n", in_range(current.start, ranges[j]), current.end > ranges[j].end);
      if (in_range(current.start, ranges[j]) && current.end > ranges[j].end) {
        // printf("joining\n");
        ranges[j].end = current.end;
        memmove(ranges + i, ranges + i + 1, sizeof(Range) * (new_count - i));
        i--;
        new_count--;
      }
    // }
  }

  return new_count;
}

int comp_range(const void *range_a, const void *range_b) {
  i64 diff= ((Range *)range_a)->start - ((Range *)range_b)->start;

  if (diff > 0) return 1;
  if (diff < 0) return -1;

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

  int range_count = parse_ranges(fd, ranges);
  qsort(&ranges, range_count, sizeof(Range), comp_range);
  print_ranges(ranges, range_count);

  printf("Minimizing %d ranges\n", range_count);
  range_count = minimize_ranges(ranges, range_count);
  printf("Minimized to %d ranges\n", range_count);
  print_ranges(ranges, range_count);

  // printf("Minimizing ranges\n");
  // range_count = minimize_ranges(ranges, range_count);
  // printf("Minimized to %d ranges\n", range_count);
  // print_ranges(ranges, range_count);

  u64 solution = 0;
  for (int i = 0; i < range_count; i++) {
    solution += ranges[i].end - ranges[i].start + 1;
  }

  printf("solution %llu\n", solution);

  fclose(fd);
  return 0;
}
