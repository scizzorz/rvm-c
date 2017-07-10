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
  if(this == NULL) {
    fprintf(stderr, "Unable to create VM\n");
    return 1;
  }

  if(!vm_load(this, fp)) {
    fprintf(stderr, "Unable to load bytecode\n");
    return 1;
  }

  vm_dump(this);

  printf("--------\n");
  vm_run(this);
  printf("--------\n");

  vm_dump(this);

  return 0;
}
