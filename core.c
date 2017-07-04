#include "rain.h"
#include <gc.h>
#include <stdbool.h>
#include <stdio.h>

void box_print(R_box *val) {
  switch(val->type) {
    case TYPE_NULL:
      printf("null\n");
      break;
    case TYPE_INT:
      printf("%ld\n", val->data.si);
      break;
    case TYPE_FLOAT:
      printf("%f\n", val->data.f);
      break;
    case TYPE_BOOL:
      printf(val->data.si != 0 ? "true\n" : "false\n");
      break;
    case TYPE_STR:
      printf("%s\n", val->data.s);
      break;
    case TYPE_TABLE:
      printf("table 0x%08lx\n", (unsigned long)val->data.p);
      break;
    case TYPE_FUNC:
      printf("func 0x%08lx\n", (unsigned long)val->data.p);
      break;
    case TYPE_CDATA:
      printf("cdata 0x%08lx\n", (unsigned long)val->data.p);
      break;
    default:
      printf("unknown\n");
  }
}

void op_print(R_op *instr) {
  printf("Instr<%d: %d %d %d>\n", instr->op, instr->args[0], instr->args[1], instr->args[2]);
}
