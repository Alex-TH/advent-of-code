#include <stdio.h>

#define INPUT_FILE "chunk.txt"
#define MAX_BUTTONS 20
#define MAX_JOLTAGES 10

#define ON '#'
#define OFF '.'

typedef unsigned int u16;

u16 str_len(char *str);

typedef struct Matrix {
  u16 values[MAX_JOLTAGES][MAX_BUTTONS + 1];
  short m;
  short n;
} Matrix;
typedef struct Button {
  short values[10];
  short len;
} Button;

typedef struct Schema {
  Button buttons[MAX_BUTTONS];
  u16 joltages[10];
  short diagram_len;
  short buttons_len;
} Schema;

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
  schema->buttons_len = 0;
  str += offset;

  while (sscanf(str, " (%[01234567689,])%n", chunk, &offset) > 0) {
    u16 chunk_len = str_len(chunk);
    short button_len = 0;

    for (u16 i = 0; i < chunk_len; i++) {
      if (is_number(chunk[i])) {
        schema->buttons[schema->buttons_len].values[button_len] = chunk[i] - '0';
        button_len++;
      }
    }

    // printf("button -> %s -> %010b\n", chunk, schema->button[schema->buttons_len]);
    schema->buttons[schema->buttons_len].len = button_len;
    schema->buttons_len++;
    str += offset;
  }
  sscanf(str, " {%[0123456789,]}", chunk);
  str = chunk;

  while (sscanf(str, "%u,%n", &schema->joltages[schema->diagram_len], &offset) > 0) {
    schema->diagram_len++;
    str += offset;
  }
}

void print_schema(Schema schema) {
  for (short i = 0; i < schema.buttons_len; i++) {
    printf("(");
    for (short j = 0; j < schema.buttons[i].len; j++) {
      if (j > 0) printf(", ");
      printf("%d", schema.buttons[i].values[j]);
    }
    printf(") ");
  }

  printf("joltages n=%u {", schema.diagram_len);
  for (short i = 0; i < schema.diagram_len; i++) {
    if (i > 0) printf(", ");
    printf("%u", schema.joltages[i]);
  }

  printf("}\n");

  printf("Matrix of %u X %u: \n", schema.diagram_len, schema.buttons_len);
  Matrix matrix = { .m = schema.diagram_len, .n = schema.buttons_len + 1 };
  for (short i = 0; i < schema.buttons_len; i++) {
    for (short j = 0; j < schema.buttons[i].len; j++) {
      matrix.values[schema.buttons[i].values[j]][i] = 1;
    }
  }
  for (short i = 0; i < schema.diagram_len; i++) {
    matrix.values[i][schema.buttons_len] = schema.joltages[i];
  }

  printf("[\n");
  for (short j = 0; j < matrix.m; j++) {
    printf("  ");
    for (short i = 0; i < matrix.n; i++) {
      if (i > 0) printf(", ");
      printf("%u", matrix.values[j][i]);
    }
    printf("\n");
  }
  printf("]\n");
}

u16 get_min_seq(Schema schema) {
  u16 result = 0;
  return result;
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
  // short index = 0;
  while (fgets(line, sizeof(line), fd) != NULL) {
    Schema current = {0};
    parse_schema(&current, line);
    print_schema(current);
    u16 value = get_min_seq(current);
    printf("current value -> %u\n", value);

    // index++;
    solution += value;
  }

  printf("Solution %u\n", solution);

  fclose(fd);
  return 0;
}
