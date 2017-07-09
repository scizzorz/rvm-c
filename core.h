#ifndef R_CORE_H
#define R_CORE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// types
#define TYPE_NULL  0
#define TYPE_INT   1
#define TYPE_FLOAT 2
#define TYPE_BOOL  3
#define TYPE_STR   4
#define TYPE_TABLE 5
#define TYPE_FUNC  6
#define TYPE_CDATA 7

#define TYPE_IS(x, t) ((x)->type == TYPE_##t)
#define TYPE_ISNT(x, t) ((x)->type != TYPE_##t)

typedef struct R_box {
  char type;
  int32_t size;
  union {
    uint64_t u64;
    int64_t i64;
    double f64;
    char *str;
    void *ptr;
  };
  struct R_box *meta;
} R_box;

#define R_OP(x) ((x->i32 & 0xFF))
#define R_SI(x) ((x->i32 >> 8))
#define R_UI(x) ((x->u32 >> 8))

typedef union {
  uint32_t u32;
  int32_t i32;
} R_op;

typedef struct R_item {
  uint64_t hash;
  R_box key;
  R_box val;
} R_item;

typedef struct R_table {
  uint32_t cur;
  uint32_t max;
  R_item **items;
} R_table;

void R_box_print(R_box *val);
void R_op_print(R_op *instr);

void R_set_box(R_box *ret, R_box *from);
void R_set_null(R_box *ret);
void R_set_int(R_box *ret, signed long si);
void R_set_float(R_box *ret, double f);
void R_set_bool(R_box *ret, bool v);
void R_set_str(R_box *ret, char* s);
void R_set_strcpy(R_box *ret, const char *s);
void R_set_table(R_box *ret);
void R_set_func(R_box *ret, void *p, int num_args);
void R_set_cdata(R_box *ret, void *p);
void R_set_meta(R_box *val, R_box *meta);

#endif
