#include <stdio.h>

#define MAX_FILES 15000
#define MAX_LAYOUT 100000

short int char_to_number(char number) {
  return number - '0';
}
int is_number(char value) {
  return value >= '0' && value <= '9';
}

int parse_input(FILE *fd, unsigned short int files[MAX_FILES]) {
  char current_file;
  unsigned int file_index = 0;
  while((current_file = fgetc(fd)) != EOF) {
    files[file_index] = char_to_number(current_file) << 4;
    char empty_blocks = fgetc(fd);
    if (is_number(empty_blocks)) {
      files[file_index] |= char_to_number(empty_blocks);
    }
    printf("%c%c", current_file, empty_blocks);
    
    file_index++;
  }
  printf("\n");

  return file_index;
}

int build_layout(int memory_layout[MAX_LAYOUT], unsigned short int files[MAX_FILES], unsigned int files_len) {
  unsigned int layout_index = 0;
  for (int i = 0; i < files_len; i++) {
    unsigned short int used = files[i] >> 4;
    unsigned short int empty = files[i] & 0x0F;

    for (int j = 0; j < used; j++) {
      memory_layout[layout_index++] = i;
    }
    for (int j = 0; j < empty; j++) {
      memory_layout[layout_index++] = -1;
    }
  }

  return layout_index;
}

void print_layout(int memory_layout[MAX_LAYOUT], unsigned int total_layout) {
  printf("total layout %d\n", total_layout);
  for (int i = 0; i < total_layout; i++) {
    printf("%d", memory_layout[i]);
  }
  printf("\n");
}

void compress(int memory_layout[MAX_LAYOUT], unsigned int total_layout) {
  unsigned int nearest_empty = 0;
  for (int i = total_layout - 1; i >= 0; i--) {
    if (memory_layout[i] == -1) continue;

    while (memory_layout[nearest_empty] != -1 && nearest_empty < i) {
      nearest_empty++;
    }
    if (memory_layout[nearest_empty] == -1 && nearest_empty < i) {
      memory_layout[nearest_empty] = memory_layout[i];
      memory_layout[i] = -1;
    }
  }
}

int main() {
  FILE *fd;
  // First 4 bits represent the number of files and the other 4 represent the blocks
  unsigned short int files[MAX_FILES] = {0};
  int memory_layout[MAX_LAYOUT] = {-1};
  unsigned int total_files;
  unsigned int total_layout;

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
  }

  total_files = parse_input(fd, files);
  printf("Total files: %u\n", total_files);

  for (int i = 0; i < total_files; i ++) {
    printf("0x%X, ", files[i]);
  }
  printf("\n");

  total_layout = build_layout(memory_layout, files, total_files);
  print_layout(memory_layout, total_layout);
  compress(memory_layout, total_layout);
  print_layout(memory_layout, total_layout);

  unsigned long int solution = 0;
  for (int i = 0; i < total_layout; i++) {
    if (memory_layout[i] == -1) continue;
    solution += (i * memory_layout[i]);
  }

  printf("solution: %lu\n", solution);

  fclose(fd);
  return 0;
}
