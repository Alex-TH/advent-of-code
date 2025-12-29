#include <stdio.h>

#define INPUT_FILE "input.txt"
#define MAX_MASKS 20
#define MAX_QUEUE_LEN 1000000

#define ON '#'
#define OFF '.'

typedef unsigned int u16;
typedef struct Schema {
  u16 diagram;
  u16 masks[MAX_MASKS];
  short diagram_len;
  short masks_len;
} Schema;
typedef struct Machine {
  u16 lights;
  u16 seq_len;
} Machine;

u16 str_len(char *str) {
  u16 len = 0;
  while (str[len] != '\0') {
    len++;
  }

  return len;
}

short is_number(char value) {
  return value >= '0' && '9' >= value;
}

void parse_schema(Schema *schema, char *line) {
  char chunk[500];
  int offset = 0;
  char *str = line;

  printf("line -> %s\n", line);
  sscanf(str, "[%[.#]]%n", chunk, &offset);
  schema->diagram_len = str_len(chunk);
  schema->masks_len = 0;
  str += offset;

  for (u16 i = 0; i < schema->diagram_len; i++) {
    schema->diagram <<= 1;
    if (chunk[i] == ON) {
      schema->diagram |= 1;
    }
  }

  printf("Sequence -> %s | %010b\n", chunk, schema->diagram);

  while (sscanf(str, " (%[01234567689,])%n", chunk, &offset) > 0) {
    u16 chunk_len = str_len(chunk);
    schema->masks[schema->masks_len] = 0;

    for (u16 i = 0; i < chunk_len; i++) {
      if (is_number(chunk[i])) {
        schema->masks[schema->masks_len] |= 1 << (schema->diagram_len - 1 - chunk[i] + '0');
      }
    }
    printf("button -> %s -> %010b\n", chunk, schema->masks[schema->masks_len]);
    schema->masks_len++;
    str += offset;
  }

  // skiping joltage parsing
}

u16 get_min_seq(Schema schema) {
  Machine queue[MAX_QUEUE_LEN];
  u16 queue_start = 0;
  u16 queue_end = 1;
  u16 queue_len = 1;
  queue[0].lights = 0;
  queue[0].seq_len = 0;

  short visited[1024];
  for (int i = 0; i < 1024; i++) visited[i] = 200;
  printf("schema  %10b with %u masks\n", schema.diagram, schema.masks_len);
  while (queue_len > 0) {
    Machine current = queue[queue_start];
    printf("[%u-%u] current seq -> %010b, %u steps\n", queue_start, queue_end, current.lights, current.seq_len);
    queue_len--;
    queue_start = (queue_start + 1) % MAX_QUEUE_LEN;

    if (visited[current.lights] <= current.seq_len + 1) {
      printf("already visited that path\n");
      continue;
    }
    if (current.lights == schema.diagram) return current.seq_len;
    visited[current.lights] = current.seq_len + 1;

    for (u16 i = 0; i < schema.masks_len; i++) {
      int next_end = (queue_end + 1) % MAX_QUEUE_LEN;
      queue[queue_end].lights = current.lights ^ schema.masks[i];
      queue[queue_end].seq_len = current.seq_len + 1;
      queue_end = next_end;
      queue_len++;
    }
  }

  perror("We shouldn't arrive here!\n");
  return 0;
}

int main() {
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  char line[1024];
  u16 solution = 0;
  short index = 0;
  while (fgets(line, sizeof(line), fd) != NULL) {
    Schema current = { .diagram = 0 };
    parse_schema(&current, line);
    u16 value = get_min_seq(current);
    printf("current value -> %u\n", value);

    index++;
    solution += value;
  }

  printf("Solution %u\n", solution);

  fclose(fd);
  return 0;
}
