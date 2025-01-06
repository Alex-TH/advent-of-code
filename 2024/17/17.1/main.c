#include <stdio.h>

#define INPUT_FILE "input.txt"
#define MAX_OUT_BUFFER 1024
#define MAX_ISTRUCTIONS 10

typedef struct CPU {
  int ax;
  int bx;
  int cx;
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
  fscanf(fd, "Register A: %d\n", &cpu->ax);
  fscanf(fd, "Register B: %d\n", &cpu->bx);
  fscanf(fd, "Register C: %d\n\n", &cpu->cx);
  cpu->pc = 0;
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

int combo_operand(short operand, CPU cpu) {
  switch (operand) {
    case 4: return cpu.ax;
    case 5: return cpu.bx;
    case 6: return cpu.cx;
  }

  return operand;
}

int pow_2(int power) {
  return 1 << power;
}

void adv(CPU *cpu, short operand) {
  int denominator = pow_2(combo_operand(operand, *cpu));
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
  cpu->bx = cpu->ax /denominator;
  cpu->pc += 2;
}

void cdv(CPU *cpu, short operand) {
  int denominator = pow_2(combo_operand(operand, *cpu));
  cpu->cx = cpu->ax /denominator;
  cpu->pc += 2;
}

void print_values(CPU cpu) {
  printf("Register A: %d\n", cpu.ax);
  printf("Register B: %d\n", cpu.bx);
  printf("Register C: %d\n", cpu.cx);
  printf("Program counter: %d\n", cpu.pc);
  printf("Program out len %d: %s\n", cpu.out_len, cpu.out);
}

int main() {
  CPU cpu;
  Instruction program[MAX_ISTRUCTIONS];
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
  short program_len = 0;
  FILE *fd;

  fd = fopen(INPUT_FILE, "r");
  if (fd == NULL) {
    perror("Missing input file");
    return -1;
  }
  initialize_cpu(&cpu, fd);
  program_len = initialize_program(fd, program);

  while ((cpu.pc / 2) < program_len) {
    short current_instruction_index = cpu.pc / 2;
    Instruction current_instruction = program[current_instruction_index];
    print_values(cpu);

    instructions[current_instruction.opcode](&cpu, current_instruction.operand);
  }
  print_values(cpu);

  fclose(fd);
  return 0;
}
