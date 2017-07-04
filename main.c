#include <gc.h>
#include <stdbool.h>
#include <stdio.h>

typedef union {
  unsigned long ui;
  signed long si;
  double f;
  char *s;
  void *p;
} R_cast;

typedef struct R_box {
  unsigned char type;
  int size;
  R_cast data;
  struct R_box *meta;
} R_box;

typedef struct R_op {
  char code[4];
} R_op;

typedef struct R_vm {
  int instr_ptr;
  int num_consts;
  int num_instrs;
  int stack_ptr;
  int stack_size;

  R_box *consts;
  R_op *instrs;
  R_box *stack;
} R_vm;

R_vm *vm_new(FILE *fp) {
  int rv;

  R_vm *this = GC_malloc(sizeof(R_vm));

  rv = fread(&this->num_consts, sizeof(int), 1, fp);

  if(rv != 1) {
    fprintf(stderr, "Unable to read num_consts\n");
    return NULL;
  }

  rv = fread(&this->num_instrs, sizeof(int), 1, fp);

  if(rv != 1) {
    fprintf(stderr, "Unable to read num_instrs\n");
    return NULL;
  }

  printf("Consts: %d, instrs: %d\n", this->num_consts, this->num_instrs);

  this->instr_ptr = 0;
  this->stack_ptr = 0;
  this->stack_size = 10;

  return this;
}

bool vm_exec(R_vm *this, R_op *instr) {
  printf("Execute instruction");
  return true;
}

bool vm_run(R_vm *this) {
  while(this->instr_ptr < this->num_instrs) {
    vm_exec(this, this->instrs + this->instr_ptr);
    this->instr_ptr += 1;
  }

  return true;
}

int main(int argv, char **argc) {
  if(argv < 2) {
    fprintf(stderr, "Usage: %s FILE\n", argc[0]);
    return 1;
  }

  FILE *fp = fopen(argc[1], "rb");

  if(fp == NULL) {
    fprintf(stderr, "Unable to open file %s\n", argc[1]);
    return 1;
  }

  R_vm *this = vm_new(fp);

  return 0;
}
