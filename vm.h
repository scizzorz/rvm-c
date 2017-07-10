#ifndef R_VM_H
#define R_VM_H

#include "rain.h"

typedef struct R_vm {
  uint32_t instr_ptr;
  uint32_t num_consts;
  uint32_t num_instrs;
  uint32_t num_strings;
  uint32_t stack_ptr;
  uint32_t stack_size;
  uint32_t scope_size;
  uint32_t scope_ptr;

  char **strings;
  R_box *consts;
  R_op *instrs;
  R_box *stack;
  R_box *scopes;
} R_vm;

R_vm *vm_load(FILE *fp);
bool vm_exec(R_vm *this, R_op *instr);
bool vm_step(R_vm *this);
bool vm_run(R_vm *this);
void vm_dump(R_vm *this);
R_box vm_pop(R_vm *this);
R_box vm_top(R_vm *this);
void vm_push(R_vm *this, R_box *val);
void vm_set(R_vm *this, R_box *val);
void vm_new_scope(R_vm *this);

#endif
