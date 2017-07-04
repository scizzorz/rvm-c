#ifndef R_RAIN_H
#define R_RAIN_H

#include <stdbool.h>
#include <stdio.h>

// types
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

void box_print(R_box *val);
void op_print(R_op *instr);

R_vm *vm_load(FILE *fp);
bool vm_exec(R_vm *this, R_op *instr);
bool vm_step(R_vm *this);
bool vm_run(R_vm *this);
void vm_dump(R_vm *this);

#endif
