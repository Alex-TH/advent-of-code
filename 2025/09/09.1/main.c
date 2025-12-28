#include <stdio.h>

#define INPUT_FILE "input.txt"
#define MAX_RED_TILES 1000

typedef unsigned long int u32;
typedef unsigned int u16;

typedef struct Tile {
  int x;
  int y;
} Tile;

u16 parse_tiles(FILE *fd, Tile tiles[MAX_RED_TILES]) {
  u16 len = 0;
  while (fscanf(fd, "%d,%d\n", &tiles[len].x, &tiles[len].y) != EOF) {
    len++;
  }

  return len;
}

void print_tiles(Tile tiles[MAX_RED_TILES], u16 len) {
  for (u16 i = 0; i < len; i++) {
    printf("%d,%d\n", tiles[i].x, tiles[i].y);
  }
}

u16 abs(int value){
  return value > 0 ? value : -value;
}

int main() {
  FILE *fd;
  Tile tiles[MAX_RED_TILES];

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  u16 tiles_len = parse_tiles(fd, tiles);
  printf("Red tiles = %u\n", tiles_len);
  // print_tiles(tiles, tiles_len);

  u32 solution = 0;
  for (u16 i = 0; i < tiles_len - 1; i++) {
    for (u16 j = i + 1; j < tiles_len; j++) {
      u16 delta_x = abs(tiles[i].x - tiles[j].x) + 1;
      u16 delta_y = abs(tiles[i].y - tiles[j].y) + 1;
      u32 area = (u32)(delta_x) * (u32)(delta_y);

      if (tiles[i].x == 85714 && tiles[i].y == 83382 && tiles[j].x == 15251 && tiles[j].y == 15659) {
        printf("Delta X %u, Delta Y %u, Area %ju\n", delta_x, delta_y, area);
      }
      // printf("From {%d,%d} to {%d,%d}\n", tiles[i].x, tiles[i].y, tiles[j].x, tiles[j].y);
      if (area > solution) {
        solution = area;
      }
    }
  }

  printf("Solution %lu\n", solution);

  fclose(fd);
  return -1;
}
