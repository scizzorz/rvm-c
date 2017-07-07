#include "rain.h"
#define __USE_GNU
#include <dlfcn.h>

void (*R_INSTR_TABLE[NUM_INSTRS])(R_vm *, R_op *) = {
  R_PUSH_CONST,
  R_PRINT_ITEM,
  R_UN_OP,
  R_BIN_OP,
  R_CMP,
  R_JUMP,
  R_JUMPIF,
  R_DUP,
  R_POP,
  R_LOAD,
  R_CALL,
};


const char *R_INSTR_NAMES[NUM_INSTRS] = {
  "PUSH_CONST",
  "PRINT_ITEM",
  "UN_OP",
  "BIN_OP",
  "CMP",
  "JUMP",
  "JUMPIF",
  "DUP",
  "POP",
  "LOAD",
  "CALL",
};

void R_PRINT_ITEM(R_vm *vm, R_op *instr) {
  R_box *val = vm->stack + vm->stack_ptr - 1;
  R_box_print(val);
}


void R_PUSH_CONST(R_vm *vm, R_op *instr) {
  vm->stack[vm->stack_ptr] = vm->consts[instr->args[0]];
  vm->stack_ptr += 1;
}

void R_UN_OP(R_vm *vm, R_op *instr) {
}

void R_BIN_OP(R_vm *vm, R_op *instr) {
  R_box lhs = vm_pop(vm);
  R_box rhs = vm_top(vm);
  R_box *top = vm->stack + vm->stack_ptr - 1;
  bool do_float = false;
  double lhs_f, rhs_f;

  if(TYPE_IS(&lhs, INT) && TYPE_IS(&rhs, INT)) {
    switch(instr->args[0]) {
      case BIN_ADD: R_set_int(top, lhs.data.si + rhs.data.si); break;
      case BIN_SUB: R_set_int(top, lhs.data.si - rhs.data.si); break;
      case BIN_MUL: R_set_int(top, lhs.data.si * rhs.data.si); break;
      case BIN_DIV: R_set_int(top, lhs.data.si / rhs.data.si); break;
    }
    return;
  }
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, INT)) {
    lhs_f = lhs.data.f;
    rhs_f = (double)rhs.data.si;
    do_float = true;
  }
  else if(TYPE_IS(&lhs, INT) && TYPE_IS(&rhs, FLOAT)) {
    lhs_f = (double)lhs.data.si;
    rhs_f = rhs.data.f;
    do_float = true;
  }
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, FLOAT)) {
    lhs_f = lhs.data.f;
    rhs_f = rhs.data.f;
    do_float = true;
  }

  if(do_float) {
    switch(instr->args[0]) {
      case BIN_ADD: R_set_float(top, lhs_f + rhs_f); break;
      case BIN_SUB: R_set_float(top, lhs_f - rhs_f); break;
      case BIN_MUL: R_set_float(top, lhs_f * rhs_f); break;
      case BIN_DIV: R_set_float(top, lhs_f / rhs_f); break;
    }
    return;
  }

  R_set_null(top);
}

void R_CMP(R_vm *vm, R_op *instr) {
  R_box lhs = vm_pop(vm);
  R_box rhs = vm_top(vm);
  R_box *top = vm->stack + vm->stack_ptr - 1;

  if(lhs.type != rhs.type) {
    R_set_bool(top, false);
    return;
  }

  if(TYPE_IS(&lhs, INT) && TYPE_IS(&rhs, INT)) {
    switch(instr->args[0]) {
      case CMP_LT: R_set_bool(top, lhs.data.si < rhs.data.si); break;
      case CMP_LE: R_set_bool(top, lhs.data.si <= rhs.data.si); break;
      case CMP_GT: R_set_bool(top, lhs.data.si > rhs.data.si); break;
      case CMP_GE: R_set_bool(top, lhs.data.si >= rhs.data.si); break;
      case CMP_EQ: R_set_bool(top, lhs.data.si == rhs.data.si); break;
      case CMP_NE: R_set_bool(top, lhs.data.si != rhs.data.si); break;
    }
    return;
  }

  // TODO: add int/float and float/int comparisons?
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, FLOAT)) {
    switch(instr->args[0]) {
      case CMP_LT: R_set_bool(top, lhs.data.f < rhs.data.f); break;
      case CMP_LE: R_set_bool(top, lhs.data.f <= rhs.data.f); break;
      case CMP_GT: R_set_bool(top, lhs.data.f > rhs.data.f); break;
      case CMP_GE: R_set_bool(top, lhs.data.f >= rhs.data.f); break;
      case CMP_EQ: R_set_bool(top, lhs.data.f == rhs.data.f); break;
      case CMP_NE: R_set_bool(top, lhs.data.f != rhs.data.f); break;
    }
    return;
  }

  else if(TYPE_IS(&lhs, BOOL) && TYPE_IS(&rhs, BOOL)) {
    switch(instr->args[0]) {
      case CMP_LT: R_set_bool(top, lhs.data.ui < rhs.data.ui); break;
      case CMP_LE: R_set_bool(top, lhs.data.ui <= rhs.data.ui); break;
      case CMP_GT: R_set_bool(top, lhs.data.ui > rhs.data.ui); break;
      case CMP_GE: R_set_bool(top, lhs.data.ui >= rhs.data.ui); break;
      case CMP_EQ: R_set_bool(top, lhs.data.ui == rhs.data.ui); break;
      case CMP_NE: R_set_bool(top, lhs.data.ui != rhs.data.ui); break;
    }
    return;
  }

  R_set_null(top);
}


void R_JUMP(R_vm *vm, R_op *instr) {
}


void R_JUMPIF(R_vm *vm, R_op *instr) {
  R_box top = vm_pop(vm);

  if(top.type != TYPE_NULL && !(top.type == TYPE_BOOL && top.data.si == 0)) {
    vm->instr_ptr += instr->args[0];
    vm->instr_ptr -= instr->args[1];
    // TODO: this is awful ^
    // TODO: what if IP goes out of bounds?
  }
}


void R_DUP(R_vm *vm, R_op *instr) {
  R_box *top = vm->stack + vm->stack_ptr - 1;
  vm_push(vm, top);
}


void R_POP(R_vm *vm, R_op *instr) {
  vm_pop(vm);
}

void R_LOAD(R_vm *vm, R_op *instr) {
  R_box *top = vm->stack + vm->stack_ptr - 1;
  if(TYPE_IS(top, STR)) {
    void *ptr = dlopen(top->data.s, RTLD_LAZY | RTLD_GLOBAL);
    R_set_cdata(top, ptr);
    return;
  }

  R_set_null(top);
}

void R_CALL(R_vm *vm, R_op *instr) {
  R_box name = vm_pop(vm);
  R_box *top = vm->stack + vm->stack_ptr - 1;
  if(TYPE_IS(&name, STR) && TYPE_IS(top, CDATA)) {
    void *ptr = dlsym(top->data.p, name.data.s);
    void (*callable)(R_vm *) = (void (*)(R_vm *))ptr;
    callable(vm);
  }
}
