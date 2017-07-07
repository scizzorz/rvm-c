#include "rain.h"

void (*R_INSTR_TABLE[NUM_INSTRS])(R_vm *, R_op *) = {
  R_PUSH_CONST,
  R_PRINT_ITEM,
  R_ADD,
  R_SUB,
  R_MUL,
  R_DIV,
  R_NEG,
  R_NOT,
  R_LT,
  R_GT,
  R_LE,
  R_GE,
  R_EQ,
  R_NE,
  R_JUMP,
  R_JUMPIF,
  R_DUP,
  R_POP,
};


const char *R_INSTR_NAMES[NUM_INSTRS] = {
  "PUSH_CONST",
  "PRINT_ITEM",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "NEG",
  "NOT",
  "LT",
  "GT",
  "LE",
  "GE",
  "EQ",
  "NE",
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
  vm->stack[vm->stack_ptr] = vm->consts[instr->args[0]];
  vm->stack_ptr += 1;
}


void R_ADD(R_vm *vm, R_op *instr) {
  R_box lhs = vm_pop(vm);
  R_box rhs = vm_top(vm);
  lhs.data.si += rhs.data.si;
  vm_set(vm, &lhs);
}


void R_SUB(R_vm *vm, R_op *instr) {
}


void R_MUL(R_vm *vm, R_op *instr) {
}


void R_DIV(R_vm *vm, R_op *instr) {
}


void R_NEG(R_vm *vm, R_op *instr) {
}


void R_NOT(R_vm *vm, R_op *instr) {
}


void R_LT(R_vm *vm, R_op *instr) {
}


void R_GT(R_vm *vm, R_op *instr) {
}


void R_LE(R_vm *vm, R_op *instr) {
}


void R_GE(R_vm *vm, R_op *instr) {
}


void R_EQ(R_vm *vm, R_op *instr) {
}


void R_NE(R_vm *vm, R_op *instr) {
  R_box lhs = vm_pop(vm);
  R_box rhs = vm_top(vm);
  lhs.type = TYPE_BOOL;
  lhs.data.si = lhs.data.si != rhs.data.si;
  vm_set(vm, &lhs);
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
