#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "input.txt"
#define MAX_RED_TILES 500
#define MAX_AREAS 132275
#define FREE '.'
#define RED '#'
#define GREEN '0'

typedef long int i32;
typedef unsigned long int u32;
typedef unsigned int u16;

typedef struct Tile {
  int x;
  int y;
} Tile;

typedef struct Area {
  Tile corner_a;
  Tile corner_b;
  u32 value;
} Area;

typedef struct Board {
  char *values;
  u16 width;
  u16 height;
} Board;

typedef enum Direction { Up, Right, Down, Left } Direction;

u16 parse_tiles(FILE *fd, Tile tiles[MAX_RED_TILES]) {
  u16 len = 0;
  while (fscanf(fd, "%d,%d\n", &tiles[len].x, &tiles[len].y) != EOF) {
    len++;
  }

  return len;
}

// u16 abs(int value) {
//   return value > 0 ? value : -value;
// }

Tile next_fill(Tile tile, Direction direction) {
  Tile result = { .x = tile.x, .y = tile.y };

  switch (direction) {
    case Up:
      result.x++;
      break;
    case Right:
      result.y++;
      break;
    case Down:
      result.x--;
      break;
    case Left:
      result.y--;
      break;
    default:
      break;
  }

  return result;
}

void build_board(Tile tiles[MAX_RED_TILES], u16 tiles_len, Board *board) {
  u16 min_delta_x = -1;
  u16 min_delta_y = -1;

  board->width = 0;
  board->height = 0;

  for (u16 i = 0; i < tiles_len; i++) {
    u16 next_index = (i + 1) % tiles_len;
    Tile current = tiles[i];
    Tile next = tiles[next_index];

    if (board->width < current.x) board->width = current.x;
    if (board->height < current.y) board->height = current.y;

    u16 delta_x = abs(current.x - next.x);
    u16 delta_y = abs(current.y - next.y);

    if (delta_x > 0 && min_delta_x > delta_x) min_delta_x = delta_x;
    if (delta_y > 0 && min_delta_y > delta_y) min_delta_y = delta_y;
  }

  // board->ratio.x = 2;
  // board->ratio.y = 1;
  // board->ratio.x = min_delta_x;
  // board->ratio.y = min_delta_y;
  // board->width = (board->width + 2) / board->ratio.x + 1;
  // board->height = (board->height + 2) / board->ratio.y + 1;

  board->width += 2;
  board->height += 2;

  u32 board_size = sizeof(char) * board->width * board->height;
  printf("Board %u x %u, Min delta X %u, Min delta Y %u\n", board->width, board->height, min_delta_x, min_delta_y);
  board->values = (char *)malloc(board_size);
  memset(board->values, FREE, board_size);

  // Build border
  for (u16 i = 0; i < tiles_len; i++) {
    u16 next_index = (i + 1) % tiles_len;
    Tile current = { .x = tiles[i].x, .y = tiles[i].y };
    Tile next = { .x = tiles[next_index].x , .y = tiles[next_index].y };

    int start_x = current.x;
    int end_x = next.x;
    if (start_x > end_x) {
      start_x = next.x;
      end_x = current.x;
    }
    for (int x = start_x + 1; x < end_x; x++) {
      board->values[current.y * board->width + x] = GREEN;
    }

    int start_y = current.y;
    int end_y = next.y;
    if (start_y > end_y) {
      start_y = next.y;
      end_y = current.y;
    }
    for (int y = start_y + 1; y < end_y; y++) {
      board->values[y * board->width + current.x] = GREEN;
    }

    board->values[(current.y * board->width) + current.x] = RED;
    board->values[(next.y * board->width) + next.x] = RED;
  }

  // Build full area
  // for (u16 i = 0; i < tiles_len; i++) {
  //   u16 next_index = (i + 1) % tiles_len;
  //   Tile start = { .x = tiles[i].x, .y = tiles[i].y };
  //   Tile end = { .x = tiles[next_index].x, .y = tiles[next_index].y };

  //   Direction direction = start.x - end.x > 0 ? Left : Right;

  //   if (start.y - end.y > 0) {
  //     direction = Up;
  //   }

  //   if (start.y - end.y < 0) {
  //     direction = Down;
  //   }

  //   printf("Start {%d,%d} End {%d,%d} -> Dir %d \n", start.x, start.y, end.x, end.y, direction);
  //   Tile current = { .x = start.x, .y = start.y };
  //   while (current.x != end.x || current.y != end.y) {
  //     printf("Current {%d,%d} -> Dir %d \n", current.x, current.y, direction);
  //     Tile next = next_fill(current, direction);
  //     printf("Next fill {%d,%d}\n", next.x, next.y);
  //     while (board->values[next.y * board->width + next.x] == FREE) {
  //       board->values[next.y * board->width + next.x] = GREEN;
  //       next = next_fill(next, direction);
  //     }

  //     switch (direction) {
  //       case Up:
  //         current.y--;
  //         break;
  //       case Right:
  //         current.x++;
  //         break;
  //       case Down:
  //         current.y++;
  //         break;
  //       case Left:
  //         current.x--;
  //         break;
  //       default:
  //         break;
  //     }
  //   }
  // }
}

u16 abs_(int value){
  return value > 0 ? value : -value;
}

u32 get_area(Tile corner_a, Tile corner_b) {
  u16 delta_x = abs_(corner_a.x - corner_b.x) + 1;
  u16 delta_y = abs_(corner_a.y - corner_b.y) + 1;

  return (u32)(delta_x) * (u32)(delta_y);
}

int comp_areas(const void *a, const void *b) {
  Area *area_a = (Area *)a;
  Area *area_b = (Area *)b;
  i32 diff = area_b->value - area_a->value;

  if (diff > 0) return 1;
  if (diff < 0) return -1;

  return 0;
}

short is_inside(Area rect, Area *lines, u16 lines_len) {
  short result = 1;

  u16 l = rect.corner_a.x;
  u16 t = rect.corner_a.y;
  u16 r = rect.corner_b.x;
  u16 b = rect.corner_b.y;
  printf("Rectangle {%u,%u} - {%u,%u} area %lu\n", rect.corner_a.x, rect.corner_a.y, rect.corner_b.x, rect.corner_b.y, rect.value);

  // for (u16 t = rect.corner_a.y; t <= rect.corner_b.y; t++) {
    for (u16 j = 0; j < lines_len; j++) {
      u16 x1 = lines[j].corner_a.x;
      u16 y1 = lines[j].corner_a.y;
      u16 x2 = lines[j].corner_b.x;
      u16 y2 = lines[j].corner_b.y;
      // u16 intersections = 0;

      // if (lx < r && ly < b && rx > rect.corner_a.x /*&& rx > rect.corner_b.x*/ && ry > t) {
      if (x1 < r && y1 < b && x2 > l && y2 > t) {
        return 0;
      }
      // ((y1 <= y && y < y2) || (y2 <= y && y < y1)) && x < ((x2 - x1) / (y2 - y1) * (y - y1) + x1)
      // for (u16 x = rect.corner_a.x; x <= rect.corner_b.x; x++) {
      //   if (((y1 <= rect.corner_a.y && rect.corner_a.y < y2) || (y2 <= rect.corner_a.y && rect.corner_a.y < y1)) && x < ((x2 - x1) / (y2 - y1) * (rect.corner_a.y - y1) + x1)) {
      //     // intersections++;
      //     return 0;
      //   }

        // if (((y1 <= rect.corner_b.y && rect.corner_b.y < y2) || (y2 <= rect.corner_b.y && rect.corner_b.y < y1)) && x < ((x2 - x1) / (y2 - y1) * (rect.corner_a.y - y1) + x1)) {
        //   // intersections++;
        //   return 0;
        // }
      // }
      // if (((y1 <= rect.corner_a.y && rect.corner_a.y < y2) || (y2 <= rect.corner_a.y && rect.corner_a.y < y1)) && rect.corner_a.x < ((x2 - x1) / (y2 - y1) * (rect.corner_a.y - y1) + x1)) {
      //   // intersections++;
      //   return 0;
      // }
      // // printf();

      // if (((y1 <= rect.corner_b.y && rect.corner_b.y < y2) || (y2 <= rect.corner_b.y && rect.corner_b.y < y1)) && rect.corner_b.x < ((x2 - x1) / (y2 - y1) * (rect.corner_b.y - y1) + x1)) {
      //   // intersections++;
      //   return 0;
      // }

      // if (((y1 <= rect.corner_b.y && rect.corner_b.y < y2) || (y2 <= rect.corner_b.y && rect.corner_b.y < y1)) && rect.corner_a.x < ((x2 - x1) / (y2 - y1) * (rect.corner_b.y - y1) + x1)) {
      //   // intersections++;
      //   return 0;
      // }

      // if (((y1 <= rect.corner_a.y && rect.corner_a.y < y2) || (y2 <= rect.corner_b.y && rect.corner_b.y < y1)) && rect.corner_b.x < ((x2 - x1) / (y2 - y1) * (rect.corner_a.y - y1) + x1)) {
      //   // intersections++;
      //   return 0;
      // }

      // if (intersections < 1) return 0;
    }
  // }

  // for (u16 l = rect.corner_a.x; l <= rect.corner_b.x; l++) {
  //   for (u16 j = 0; j < lines_len; j++) {
  //     u16 lx = lines[j].corner_a.x;
  //     u16 ly = lines[j].corner_a.y;
  //     u16 rx = lines[j].corner_b.x;
  //     u16 ry = lines[j].corner_b.y;

  //     if (lx < r && ly < b && rx > l && ry > rect.corner_a.y/* && ry > rect.corner_b.y*/) {
  //     // if (lx < r && ly < b && rx > l && ry > t) {
  //       return 0;
  //     }
  //   }
  // }

  return result;
}

void write_bmp(Board board) {
  FILE *fd;
  u16 width = 9000;
  u16 height = 9000;
  // u16 width = board.width;
  // u16 height = board.height;
  u16 size = 54 + 4 + width * height;
  u16 header[] = { size, 0, 0 };
  int DIB_header[] = {
    0x28,    // header size
    width,   // width
    height,  // height
    0x180001, // color | bits per pixel
    0x00,    // no compression
    0x00,    // size in bytes (ignored)
    0x002e23,  // print resolution 
    0x002e23,  // no compression
    0x00,    // no color pallete
    0x00,    // no color pallete
  };

  fd = fopen("result.bmp", "w+");
  if (fd == NULL) {
    perror("Error creating result.");
    return;
  }

  fprintf(fd, "BM");
  fwrite(&header, sizeof(header), 1, fd);
  fwrite(&DIB_header, sizeof(DIB_header), 1, fd);

  u16 padded_width = width % 4 == 0 ? width : width - width % 4 + 4;
  for (u16 y = 0; y < height; y++) {
    for (u16 x = 0; x < padded_width; x++) {
      if (x >= width) {
        fputc(0, fd);
      }

      char pixel[] = {
        board.values[(y) * board.width + x + 9000] != FREE ? 0 : 0xFF, // blue
        board.values[y * board.width + x + 9000] != FREE ? 0 : 0xFF, // green
        0xFF, // red
      };
      fwrite(&pixel, sizeof(pixel), 1, fd);
    }
  }


  fclose(fd);
}

void write_svg(Area *lines, u16 lines_len, Area result) {
  FILE *svg;

  // for (u16 i = 0; i < lines_len; i++) {
  //   if (width < lines[i].corner_b.x) width = lines[i].corner_b.x;
  //   if (height < lines[i].corner_b.y) height = lines[i].corner_b.y;
  // }

  svg = fopen("result.svg", "w+");
  if (svg == NULL) {
    perror("Error creating svg result file");
  }

  fprintf(svg, "<svg width=\"1080px\" background=\"white\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 100000 100000\">\n");
  fprintf(svg, "<g stroke=\"black\" stroke-width=\"100\">\n");
  for (u16 i = 0; i < lines_len; i++) {
    Area current = lines[i];
    fprintf(svg, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" />\n", current.corner_a.x, current.corner_a.y, current.corner_b.x, current.corner_b.y);
  }

  int result_width = result.corner_b.x - result.corner_a.x;
  int result_height = result.corner_b.y - result.corner_a.y;

  fprintf(svg, "<rect x=\"%d\" y=\"%d\" width=\"%d\" fill=\"#FF0000\" opacity=\"0.5\" height=\"%d\" />", result.corner_a.x, result.corner_a.y, result_width, result_height);
  fprintf(svg, "</g>\n");
  fprintf(svg, "</svg>");
  fclose(svg);
}

int main() {
  FILE *fd;
  Tile tiles[MAX_RED_TILES];
  // Board board;
  Area areas[MAX_AREAS];
  Area lines[MAX_RED_TILES];

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  // areas = (Area *)malloc(sizeof(Area) * MAX_AREAS);
  u16 tiles_len = parse_tiles(fd, tiles);
  printf("Red tiles = %u\n", tiles_len);

  // for (int y = 0; y < board.height; y++) {
  //   for (int x = 0; x < board.width; x++) {
  //     printf("%c", board.values[y * board.width + x]);
  //   }
  //   printf("\n");
  // }
  // for (int y = 50350; y < 51581; y++) {
  //   for (int x = 98214; x < 98315; x++) {
  //     printf("%c", board.values[y * board.width + x]);
  //   }
  //   printf("\n");
  // }

  u32 solution = 0;
  // for (u16 i = 0; i < tiles_len - 1; i++) {
  //   printf("Current {%u,%u}\n", tiles[i].x, tiles[i].y);
  //   for (u16 j = i + 1; j < tiles_len; j++) {
  //     int raw_delta_x = tiles[i].x - tiles[j].x;
  //     int raw_delta_y = tiles[i].y - tiles[j].y;

  //     // printf("Rectangle {%u,%u} - {%u,%u} raw_x %d raw_y %d\n", tiles[i].x, tiles[i].y, tiles[j].x, tiles[j].y, raw_delta_x, raw_delta_y);
  //     Tile start_corner = {
  //       .x = raw_delta_x > 0 ? tiles[j].x : tiles[i].x,
  //       .y = raw_delta_y > 0 ? tiles[j].y : tiles[i].y,
  //     };
  //     Tile end_corner = {
  //       .x = raw_delta_x > 0 ? tiles[i].x : tiles[j].x,
  //       .y = raw_delta_y > 0 ? tiles[i].y : tiles[j].y,
  //     };
  //     // printf("From {%u,%u} to {%u,%u}\n", start_corner.x, start_corner.y, end_corner.x, end_corner.y);

  //     short inside = 1;
  //     for (u16 y = start_corner.y; y <= end_corner.y; y++) {
  //       if (!inside) break;
  //       for (u16 x = start_corner.x; x <= end_corner.x; x++) {
  //         if (board.values[y * board.width + x] == FREE) {
  //           inside = 0;
  //           break;
  //         }
  //       }
  //     }
  //     if (!inside) continue;

  //     u16 delta_x = abs(raw_delta_x) + 1;
  //     u16 delta_y = abs(raw_delta_y) + 1;
  //     u32 area = (u32)(delta_x) * (u32)(delta_y);

  //     // if (tiles[i].x == 85714 && tiles[i].y == 83382 && tiles[j].x == 15251 && tiles[j].y == 15659) {
  //     //   printf("Delta X %u, Delta Y %u, Area %ju\n", delta_x, delta_y, area);
  //     // }
  //     // printf("From {%d,%d} to {%d,%d}\n", tiles[i].x, tiles[i].y, tiles[j].x, tiles[j].y);
  //     if (area > solution) {
  //       printf("From {%d,%d} to {%d,%d}\n", tiles[i].x, tiles[i].y, tiles[j].x, tiles[j].y);
  //       solution = area;
  //     }
  //   }
  // }
  u16 areas_len = 0;
  for (u16 i = 0; i < tiles_len - 1; i++) {
    for (u16 j = i + 1; j < tiles_len; j++) {
      areas[areas_len].corner_a.x = tiles[i].x > tiles[j].x ? tiles[j].x : tiles[i].x;
      areas[areas_len].corner_a.y = tiles[i].y > tiles[j].y ? tiles[j].y : tiles[i].y;

      areas[areas_len].corner_b.x = tiles[i].x < tiles[j].x ? tiles[j].x : tiles[i].x;
      areas[areas_len].corner_b.y = tiles[i].y < tiles[j].y ? tiles[j].y : tiles[i].y;

      areas[areas_len].value = get_area(tiles[i], tiles[j]);
      areas_len++;
    }
  }
  printf("Found %u rectangles\n", areas_len);
  qsort(&areas, areas_len, sizeof(Area), comp_areas);

  u16 lines_len = 0;
  Area result;
  for (u16 i = 0; i < tiles_len; i++) {
    Tile origin = tiles[i];
    Tile destination = tiles[(i + 1) % tiles_len];

    lines[lines_len].corner_a.x = origin.x > destination.x ? destination.x : origin.x;
    lines[lines_len].corner_a.y = origin.y > destination.y ? destination.y : origin.y;

    lines[lines_len].corner_b.x = origin.x < destination.x ? destination.x : origin.x;
    lines[lines_len].corner_b.y = origin.y < destination.y ? destination.y : origin.y;

    // lines[lines_len].value = get_area(tiles[i], tiles[i + 1 % tiles_len]);
    lines_len++;
  }

  // qsort(&lines, lines_len, sizeof(Area), comp_areas);
  printf("Found %u lines\n", lines_len);
  // const bounds = (x1,y1,x2,y2) => [ Math.min(x1,x2), Math.min(y1,y2), Math.max(x1,x2), Math.max(y1,y2) ];
  // const area = (x1,y1,x2,y2) => (x2-x1+1)*(y2-y1+1);
  // const areaDesc = (a,b) => area(...b)-area(...a);

  // const red = loadLines("input.txt").map(s => s.split(",").map(Number));
  // const pairs = red.flatMap((p,i) => red.slice(i+1).map(q => bounds(...p,...q))).sort(areaDesc);
  // const lines = red.map((p,i) => bounds(...p,...red[(i+1) % red.length])).sort(areaDesc);
  // const good = pairs.find(([l,t,r,b]) => !lines.find(([lx,ly,rx,ry]) => lx < r && ly < b && rx > l && ry > t));

  for (u16 i = 0; i < areas_len; i++) {
    if (is_inside(areas[i], lines, lines_len)) {
      solution = areas[i].value;
      result = areas[i];
      printf("Solution in index %u\n", i);
      break;
    }
  }

  // 526854903

  printf("\nSolution %lu\n", solution);
  write_svg(lines, lines_len, result);

  // build_board(tiles, tiles_len, &board);
  // printf("Board of %u X %u\n", board.width, board.height);
  // write_bmp(board);
  // free(areas);
  // free(board.values);
  fclose(fd);
  return -1;
}
