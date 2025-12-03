#include <stdio.h>
#include <stdlib.h>

/* #define BOARD_WIDTH 12 */
/* #define BOARD_HEIGHT 12 */

#define BOARD_WIDTH 50
#define BOARD_HEIGHT 50

// optimize later?
/* #define ANTHENA_HASH_LEN 64 */
#define ANTHENA_HASH_LEN 255

typedef struct Anthena {
  char value;
  short int x;
  short int y;
} Anthena;

typedef Anthena Distance;
typedef Anthena Antinode;

typedef struct ArrayList {
  void *values;
  unsigned int len;
  unsigned int capacity;
}ArrayList;

void initialize_array_list(ArrayList **array_list, unsigned short int data_size) {
  *array_list = malloc(sizeof(ArrayList));
  (*array_list)->len = 0;
  (*array_list)->capacity = 10;
  (*array_list)->values = malloc(data_size * 10);
}

void append(ArrayList **array_list, Anthena value) {
  if (*array_list == NULL) initialize_array_list(&*array_list, sizeof(Anthena));

  unsigned int len = (*array_list)->len;
  unsigned int next_len = len + 1;
  Anthena *values = (*array_list)->values;

  if (next_len == (*array_list)->capacity) {
    (*array_list)->capacity *= 2;
    unsigned int capacity = (*array_list)->capacity;
    (*array_list)->values = realloc(values, sizeof(Anthena) * capacity);
  }

  values[len] = value;
  (*array_list)->len = next_len;
}

void print_array(ArrayList array) {
  Anthena *values = (Anthena *) array.values;
  printf("Array len: %u\n", array.len);

  for (int i = 0; i < array.len; i++) {
    if (i == 0) printf("%c: ", values[i].value);
    printf("{ %d, %d }, ", values[i].x, values[i].y);
  }
  printf("\n");
}

int abs(int value) {
  return value > 0 ? value : -value;
}

Distance abs_distance(Anthena a, Anthena b) {
  Distance distance;
  distance.value = a.value;
  distance.x = b.x - a.x;
  distance.y = b.y - a.y;

  return distance;
}

void build_board(FILE *fd, char board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      board[y][x] = fgetc(fd);
    }

    fgetc(fd);
  }
}

void print_board(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      printf("%c", board[y][x]);
    }
    printf("\n");
  }
}

int is_anthena(char value) {
  return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') || (value >= '0' && value <= '9');
}

void get_anthenas(char board[BOARD_HEIGHT][BOARD_WIDTH], ArrayList *anthena_lists[ANTHENA_HASH_LEN]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      short int value = board[y][x];
      if (!is_anthena(value)) continue;
      if (anthena_lists[value] == NULL) {
        initialize_array_list(&anthena_lists[value], sizeof(Anthena));
      } 

      Anthena anthena;
      anthena.value = value;
      anthena.x = x;
      anthena.y = y;

      append(&anthena_lists[value], anthena);
    }
  }
}

void get_distances(ArrayList *anthena_lists[ANTHENA_HASH_LEN], ArrayList *distances[ANTHENA_HASH_LEN]) {
  for (int i = 0; i < ANTHENA_HASH_LEN; i++) {
    if (anthena_lists[i] == NULL || anthena_lists[i]->len == 1) continue;

    Anthena *anthenas = anthena_lists[i]->values;
    print_array(*anthena_lists[i]);

    for (int a = 0; a < anthena_lists[i]->len - 1; a++) {
      for (int b = a + 1; b < anthena_lists[i]->len; b++) {
        append(&distances[i], abs_distance(anthenas[a], anthenas[b]));
      }
    }

    print_array(*distances[i]);
  }
}

int is_in_bounds(Antinode antinode) {
  return antinode.x >= 0 && antinode.y >= 0 && antinode.x < BOARD_WIDTH && antinode.y < BOARD_HEIGHT;
};

int main() {
  FILE *fd;
  char board[BOARD_HEIGHT][BOARD_WIDTH];
  ArrayList *anthena_lists[ANTHENA_HASH_LEN] = { NULL };
  ArrayList *distance_lists[ANTHENA_HASH_LEN] = { NULL };

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  int solution = 0;
  build_board(fd, board);
  print_board(board);
  get_anthenas(board, anthena_lists);
  get_distances(anthena_lists, distance_lists);

  for (int i = 0; i < ANTHENA_HASH_LEN; i++) {
    if (anthena_lists[i] == NULL || anthena_lists[i]->len == 1) continue;

    int distance_index = 0;
    Anthena *anthenas = anthena_lists[i]->values;
    Distance *distances = distance_lists[i]->values;

    for (int a = 0; a < anthena_lists[i]->len - 1; a++) {
      for (int b = a + 1; b < anthena_lists[i]->len; b++) {
        Antinode antinode_a, antinode_b;
        antinode_a.value = '#';
        antinode_a.x = anthenas[a].x - distances[distance_index].x;
        antinode_a.y = anthenas[a].y - distances[distance_index].y;

        antinode_b.value = '#';
        antinode_b.x = distances[distance_index].x + anthenas[b].x;
        antinode_b.y = distances[distance_index].y + anthenas[b].y;

        if (is_in_bounds(antinode_a) && board[antinode_a.y][antinode_a.x] != antinode_a.value) {
          solution++;
          board[antinode_a.y][antinode_a.x] = antinode_a.value;
        }
        if (is_in_bounds(antinode_b) && board[antinode_b.y][antinode_b.x] != antinode_b.value) {
          solution++;
          board[antinode_b.y][antinode_b.x] = antinode_b.value;
        }

        distance_index++;
      }
    }
  }

  print_board(board);
  printf("solution: %d\n", solution);

  for (int i = 0; i < ANTHENA_HASH_LEN; i++) {
    if (anthena_lists[i] != NULL) free(anthena_lists[i]);
    if (distance_lists[i] != NULL) free(distance_lists[i]);
  }

  fclose(fd);
  return 0;
}
