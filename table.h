#ifndef R_TABLE_H
#define R_TABLE_H

#include "rain.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

uint64_t R_hash(R_box *val);
bool R_hash_eq(R_box *lhs, R_box *rhs);
void R_table_set(R_box *table, R_box *key, R_box *value);
void R_table_set_aux(R_box *table, R_box *key, R_box *value, R_item *item);
R_box *R_table_get(R_box *table, R_box *key);

#endif
