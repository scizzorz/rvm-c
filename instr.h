#ifndef R_INSTR_H
#define R_INSTR_H

#include "rain.h"

// op codes
#define LOAD_CONST 0
#define PRINT_ITEM 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define NEG 6
#define NOT 7
#define LT  8
#define GT  9
#define LE  10
#define GE  11
#define EQ  12
#define NE  13

void R_PRINT_ITEM(R_vm *this);
void R_LOAD_CONST(R_vm *this, char idx);
void R_ADD(R_vm *this);
//void R_SUB(R_vm *this);
//void R_MUL(R_vm *this);
//void R_DIV(R_vm *this);
//void R_NEG(R_vm *this);
//void R_NOT(R_vm *this);
//void R_LT(R_vm *this);
//void R_GT(R_vm *this);
//void R_LE(R_vm *this);
//void R_GE(R_vm *this);
//void R_EQ(R_vm *this);
//void R_NE(R_vm *this);

#endif
