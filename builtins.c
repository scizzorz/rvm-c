#include "rain.h"

#define __USE_GNU
#include <dlfcn.h>

void R_builtin_load(R_vm *vm) {
  R_box name = vm_pop(vm);
  R_box lib = vm_top(vm);
  R_box *top = &vm->stack[vm->stack_ptr - 1];

  if(R_TYPE_IS(&name, STR) && R_TYPE_IS(&lib, STR)) {
    void *handle = dlopen(lib.str, RTLD_LAZY | RTLD_GLOBAL);
    void *func = dlsym(handle, name.str);
    if(func != NULL) {
      R_set_cfunc(top, func);
      return;
    }
  }

  R_set_null(top);
}

void R_builtin_print(R_vm *vm) {
  R_box pop = vm_pop(vm);
  R_box_print(&pop);
}

void R_builtin_scope(R_vm *vm) {
  // push frame -2 scope because we don't want to push this function call's
  // scope, we want its outer scope
  vm_push(vm, &vm->frames[vm->frame_ptr - 2].scope);
}

void R_builtin_meta(R_vm *vm) {
  R_box *top = &vm->stack[vm->stack_ptr - 1];
  if(R_has_meta(top)) {
    *top = *(top->meta);
  }
  else {
    R_set_null(top);
  }
}
