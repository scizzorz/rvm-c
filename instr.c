#include "rain.h"

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
};

void R_PRINT_ITEM(R_vm *vm, R_op *instr) {
  R_box *val = vm->stack + vm->stack_ptr - 1;
  R_box_print(val);
}


void R_PUSH_CONST(R_vm *vm, R_op *instr) {
  vm->stack[vm->stack_ptr] = vm->consts[R_UI(instr)];
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
    switch(R_UI(instr)) {
      case BIN_ADD: R_set_int(top, lhs.i64 + rhs.i64); break;
      case BIN_SUB: R_set_int(top, lhs.i64 - rhs.i64); break;
      case BIN_MUL: R_set_int(top, lhs.i64 * rhs.i64); break;
      case BIN_DIV: R_set_int(top, lhs.i64 / rhs.i64); break;
    }
    return;
  }
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, INT)) {
    lhs_f = lhs.f64;
    rhs_f = (double)rhs.i64;
    do_float = true;
  }
  else if(TYPE_IS(&lhs, INT) && TYPE_IS(&rhs, FLOAT)) {
    lhs_f = (double)lhs.i64;
    rhs_f = rhs.f64;
    do_float = true;
  }
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, FLOAT)) {
    lhs_f = lhs.f64;
    rhs_f = rhs.f64;
    do_float = true;
  }

  if(do_float) {
    switch(R_UI(instr)) {
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
    switch(R_UI(instr)) {
      case CMP_LT: R_set_bool(top, lhs.i64 < rhs.i64); break;
      case CMP_LE: R_set_bool(top, lhs.i64 <= rhs.i64); break;
      case CMP_GT: R_set_bool(top, lhs.i64 > rhs.i64); break;
      case CMP_GE: R_set_bool(top, lhs.i64 >= rhs.i64); break;
      case CMP_EQ: R_set_bool(top, lhs.i64 == rhs.i64); break;
      case CMP_NE: R_set_bool(top, lhs.i64 != rhs.i64); break;
    }
    return;
  }

  // TODO: add int/float and float/int comparisons?
  else if(TYPE_IS(&lhs, FLOAT) && TYPE_IS(&rhs, FLOAT)) {
    switch(R_UI(instr)) {
      case CMP_LT: R_set_bool(top, lhs.f64 < rhs.f64); break;
      case CMP_LE: R_set_bool(top, lhs.f64 <= rhs.f64); break;
      case CMP_GT: R_set_bool(top, lhs.f64 > rhs.f64); break;
      case CMP_GE: R_set_bool(top, lhs.f64 >= rhs.f64); break;
      case CMP_EQ: R_set_bool(top, lhs.f64 == rhs.f64); break;
      case CMP_NE: R_set_bool(top, lhs.f64 != rhs.f64); break;
    }
    return;
  }

  else if(TYPE_IS(&lhs, BOOL) && TYPE_IS(&rhs, BOOL)) {
    switch(R_UI(instr)) {
      case CMP_LT: R_set_bool(top, lhs.u64 < rhs.u64); break;
      case CMP_LE: R_set_bool(top, lhs.u64 <= rhs.u64); break;
      case CMP_GT: R_set_bool(top, lhs.u64 > rhs.u64); break;
      case CMP_GE: R_set_bool(top, lhs.u64 >= rhs.u64); break;
      case CMP_EQ: R_set_bool(top, lhs.u64 == rhs.u64); break;
      case CMP_NE: R_set_bool(top, lhs.u64 != rhs.u64); break;
    }
    return;
  }

  R_set_null(top);
}


void R_JUMP(R_vm *vm, R_op *instr) {
}


void R_JUMPIF(R_vm *vm, R_op *instr) {
  R_box top = vm_pop(vm);

  if(top.type != TYPE_NULL && !(top.type == TYPE_BOOL && top.i64 == 0)) {
    vm->instr_ptr += R_SI(instr);
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
