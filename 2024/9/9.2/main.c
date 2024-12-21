#include <stdio.h>
#include <string.h>

#define MAX_FILES 15000
#define MAX_LAYOUT 100000

typedef struct File {
  unsigned short int used;
  unsigned short int empty;
  unsigned int id;
} File;

short int char_to_number(char number) {
  return number - '0';
}
int is_number(char value) {
  return value >= '0' && value <= '9';
}

int parse_input(FILE *fd, File files[MAX_FILES]) {
  char current_file;
  unsigned int file_index = 0;
  while((current_file = fgetc(fd)) != EOF) {
    files[file_index].used = char_to_number(current_file);
    char empty_blocks = fgetc(fd);
    files[file_index].empty = is_number(empty_blocks) ? char_to_number(empty_blocks) : 0;

    printf("%c%c", current_file, empty_blocks);
    files[file_index].id = file_index;
    
    file_index++;
  }
  printf("\n");

  return file_index;
}

void print_memory(File files[MAX_FILES], unsigned int files_len) {
  for (int i = 0; i < files_len; i++) {
    unsigned short int used = files[i].used;
    unsigned short int empty = files[i].empty;

    for (int j = 0; j < used; j++) {
      printf("%hu", files[i].id);
    }
    for (int j = 0; j < empty; j++) {
      printf(".");
    }
  }
  printf("\n");
}

void compress(File files[MAX_FILES], unsigned int files_len) {
  unsigned short int visited[files_len];
  memset(visited, 0, sizeof(visited));

  for (int i = files_len - 1; i >= 0; i--) {
    if (visited[files[i].id]) continue;
    visited[files[i].id] = 1;
    unsigned short int used = files[i].used;
    unsigned short int empty = files[i].empty;
    unsigned int id = files[i].id;

    unsigned int nearest_fit = 0;
    while (used > (files[nearest_fit].empty) && nearest_fit < i) {
      nearest_fit++;
    }
    if (used <= (files[nearest_fit].empty) && nearest_fit < i) {
      printf("moving %u after %u\n", files[i].id, files[nearest_fit].id);
      // change used empty to = used - nearest empty
      unsigned short int new_empty = (files[nearest_fit].empty) - used;
      // move files to the right
      memmove(files + nearest_fit + 1, files + nearest_fit, (i - nearest_fit) * sizeof(File));
      printf("memory after move\n");
      /* print_memory(files, files_len); */
      printf("new arrange {used %u}, {empty %u}\n", used, new_empty);
      // change nearest empty to 0
      files[nearest_fit].empty = 0;
      files[nearest_fit + 1].used = used;
      files[nearest_fit + 1].empty = new_empty;
      files[nearest_fit + 1].id = id;

      // increase empty at the right used + empty
      new_empty = files[i].empty + used + empty;
      printf("current id %d used %u new empty %u\n", files[i].id, files[i].used, new_empty);
      files[i].empty = new_empty;
      i++;
    }
  }
}

int main() {
  FILE *fd;
  File files[MAX_FILES];
  unsigned int total_files;

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
  }

  total_files = parse_input(fd, files);
  printf("Total files: %u\n", total_files);

  print_memory(files, total_files);
  compress(files, total_files);
  print_memory(files, total_files);

  unsigned long long int solution = 0;
  unsigned int memory_index = 0;
  for (int i = 0; i < total_files; i++) {
    unsigned short int used = files[i].used;
    unsigned short int empty = files[i].empty;

    for (int j = 0; j < used; j++) {
      solution += files[i].id * memory_index;
      memory_index++;
    }
    for (int j = 0; j < empty; j++) {
      memory_index++;
    }
  }

  printf("solution: %llu\n", solution);

  fclose(fd);
  return 0;
}
