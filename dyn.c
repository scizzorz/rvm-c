#include <stdio.h>
#include "rain.h"

void print_hello(R_vm *vm) {
  printf("Hello! The VM is at 0x%08lx\n", (unsigned long)vm);
}
