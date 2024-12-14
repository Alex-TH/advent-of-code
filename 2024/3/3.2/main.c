#include <stdio.h>

#define STATES 13
#define INPUTS 10

#define MUL_ACCEPTED 3
#define DO_ACCEPTED 7
#define DONT_ACCEPTED 12

int next_state(char input, int state, int automata[STATES][INPUTS]) {
  switch (input) {
    case 'm': return automata[state][0];
    case 'u': return automata[state][1]; 
    case 'l': return automata[state][2]; 
    case 'd': return automata[state][3]; 
    case 'o': return automata[state][4]; 
    case '(': return automata[state][5]; 
    case ')': return automata[state][6]; 
    case 'n': return automata[state][7]; 
    case '\'': return automata[state][8];
    case 't': return automata[state][9];
  }

  return 0;
}

int main() {
  FILE *fd;
  int automata[STATES][INPUTS] = {
  /* 'm' 'u' 'l' 'd' 'o' '(' ')' 'n' '\'' 't' */
    { 1,  0,  0,  4,  0,  0,  0,  0,  0,   0 }, // 0
    { 1,  2,  0,  4,  0,  0,  0,  0,  0,   0 }, // 1
    { 1,  0,  3,  4,  0,  0,  0,  0,  0,   0 }, // 2
    { 1,  0,  0,  4,  0,  0,  0,  0,  0,   0 }, // 3 <- mul accepted
    { 1,  0,  0,  4,  5,  0,  0,  0,  0,   0 }, // 4
    { 1,  0,  0,  4,  0,  6,  0,  8,  0,   0 }, // 5
    { 1,  0,  0,  4,  0,  0,  7,  0,  0,   0 }, // 6
    { 1,  0,  0,  4,  0,  0,  0,  0,  0,   0 }, // 7 <- do accepted
    { 1,  0,  0,  4,  0,  0,  0,  0,  9,   0 }, // 8
    { 1,  0,  0,  4,  0,  0,  0,  0,  0,  10 }, // 9
    { 1,  0,  0,  4,  0,  11, 0,  0,  0,   0 }, // 10
    { 1,  0,  0,  4,  0,  0,  12, 0,  0,   0 }, // 11
    { 1,  0,  0,  4,  0,  0,  0,  0,  0,   0 }, // 12 <- don't accepted
  };

  fd = fopen("input.txt", "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  int state = 0;
  char current_char;
  int solution = 0;
  char mul_enabled = 1;

  while ((current_char = fgetc(fd)) != EOF) {
    int new_state = next_state(current_char, state, automata);
    state = new_state;

    switch (state) {
      case MUL_ACCEPTED: {
        int a, b;
        int scan_result = fscanf(fd, "(%d,%d", &a, &b);
        int is_closing = fgetc(fd) == ')';

        solution += (scan_result == 2 && is_closing && mul_enabled) ? a * b : 0;
        break;
      }
      case DO_ACCEPTED: {
        mul_enabled = 1;
        break;
      }
      case DONT_ACCEPTED: {
        mul_enabled = 0;
        break;
      }
    }
  }

  printf("solution: %d\n", solution);
  fclose(fd);

  return 0;
}
