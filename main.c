#include "rain.h"
#include <stdio.h>

int main(int argv, char **argc) {
  if(argv < 2) {
    fprintf(stderr, "Usage: %s FILE\n", argc[0]);
    return 1;
  }

  FILE *fp = fopen(argc[1], "rb");

  if(fp == NULL) {
    fprintf(stderr, "Unable to open file %s\n", argc[1]);
    return 1;
  }

  R_vm *this = vm_new();
  vm_load(this, fp);

  if(this == NULL) {
    fprintf(stderr, "Unable to load VM\n");
    return 1;
  }

  vm_dump(this);

  printf("--------\n");
  vm_run(this);
  printf("--------\n");

  vm_dump(this);

  return 0;
}
