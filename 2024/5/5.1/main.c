#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RULES_ARRAY 100
#define LINE_BUFFER_SIZE 1024

typedef struct Rule {
  unsigned char value;
  struct Rule *next;
} Rule;

Rule* get_last(Rule *rule) {
  if (rule == NULL) return rule;

  Rule *current_pos;
  current_pos = rule;
  while (current_pos->next != NULL) {
    current_pos = current_pos->next;
  }

  return current_pos;
}

Rule* create_rule(unsigned char value) {
  Rule *rule;
  rule = (Rule *) malloc(sizeof(Rule));
  rule->value = value;

  return rule;
}

void build_rules(FILE *fd, Rule asc_rules[RULES_ARRAY]) {
  unsigned char a, b;
  char line[LINE_BUFFER_SIZE];

  while (fgets(line, LINE_BUFFER_SIZE, fd) && sscanf(line, "%hhu|%hhu", &a, &b) == 2) {
    if (asc_rules[a].value == 0) {
      asc_rules[a].value = b;
    } else {
      Rule *position = get_last(&asc_rules[a]);
      position->next = create_rule(b);
    }
  }
}

int validate_line(char line[LINE_BUFFER_SIZE], Rule rules[RULES_ARRAY]) {
  unsigned char numbers[RULES_ARRAY] = {0};
  unsigned char count[RULES_ARRAY] = {0};
  unsigned char len = 0;
  char *token;

  token = strtok(line, ",");

  while (token != NULL) {
    sscanf(token, "%hhu", &numbers[len]);

    token = strtok(NULL, ",");
    len++;
  }

  for (int i = 0; i < len; i++) {
    Rule *current_pos = &rules[numbers[i]];
    count[numbers[i]]++;
    while (current_pos != NULL) {
      if (count[current_pos->value] > 0) {
        return 0;
      }
      current_pos = current_pos->next;
    }
  }

  return numbers[len / 2];
}

int main() {
  FILE *fd;
  char line[LINE_BUFFER_SIZE];
  Rule asc_rules[RULES_ARRAY] = {{ 0, NULL }};

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  build_rules(fd, asc_rules);

  for (int i = 0; i < RULES_ARRAY; i++) {
    if (asc_rules[i].value == 0) continue;

    Rule *current_pos = &asc_rules[i];
    while (current_pos != NULL) {
      current_pos = current_pos->next;
    }
  }

  /* fgetc(fd); */

  unsigned int solution = 0;
  while (fgets(line, LINE_BUFFER_SIZE, fd) !=NULL) {
    solution += validate_line(line, asc_rules);
  }

  printf("solution: %d\n", solution);

  fclose(fd);
  return 0;
}
