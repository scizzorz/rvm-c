#ifndef R_INSTR_H
#define R_INSTR_H

#include "rain.h"

// op codes
#define PUSH_CONST 0x00
#define PRINT_ITEM 0x01
#define ADD        0x02
#define SUB        0x03
#define MUL        0x04
#define DIV        0x05
#define NEG        0x06
#define NOT        0x07
#define LT         0x08
#define GT         0x09
#define LE         0x0A
#define GE         0x0B
#define EQ         0x0C
#define NE         0x0D
#define JUMP       0x0E
#define JUMPIF     0x0F
#define DUP        0x10
#define POP        0x11

#define NUM_INSTRS 0x12

void R_PUSH_CONST(R_vm *vm, R_op *instr);
void R_PRINT_ITEM(R_vm *vm, R_op *instr);
void R_ADD(R_vm *vm, R_op *instr);
void R_SUB(R_vm *vm, R_op *instr);
void R_MUL(R_vm *vm, R_op *instr);
void R_DIV(R_vm *vm, R_op *instr);
void R_NEG(R_vm *vm, R_op *instr);
void R_NOT(R_vm *vm, R_op *instr);
void R_LT(R_vm *vm, R_op *instr);
void R_GT(R_vm *vm, R_op *instr);
void R_LE(R_vm *vm, R_op *instr);
void R_GE(R_vm *vm, R_op *instr);
void R_EQ(R_vm *vm, R_op *instr);
void R_NE(R_vm *vm, R_op *instr);
void R_JUMP(R_vm *vm, R_op *instr);
void R_JUMPIF(R_vm *vm, R_op *instr);
void R_DUP(R_vm *vm, R_op *instr);
void R_POP(R_vm *vm, R_op *instr);

void (*R_INSTR_TABLE[NUM_INSTRS])(R_vm *, R_op *);

const char *R_INSTR_NAMES[NUM_INSTRS];

#endif
