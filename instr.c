#include "rain.h"

void R_PRINT_ITEM(R_vm *this) {
  R_box *val = this->stack + this->stack_ptr - 1;
  box_print(val);
}

void R_LOAD_CONST(R_vm *this, char idx) {
  this->stack[this->stack_ptr] = this->consts[idx];
  this->stack_ptr += 1;
}

void R_ADD(R_vm *this) {
  R_box lhs = vm_pop(this);
  R_box rhs = vm_top(this);
  lhs.data.si += rhs.data.si;
  vm_set(this, &lhs);
}
