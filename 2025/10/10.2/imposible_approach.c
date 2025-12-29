#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "chunk.txt"
#define MAX_MASKS 20
#define MAX_QUEUE_LEN 1000000

#define HASH_CAPACITY 10000000
#define HASH_FACTOR 31
#define U16_MAX 4294967295

#define START_PRIORITY_LEN 10000

#define ON '#'
#define OFF '.'

typedef unsigned int u16;

u16 str_len(char *str);

/** HashMap implementation [char *] -> u16 **/
typedef struct Node {
  struct Node *next;
  char *key;
  u16 value;
} Node;
typedef struct HashMap {
  Node **values;
  u16 len;
  u16 capacity;
} HashMap;
typedef struct HashResult {
  u16 value;
  short found;
} HashResult;

HashMap init_hash() {
  HashMap map;
  map.capacity = HASH_CAPACITY;
  map.len = 0;
  map.values = (Node **)malloc(sizeof(Node *) * map.capacity);

  return map;
}

u16 hash(HashMap *map, char *key) {
  u16 key_len = str_len(key);
  u16 factor = HASH_FACTOR;
  u16 sum = 0;

  for (u16 i = 0; i < key_len; i++) {
    sum = (sum + (((u16)key[i] * factor) % map->capacity) % map->capacity) % map->capacity;
    factor = (factor * HASH_FACTOR) % U16_MAX;
  }

  return sum;
}

void insert_hash(HashMap *map, char *key, u16 value) {
  u16 index = hash(map, key);
  printf("Inserting %u with key %s in %u\n", value, key, index);
  Node *newValue = (Node *)malloc(sizeof(Node));
  newValue->key = key;
  newValue->next = NULL;
  newValue->value = value;

  if (map->values[index] == NULL) {
    printf("No collision detected, adding normally!\n");
    map->values[index] = newValue;
  } else {
    printf("Collision detected, adding or overriding!\n");
    Node *current = map->values[index];
    Node *prev = NULL;

    while (current != NULL) {
      if (strcmp(current->key, key) == 0) {
        if (prev == NULL) {
          map->values[index] = current->next;
          free(current);
          current = map->values[index];
        } else {
          prev->next = current->next;
          free(current);
          current = prev->next;
        }
        map->len--;
        continue;
      }

      prev = current;
      current = current->next;
    }
    newValue->next = map->values[index];
    map->values[index] = newValue;
  }
  map->len++;
}

HashResult get_hash(HashMap *map, char *key) {
  u16 index = hash(map, key);
  HashResult result = { .value = 0, .found = 0 };

  Node *current = map->values[index];
  while (current != NULL && strcmp(key, current->key) != 0) {
    current = current->next;
  }

  if (current == NULL) return result;
  result.value = current->value;
  result.found = 1;

  return result;
}

short delete_hash(HashMap *map, char *key) {
  u16 index = hash(map, key);

  Node *current = map->values[index];
  Node *prev = NULL;
  while (current != NULL && strcmp(key, current->key) != 0) {
    prev = current;
    current = current->next;
  }

  if (current == NULL) return 0;

  if (prev == NULL) {
    map->values[index] = current->next;
  } else {
    prev->next = current->next;
  }
  map->len--;

  free(current);
  return 1;
}

/** End HashMap **/

/** Priority queue **/

typedef struct PriorityValue {
  int priority;
  u16 joltages[10];
  short joltages_len;
} PriorityValue;
typedef struct PriorityQueue {
  PriorityValue *values;
  u16 len;
  u16 _capacity;
} PriorityQueue;

PriorityQueue init_pqueue() {
  PriorityQueue queue;
  queue.values = (PriorityValue *) malloc(sizeof(PriorityValue) * START_PRIORITY_LEN);
  queue._capacity = START_PRIORITY_LEN;

  return queue;
}

void heapify_up(PriorityQueue *queue, u16 index) {
  u16 parent = (index - 1) / 2;
  u16 current = index;

  while (current > 0 && queue->values[current].priority < queue->values[parent].priority) {
    PriorityValue temp = queue->values[current];
    queue->values[index] = queue->values[parent];
    queue->values[parent] = temp;

    current = parent;
    parent = (current - 1) / 2;
  }
}

void heapify_down(PriorityQueue *queue, u16 index) {
  if (index >= queue->len) return;

  u16 left_index = index * 2 + 1;
  u16 right_index = index * 2 + 2;
  u16 lowest = index;

  if (left_index < queue->len && queue->values[left_index].priority < queue->values[lowest].priority) {
    lowest = left_index;
  }

  if (right_index < queue->len && queue->values[right_index].priority < queue->values[lowest].priority) {
    lowest = right_index;
  }

  if (lowest != index) {
    PriorityValue temp = queue->values[index];
    queue->values[index] = queue->values[lowest];
    queue->values[lowest] = temp;

    heapify_down(queue, lowest);
  }
}

void insert_queue(PriorityQueue *queue, PriorityValue value) {
  if (queue->len + 1 == queue->_capacity) {
    queue->_capacity *= 2;
    queue->values = realloc(queue->values, sizeof(PriorityValue) * queue->_capacity);
  }

  queue->values[queue->len++] = value;
  heapify_up(queue, queue->len - 1);
}

PriorityValue dequeue(PriorityQueue *queue) {
  PriorityValue value = queue->values[0];
  queue->values[0] = queue->values[queue->len - 1];
  queue->len--;
  heapify_down(queue, 0);

  return value;
}

/** End Priority queue **/

typedef struct Mask {
  short values[10];
  short len;
} Mask;

typedef struct Schema {
  // u16 diagram;
  Mask masks[MAX_MASKS];
  u16 joltages[10];
  short diagram_len;
  short masks_len;
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
  schema->masks_len = 0;
  str += offset;

  while (sscanf(str, " (%[01234567689,])%n", chunk, &offset) > 0) {
    u16 chunk_len = str_len(chunk);
    short mask_len = 0;

    for (u16 i = 0; i < chunk_len; i++) {
      if (is_number(chunk[i])) {
        schema->masks[schema->masks_len].values[mask_len] = chunk[i] - '0';
        mask_len++;
      }
    }

    // printf("button -> %s -> %010b\n", chunk, schema->masks[schema->masks_len]);
    schema->masks[schema->masks_len].len = mask_len;
    schema->masks_len++;
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
  for (short i = 0; i < schema.masks_len; i++) {
    printf("(");
    for (short j = 0; j < schema.masks[i].len; j++) {
      if (j > 0) printf(", ");
      printf("%d", schema.masks[i].values[j]);
    }
    printf(") ");
  }

  printf("joltages n=%u {", schema.diagram_len);
  for (short i = 0; i < schema.diagram_len; i++) {
    if (i > 0) printf(", ");
    printf("%u", schema.joltages[i]);
  }

  printf("}\n");
}

char *get_key(u16 joltages[10], short len) {
  char *key = (char *) malloc(sizeof(char) * 50);
  char *str = key;

  sprintf(str, "{");
  str++;
  for (short i = 0; i < len; i++) {
    if (i > 0) {
      sprintf(str, ",");
      str++;
    }
    int offset;
    sprintf(str, "%u%n", joltages[i], &offset);
    str += offset;
  }
  sprintf(str, "}");

  return key;
}

short schema_cmp(Schema schema, u16 joltages[10]) {
  short result = 0;
  printf("Comparing %s vs %s\n", get_key(schema.joltages, schema.diagram_len), get_key(joltages, schema.diagram_len));
  for (short i = 0; i < schema.diagram_len; i++) {
    int diff = schema.joltages[i] - joltages[i];
    if (diff < 0) return -1;
    if (diff > 0) result = 1;
  }

  return result;
}

u16 get_min_seq(Schema schema) {
  HashMap costs = init_hash();
  PriorityQueue queue = init_pqueue();
  PriorityValue initial = { .priority = 0, .joltages = {0}, .joltages_len = schema.diagram_len };

  insert_hash(&costs, get_key(initial.joltages, initial.joltages_len), initial.priority);
  insert_queue(&queue, initial);

  u16 result = 0;
  print_schema(schema);
  u16 iterations = 0;

  while (queue.len > 0/* && iterations < 10000*/) {
    iterations++;
    PriorityValue current = dequeue(&queue);
    short comparison = schema_cmp(schema, current.joltages);
    printf("Current priority %u joltages -> %s, comparison -> %d\n", current.priority, get_key(current.joltages, current.joltages_len), comparison);

    if (comparison == 0) {
      result = current.priority;
      break;
    }
    if (comparison < 0) continue;

    u16 next_cost = current.priority + 1;
    for (u16 i = 0; i < schema.masks_len; i++) {
      PriorityValue value = { .priority = next_cost, .joltages_len = schema.diagram_len };
      Mask current_mask = schema.masks[i];

      memcpy(value.joltages, current.joltages, sizeof(schema.joltages));
      for (short j = 0; j < current_mask.len; j++) {
        value.joltages[current_mask.values[j]]++;
      }

      char *key = get_key(value.joltages, value.joltages_len);
      HashResult current_cost = get_hash(&costs, key);

      if (!current_cost.found || next_cost <= current_cost.value) {
        insert_queue(&queue, value);
      }
    }
  }

  // perror("We shouldn't arrive here!\n");
  free(queue.values);
  free(costs.values);
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
    // print_schema(current);
    u16 value = get_min_seq(current);
    printf("current value -> %u\n", value);

    // index++;
    solution += value;
  }

  printf("Solution %u\n", solution);

  fclose(fd);
  return 0;
}
