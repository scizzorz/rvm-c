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
  char op;
  char args[3];
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

  this->instr_ptr = 0;
  this->stack_ptr = 0;
  this->stack_size = 10;

  this->stack = GC_malloc(sizeof(R_box) * this->stack_size);
  this->consts = GC_malloc(sizeof(R_box) * this->num_consts);
  this->instrs = GC_malloc(sizeof(R_op) * this->num_instrs);

  rv = fread(this->consts, sizeof(R_box), this->num_consts, fp);
  if(rv != this->num_consts) {
    fprintf(stderr, "Unable to read constants\n");
    return NULL;
  }

  rv = fread(this->instrs, sizeof(R_op), this->num_instrs, fp);
  if(rv != this->num_instrs) {
    fprintf(stderr, "Unable to read instructions\n");
    return NULL;
  }

  return this;
}

void R_PRINT_ITEM(R_vm *this) {
  R_box *val = this->stack + this->stack_ptr - 1;
  box_print(val);
}

void R_LOAD_CONST(R_vm *this, char idx) {
  this->stack[this->stack_ptr] = this->consts[idx];
  this->stack_ptr += 1;
}

bool vm_exec(R_vm *this, R_op *instr) {
  switch(instr->op) {
    case LOAD_CONST:
      R_LOAD_CONST(this, instr->args[0]);
      break;
    case PRINT_ITEM:
      R_PRINT_ITEM(this);
      break;
    default:
      printf("Unknown instruction %d %d %d %d\n", instr->op, instr->args[0], instr->args[1], instr->args[2]);
  }

  return true;
}

bool vm_step(R_vm *this) {
  if(this->instr_ptr < this->num_instrs) {
    vm_exec(this, this->instrs + this->instr_ptr);
    this->instr_ptr += 1;
  }

  return true;
}

bool vm_run(R_vm *this) {
  while(this->instr_ptr < this->num_instrs) {
    vm_step(this);
  }

  return true;
}

void vm_dump(R_vm *this) {
  printf("Constants (%d):\n", this->num_consts);
  for(int i=0; i<this->num_consts; i++) {
    printf("   ");
    box_print(this->consts + i);
  }

  printf("Instructions (%d):\n", this->num_instrs);
  for(int i=0; i<this->num_instrs; i++) {
    printf(i == this->instr_ptr ? "-> " : "   ");
    op_print(this->instrs + i);
  }

  printf("Stack (%d):\n", this->stack_size);
  for(int i=0; i<this->stack_size; i++) {
    printf(i == this->stack_ptr ? "-> " : "   ");
    box_print(this->stack + i);
  }
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

  if(this == NULL) {
    fprintf(stderr, "Unable to load VM\n");
    return 1;
  }

  vm_dump(this);

  return 0;
}
