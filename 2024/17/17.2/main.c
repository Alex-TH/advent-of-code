#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"
#define MAX_OUT_BUFFER 1024
#define MAX_ISTRUCTIONS 10

typedef struct CPU {
  unsigned long long int ax;
  unsigned long long int bx;
  unsigned long long int cx;
  short pc;
  char out[MAX_OUT_BUFFER];
  int out_len;
} CPU;
typedef enum { ADV, BXL, BST, JNZ, BXC, OUT, BDV, CDV } Opcode;
typedef struct Instruction {
  short opcode;
  short operand;
} Instruction;

typedef void (* instruction_fn)(CPU *cpu, short operand);

void initialize_cpu(CPU *cpu, FILE *fd) {
  fscanf(fd, "Register A: %u\n", &cpu->ax);
  fscanf(fd, "Register B: %u\n", &cpu->bx);
  fscanf(fd, "Register C: %u\n\n", &cpu->cx);
  cpu->pc = 0;
  cpu->out_len = 0;
}

void restart_cpu(CPU *cpu) {
  cpu->ax = 0;
  cpu->bx = 0;
  cpu->cx = 0;
  cpu->pc = 0;
  cpu->out_len = 0;
}

short initialize_program(FILE *fd, Instruction program[MAX_ISTRUCTIONS]) {
  short program_len = 0;

  fscanf(fd, "Program: %hd,%hd,", &program[program_len].opcode, &program[program_len].operand);
  program_len++;

  while (fscanf(fd, "%hd,%hd", &program[program_len].opcode, &program[program_len].operand) != EOF) {
    fgetc(fd);
    program_len++;
  }

  return program_len;
}

unsigned long long int combo_operand(short operand, CPU cpu) {
  switch (operand) {
    case 4: return cpu.ax;
    case 5: return cpu.bx;
    case 6: return cpu.cx;
  }

  return operand;
}

unsigned long long int pow_2(int power) {
  return 1 << power;
}

void adv(CPU *cpu, short operand) {
  unsigned long long int denominator = pow_2(combo_operand(operand, *cpu));
  cpu->ax /= denominator;
  cpu->pc += 2;
}

void bxl(CPU *cpu, short operand) {
  cpu->bx ^= operand;
  cpu->pc += 2;
}

void bst(CPU *cpu, short operand) {
  cpu->bx = combo_operand(operand, *cpu) % 8;
  cpu->pc += 2;
}

void jnz(CPU *cpu, short operand) {
  if (cpu->ax == 0) {
    cpu->pc += 2;
  } else {
    cpu->pc = operand;
  }
}

void bxc(CPU *cpu, short operand) {
  cpu->bx ^= cpu->cx;
  cpu->pc += 2;
}

void out(CPU *cpu, short operand) {
  short combo_value = combo_operand(operand, *cpu) % 8;

  if (cpu->out_len > 0) {
    cpu->out[cpu->out_len++] = ',';
  }

  cpu->out[cpu->out_len++] = combo_value + '0';
  cpu->out[cpu->out_len] = '\0';
  cpu->pc += 2;
}

void bdv(CPU *cpu, short operand) {
  int denominator = pow_2(combo_operand(operand, *cpu));
  cpu->bx = cpu->ax / denominator;
  cpu->pc += 2;
}

void cdv(CPU *cpu, short operand) {
  unsigned long long int denominator = pow_2(combo_operand(operand, *cpu));
  cpu->cx = cpu->ax / denominator;
  cpu->pc += 2;
}

void print_values(CPU cpu) {
  printf("Register A: %llu\n", cpu.ax);
  printf("Register B: %llu\n", cpu.bx);
  printf("Register C: %llu\n", cpu.cx);
  printf("Program counter: %d\n", cpu.pc);
  printf("Program out len %d: %s\n", cpu.out_len, cpu.out);
}
void run_program(CPU *cpu, Instruction program[MAX_ISTRUCTIONS], short program_len) {
  instruction_fn instructions[CDV + 1] = {
    &adv,
    &bxl,
    &bst,
    &jnz,
    &bxc,
    &out,
    &bdv,
    &cdv,
  };

  while ((cpu->pc / 2) < program_len) {
    short current_instruction_index = cpu->pc / 2;
    Instruction current_instruction = program[current_instruction_index];
    /* print_values(*cpu); */
    instructions[current_instruction.opcode](cpu, current_instruction.operand);
  }
}

int str_equal(char *str_a, char *str_b) {
  while (*str_a != '\0' && *str_b != '\0') {
    if (*str_a != *str_b) return 0;
    str_a++;
    str_b++;
  }

  return 1;
}

int main() {
  CPU cpu;
  Instruction program[MAX_ISTRUCTIONS];
  short program_len = 0;
  char *expected_out;
  short out_len = 0;
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }

  expected_out = malloc(sizeof(char) * MAX_OUT_BUFFER);
  initialize_cpu(&cpu, fd);
  program_len = initialize_program(fd, program);
  for (int i = 0; i < program_len; i++) {
    if (i > 0) expected_out[out_len++] = ',';
    expected_out[out_len++] = program[i].opcode + '0';
    expected_out[out_len++] = ',';
    expected_out[out_len++] = program[i].operand + '0';
  }
  expected_out[out_len] = '\0';
  printf("Expected out: %s\n", expected_out);

  unsigned long long int prev_ax_values[100];
  unsigned long long int stack[1000];
  unsigned long long int current_ax;
  unsigned long long int solution = -1;
  short prev_ax_values_len = 1;
  short stack_len = 0;
  short total_digits = program_len * 2;

  prev_ax_values[0] = 0;

  for (int i = 0; i < total_digits; i++) {
    for (int j = 0; j < prev_ax_values_len; j++) {
      for (int k = 0; k < 8; k++) {
        stack[stack_len++] = prev_ax_values[j] * 8 + k;
      }
    }

    prev_ax_values_len = 0;
    while (stack_len > 0) {
      current_ax = stack[--stack_len];
      restart_cpu(&cpu);
      cpu.ax = current_ax;
      run_program(&cpu, program, program_len);

      if (str_equal(cpu.out, expected_out + out_len - cpu.out_len)) {
        prev_ax_values[prev_ax_values_len++] = current_ax;
        printf("current out %s\n", cpu.out);
        printf("adding %llu\n", current_ax);
        if (cpu.out_len == out_len && current_ax < solution) {
          solution = current_ax;
        }
      }
    }
  }

  printf("expected out %d len: %s\n", out_len, expected_out);
  printf("solution: %llu\n", solution);

  fclose(fd);
  return 0;
}
