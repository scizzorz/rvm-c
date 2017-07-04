#include <gc.h>
#include <stdbool.h>
#include <stdio.h>

#define TYPE_NULL  0
#define TYPE_INT   1
#define TYPE_FLOAT 2
#define TYPE_BOOL  3
#define TYPE_STR   4
#define TYPE_TABLE 5
#define TYPE_FUNC  6
#define TYPE_CDATA 7

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

#define LOAD_CONST 0
#define PRINT_ITEM 1

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

void box_print(R_box *val) {
  switch(val->type) {
    case TYPE_NULL:
      printf("null\n");
      break;
    case TYPE_INT:
      printf("%ld\n", val->data.si);
      break;
    case TYPE_FLOAT:
      printf("%f\n", val->data.f);
      break;
    case TYPE_BOOL:
      printf(val->data.si != 0 ? "true\n" : "false\n");
      break;
    case TYPE_STR:
      printf("%s\n", val->data.s);
      break;
    case TYPE_TABLE:
      printf("table 0x%08lx\n", (unsigned long)val->data.p);
      break;
    case TYPE_FUNC:
      printf("func 0x%08lx\n", (unsigned long)val->data.p);
      break;
    case TYPE_CDATA:
      printf("cdata 0x%08lx\n", (unsigned long)val->data.p);
      break;
    default:
      printf("unknown\n");
  }
}

void op_print(R_op *instr) {
  printf("Instr<%d: %d %d %d>\n", instr->op, instr->args[0], instr->args[1], instr->args[2]);
}

R_vm *vm_load(FILE *fp) {
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

  R_vm *this = vm_load(fp);

  return 0;
}
