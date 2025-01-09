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
typedef struct Value {
  Towel towel;
  int offset;
} Value;
typedef struct {
  Value *values;
  int len;
  int capacity;
} List;

List initialize_list() {
  List list;
  list.len = 0;
  list.capacity = START_CAPACITY;
  list.values = calloc(list.capacity, sizeof(Value));

  return list;
}

void append(List *list, Value value) {
  if (list->len == list->capacity) {
    list->capacity *= 2;
    list->values = realloc(list->values, list->capacity * sizeof(Value));
  }

  list->values[list->len++] = value;
}

Value dequeue(List *list) {
  Value value = list->values[0];

  list->len--;
  memmove(list->values, list->values + 1, sizeof(Value) * list->len);

  return value;
}

Value pop(List *list) {
  Value value = list->values[--list->len];

  return value;
}

void free_list(List *list) {
  if (list->values != NULL) free(list->values);
}

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

int validate_pattern(char pattern[MAX_PATTERN_LEN], Towel towels[MAX_TOWEL_COUNT], int towel_count) {
  short pattern_len = strlen(pattern);
  List stack = initialize_list();
  List queue = initialize_list();

  short front_cache[towel_count];
  short back_cache[towel_count];
  int result = 0;

  memset(front_cache, 0, sizeof(front_cache));
  memset(back_cache, 0, sizeof(back_cache));
  printf("validating: %s with len %hd\n", pattern, pattern_len);
  for (int i = 0; i < towel_count; i++) {
    if (starts_with(pattern, towels[i].value)) {
      Value value;
      value.towel = towels[i];
      value.offset = towels[i].len;
      append(&stack, value);
      front_cache[towels[i].len] = 1;
    }

    if (starts_with(pattern + pattern_len - towels[i].len, towels[i].value)) {
      Value value;
      value.towel = towels[i];
      value.offset = towels[i].len;
      append(&queue, value);
    }
  }

  while (stack.len > 0 && queue.len > 0) {
    printf("Stack len %d\n", stack.len);
    printf("Queue len %d\n", queue.len);
    Value front_current = pop(&stack);
    Value back_current = dequeue(&queue);

    printf("current front offset %d pattern len %d\n", front_current.offset, pattern_len);
    printf("current back offset %d pattern len %d\n", back_current.offset, pattern_len);

    if (front_current.offset == pattern_len) {
      result = 1;
      break;
    }

    if (back_cache[pattern_len - front_current.offset] || front_cache[pattern_len - back_current.offset]) {
      printf("thanks to cache!\n");
      result = 1;
      break;
    }

    for (int i = 0; i < towel_count; i++) {
      Towel towel = towels[i];
      int next_offset = front_current.offset + towel.len;

      if (next_offset <= pattern_len && starts_with(pattern + front_current.offset, towel.value)) {
        printf("%s start with %s\n", pattern + front_current.offset, towel.value);
        Value next;
        next.towel = towel;
        next.offset = next_offset;
        append(&stack, next);
        front_cache[next_offset] = 1;
      }

      next_offset = back_current.offset + towel.len;
      if (next_offset <= pattern_len && starts_with(pattern + pattern_len - next_offset, towel.value)) {
        printf("%s start with %s\n", pattern + pattern_len - back_current.offset, towel.value);
        Value next;
        next.towel = towel;
        next.offset = next_offset;
        append(&queue, next);
        back_cache[next_offset] = 1;
      }
    }
    /* char wait; */
    /* scanf("%c", &wait); */
  }

  free_list(&stack);
  free_list(&queue);
  return result;
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
  int solution = 0;
  while (fscanf(fd, "\n%s\n", current_pattern) != EOF) {
    solution += validate_pattern(current_pattern, towels, towel_count);
  }

  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
