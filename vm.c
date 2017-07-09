#include "rain.h"
#include <gc.h>

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

  rv = fread(&this->num_strings, sizeof(int), 1, fp);
  if(rv != 1) {
    fprintf(stderr, "Unable to read num_strings\n");
    return NULL;
  }

  this->instr_ptr = 0;
  this->stack_ptr = 0;
  this->stack_size = 10;

  this->stack = GC_malloc(sizeof(R_box) * this->stack_size);
  this->strings = GC_malloc(sizeof(char *) * this->num_strings);
  this->consts = GC_malloc(sizeof(R_box) * this->num_consts);
  this->instrs = GC_malloc(sizeof(R_op) * this->num_instrs);

  int len = 0;
  for(int i=0; i<this->num_strings; i++) {
    rv = fread(&len, sizeof(int), 1, fp);
    if(rv != 1) {
      fprintf(stderr, "Unable to read string %d length\n", i);
      return NULL;
    }

    this->strings[i] = GC_malloc(sizeof(char) * len + 1);
    rv = fread(this->strings[i], sizeof(char), len, fp);
    if(rv != len) {
      fprintf(stderr, "Unable to read string %d\n", i);
      return NULL;
    }

    this->strings[i][len] = 0;
  }

  rv = fread(this->consts, sizeof(R_box), this->num_consts, fp);
  if(rv != this->num_consts) {
    fprintf(stderr, "Unable to read constants\n");
    return NULL;
  }

  for(int i=0; i<this->num_consts; i++) {
    if(R_TYPE_IS(&this->consts[i], STR)) {
      this->consts[i].str = this->strings[this->consts[i].i64];
    }
  }

  rv = fread(this->instrs, sizeof(R_op), this->num_instrs, fp);
  if(rv != this->num_instrs) {
    fprintf(stderr, "Unable to read instructions\n");
    return NULL;
  }

  return this;
}

bool vm_exec(R_vm *this, R_op *instr) {
  if(R_OP(instr) < NUM_INSTRS) {
    R_INSTR_TABLE[R_OP(instr)](this, instr);
    return true;
  }

  printf("Unknown instruction %02x %06x\n", R_OP(instr), R_UI(instr));
  return false;
}

bool vm_step(R_vm *this) {
  if(this->instr_ptr < this->num_instrs) {
    if(vm_exec(this, this->instrs + this->instr_ptr)) {
      this->instr_ptr += 1;
      return true;
    }
  }

  return false;
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
    R_box_print(this->consts + i);
  }

  printf("Instructions (%d):\n", this->num_instrs);
  for(int i=0; i<this->num_instrs; i++) {
    printf(i == this->instr_ptr ? "-> " : "   ");
    R_op_print(this->instrs + i);
  }

  printf("Stack (%d / %d):\n", this->stack_ptr, this->stack_size);
  for(int i=0; i<this->stack_size; i++) {
    if(i < this->stack_ptr - 1) {
      printf("|  ");
    }
    else if(i == this->stack_ptr - 1) {
      printf("-> ");
    }
    else {
      printf("   ");
    }

    R_box_print(this->stack + i);
  }
}

R_box vm_pop(R_vm *this) {
  this->stack_ptr -= 1;
  return this->stack[this->stack_ptr];
}

R_box vm_top(R_vm *this) {
  return this->stack[this->stack_ptr - 1];
}

void vm_push(R_vm *this, R_box *val) {
  this->stack[this->stack_ptr] = *val;
  this->stack_ptr += 1;
}

void vm_set(R_vm *this, R_box *val) {
  this->stack[this->stack_ptr - 1] = *val;
}
