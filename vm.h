#ifndef R_VM_H
#define R_VM_H

#include "rain.h"

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

R_vm *vm_load(FILE *fp);
bool vm_exec(R_vm *this, R_op *instr);
bool vm_step(R_vm *this);
bool vm_run(R_vm *this);
void vm_dump(R_vm *this);

#endif
