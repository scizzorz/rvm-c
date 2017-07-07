#include "rain.h"
#include <gc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void R_box_print(R_box *val) {
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

void R_set_box(R_box *ret, R_box *from) {
  ret->type = from->type;
  ret->data.ui = from->data.ui;
  ret->size = from->size;
  ret->meta = from->meta;
}

void R_set_null(R_box *ret) {
  ret->type = TYPE_NULL;
  ret->data.ui = 0;
  ret->size = 0;
}

void R_set_int(R_box *ret, signed long si) {
  ret->type = TYPE_INT;
  ret->data.si = si;
  ret->size = 0;
}

void R_set_float(R_box *ret, double f) {
  ret->type = TYPE_FLOAT;
  ret->data.f = f;
  ret->size = 0;
}

void R_set_bool(R_box *ret, bool v) {
  ret->type = TYPE_BOOL;
  ret->data.ui = v;
  ret->size = 0;
}

void R_set_str(R_box *ret, char* s) {
  ret->type = TYPE_STR;
  ret->data.s = s;
  ret->size = strlen(s);
}

void R_set_strcpy(R_box *ret, const char *s) {
  int size = strlen(s);

  ret->type = TYPE_STR;
  ret->data.s = GC_malloc(size + 1);
  ret->size = size;

  memcpy(ret->data.s, s, size);
  ret->data.s[size] = 0;
}

void R_set_table(R_box *ret) {
  ret->type = TYPE_TABLE;
  ret->data.ui = 0;
  ret->size = 0;
}

void R_set_func(R_box *ret, void *p, int num_args) {
  ret->type = TYPE_FUNC;
  ret->data.p = p;
  ret->size = num_args;
}

void R_set_cdata(R_box *ret, void *p) {
  ret->type = TYPE_CDATA;
  ret->data.p = p;
  ret->size = 0;
}

void R_set_meta(R_box *val, R_box *meta) {
  val->meta = meta;
}

void R_op_print(R_op *instr) {
  printf("%s (%d | %d)\n", R_INSTR_NAMES[R_OP(instr)], R_SI(instr), R_UI(instr));
}
