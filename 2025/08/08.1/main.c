#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"

#define MAX_BOXES 1000
#define MAX_DISTANCE_LEN 499500
#define ITERATIONS 1000

typedef long int int32;
typedef unsigned int u16;
typedef unsigned long long int u64;

typedef struct Position {
  int32 x;
  int32 y;
  int32 z;
  int index;
} Position;

typedef struct Distance {
  Position from;
  Position to;
  int32 value;
} Distance;

Distance calc_distance(Position from, Position to) {
  double x_delta = pow(from.x - to.x, 2);
  double y_delta = pow(from.y - to.y, 2);
  double z_delta = pow(from.z - to.z, 2);

  Distance result = {
    .from = from,
    .to = to,
    .value = sqrt(x_delta + y_delta + z_delta),
  };

  return result;
}

int parse_boxes(FILE *fd, Position boxes[MAX_BOXES]) {
  int len = 0;
  while (fscanf(fd, "%lu,%lu,%lu\n", &boxes[len].x, &boxes[len].y, &boxes[len].z) != EOF) {
    boxes[len].index = len;
    len++;
  }

  return len;
}

void print_boxes(Position boxes[MAX_BOXES], int len) {
  for (int i = 0; i < len; i++) {
    printf("boxes[%d] = {%lu,%lu,%lu}\n", boxes[i].index, boxes[i].x, boxes[i].y, boxes[i].z);
  }
}

int comp_distance(const void *distance_a, const void *distance_b) {
  int32 diff = ((Distance *)distance_a)->value - ((Distance *)distance_b)->value;

  if (diff > 0) return 1;
  if (diff < 0) return -1;

  return 0;
}

int comp_circuit(const void * a, const void *b) {
  return *(int *)(b) - *(int *)(a);
}

int main() {
  FILE *fd;
  Position boxes[MAX_BOXES];
  Distance *distances;

  short circuit_map[MAX_BOXES];
  int circuits[MAX_BOXES] = {0};
  int circuits_len = 0;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }
  distances = malloc(sizeof(Distance) * MAX_DISTANCE_LEN);
  memset(circuit_map, -1, sizeof(circuit_map));

  int boxes_len = parse_boxes(fd, boxes);
  print_boxes(boxes, boxes_len);

  int32 distances_len = 0;
  for (int i = 0; i < boxes_len - 1; i++) {
    for (int j = i + 1; j < boxes_len; j++) {
      distances[distances_len] = calc_distance(boxes[i], boxes[j]);
      distances_len++;
    }
  }

  qsort(distances, distances_len, sizeof(Distance), comp_distance);
  printf("Number of distances %ld!\n", distances_len);

  for (int i = 0; i < ITERATIONS; i++) {
    Distance current = distances[i];
    printf("Current -> {%lu,%lu,%lu} - {%lu,%lu,%lu} = %lu\n", current.from.x, current.from.y, current.from.z, current.to.x, current.to.y, current.to.z, current.value);
    int from_circuit = circuit_map[current.from.index];
    int to_circuit = circuit_map[current.to.index];
    printf("From -> %d, to -> %d\n", from_circuit, to_circuit);

    if (from_circuit == -1 && to_circuit == -1) {
      circuit_map[current.from.index] = circuits_len;
      circuit_map[current.to.index] = circuits_len;
      circuits[circuits_len] += 2;
      circuits_len++;
    } else if (from_circuit != -1 && to_circuit == -1) {
      circuit_map[current.to.index] = from_circuit;
      circuits[from_circuit]++;
    } else if (from_circuit == -1 && to_circuit != -1) {
      circuit_map[current.from.index] = to_circuit;
      circuits[to_circuit]++;
    } else if (from_circuit != to_circuit) {
      int min_index = from_circuit > to_circuit ? to_circuit : from_circuit;
      int max_index = from_circuit > to_circuit ? from_circuit : to_circuit;
      printf("%d > %d\n", max_index, min_index);

      circuits[min_index] += circuits[max_index];
      circuits[max_index] = 0;

      for(int j = 0; j < MAX_BOXES; j++) {
        if (circuit_map[j] == max_index) {
          circuit_map[j] = min_index;
        }

        if (circuit_map[j] > max_index) {
          circuit_map[j]--;
        }
      }

      for (int x = 0; x < MAX_BOXES; x++) printf("%d ", circuits[x]);
      printf("\n");
      // for (int x = 0; x < MAX_BOXES; x++) printf("%d ", circuit_map[x]);
      // printf("\n");
      memmove(circuits + max_index, circuits + max_index + 1, sizeof(int) * (MAX_BOXES - max_index - 1));
      // for (int x = 0; x < MAX_BOXES; x++) printf("%d ", circuit_map[x]);
      // printf("\n");
      for (int x = 0; x < MAX_BOXES; x++) printf("%d ", circuits[x]);
      printf("\n");
      circuits_len--;
    }
  }

  printf("Before sorting\n");
  for(int i = 0; i < circuits_len; i++) {
    printf("circuit[%d] = %d\n", i, circuits[i]);
  }
  qsort(circuits, circuits_len, sizeof(int), comp_circuit);

  printf("Number of circuits = %d\n", circuits_len);
  u64 solution = 1;
  short count = 0;
  int prev = 0;
  for(int i = 0; i < circuits_len && count < 3; i++) {
    printf("circuit[%d] = %d\n", i, circuits[i]);
    if (circuits[i] != prev) {
      printf("current solution = %llu\n", solution);
      solution *= circuits[i];
      count++;
    }

    prev = circuits[i];
  }

  printf("Solution %llu\n", solution);

  fclose(fd);
  free(distances);
  return 0;
}
