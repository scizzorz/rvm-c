#include "rain.h"

void R_builtin_print(R_vm *vm) {
  R_box pop = vm_pop(vm);
  R_box_print(&pop);
}
