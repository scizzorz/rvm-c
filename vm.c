#include "rain.h"

R_vm *vm_new() {
  GC_init();

  R_vm *this = GC_malloc(sizeof(R_vm));

  this->num_consts = 0;
  this->num_instrs = 0;
  this->num_strings = 0;

  this->instr_ptr = 0;
  this->stack_ptr = 0;
  this->stack_size = 10;
  this->scope_size = 10;

  this->consts = GC_malloc(sizeof(R_box));
  this->instrs = GC_malloc(sizeof(R_op));
  this->strings = GC_malloc(sizeof(char *));

  this->stack = GC_malloc(sizeof(R_box) * this->stack_size);
  this->scopes = GC_malloc(sizeof(R_box) * this->scope_size);

  return this;
}

bool vm_load(R_vm *this, FILE *fp) {
  int rv;

  R_header header;

  rv = fread(&header, sizeof(R_header), 1, fp);
  if(rv != 1) {
    fprintf(stderr, "Unable to read header\n");
    return false;
  }

  int prev_consts = this->num_consts;
  int prev_instrs = this->num_instrs;
  int prev_strings = this->num_strings;

  this->num_consts += header.num_consts;
  this->num_instrs += header.num_instrs;
  this->num_strings += header.num_strings;

  this->instr_ptr = 0;
  this->stack_ptr = 0;

  this->consts = GC_realloc(this->consts, sizeof(R_box) * this->num_consts);
  this->instrs = GC_realloc(this->instrs, sizeof(R_op) * this->num_instrs);
  this->strings = GC_realloc(this->strings, sizeof(char *) * this->num_strings);

  int len = 0;
  for(int i=prev_strings; i<this->num_strings; i++) {
    rv = fread(&len, sizeof(int), 1, fp);
    if(rv != 1) {
      fprintf(stderr, "Unable to read string %d length\n", i);
      return false;
    }

    this->strings[i] = GC_malloc_atomic(len + 1);
    rv = fread(this->strings[i], 1, len, fp);
    if(rv != len) {
      fprintf(stderr, "Unable to read string %d\n", i);
      return false;
    }

    this->strings[i][len] = 0;
  }

  rv = fread(this->consts + prev_consts, sizeof(R_box), header.num_consts, fp);
  if(rv != header.num_consts) {
    fprintf(stderr, "Unable to read constants\n");
    return false;
  }

  rv = fread(this->instrs + prev_instrs, sizeof(R_op), header.num_instrs, fp);
  if(rv != header.num_instrs) {
    fprintf(stderr, "Unable to read instructions\n");
    return false;
  }

  // adjust string const pointers
  for(int i=prev_consts; i<this->num_consts; i++) {
    if(R_TYPE_IS(&this->consts[i], STR)) {
      this->consts[i].str = this->strings[this->consts[i].i64 + prev_strings];
    }
  }

  // adjust instruction indices
  for(int i=prev_instrs; i<this->num_instrs; i++) {
    switch(R_OP(&this->instrs[i])) {
      case PUSH_CONST:
        this->instrs[i].u32 += (prev_consts << 8);
        break;
      case PUSH_SCOPE:
        this->instrs[i].u32 += (this->scope_ptr << 8);
        break;
    }
  }

  return true;
}

bool vm_exec(R_vm *this, R_op *instr) {
  if(R_OP(instr) < NUM_INSTRS) {
    R_INSTR_TABLE[R_OP(instr)](this, instr);
    return true;
  }

  printf("Unknown instruction %02x %06x\n", R_OP(instr), R_UI(instr));
  return false;
}

bool vm_step(R_vm *this) {
  if(this->instr_ptr < this->num_instrs) {
    if(vm_exec(this, this->instrs + this->instr_ptr)) {
      this->instr_ptr += 1;
      return true;
    }
  }

  return false;
}

bool vm_run(R_vm *this) {
  while(this->instr_ptr < this->num_instrs) {
    vm_step(this);
  }

  return true;
}

void vm_dump(R_vm *this) {
  printf("Constants (%d):\n", this->num_consts);
  for(int i=0; i<this->num_consts; i++) {
    printf("   ");
    R_box_print(this->consts + i);
  }

  printf("Instructions (%d):\n", this->num_instrs);
  for(int i=0; i<this->num_instrs; i++) {
    printf(i == this->instr_ptr ? "-> " : "   ");
    R_op_print(this->instrs + i);
  }

  printf("Stack (%d / %d):\n", this->stack_ptr, this->stack_size);
  for(int i=0; i<this->stack_size; i++) {
    if(i + 1 > this->stack_ptr) {
      printf("       ");
    }
    else if(i + 1 == this->stack_ptr) {
      printf("[% 2d] > ", i);
    }
    else {
      printf("[% 2d]   ", i);
    }

    R_box_print(this->stack + i);
  }
}

R_box vm_pop(R_vm *this) {
  this->stack_ptr -= 1;
  return this->stack[this->stack_ptr];
}

R_box vm_top(R_vm *this) {
  return this->stack[this->stack_ptr - 1];
}

void vm_set(R_vm *this, R_box *val) {
  this->stack[this->stack_ptr - 1] = *val;
}

void vm_new_scope(R_vm *this) {
  if(this->scope_ptr >= this->scope_size) {
    this->scope_size *= 2;
    this->scopes = GC_realloc(this->scopes, sizeof(R_box) * this->scope_size);
  }

  R_set_table(&this->scopes[this->scope_ptr]);
  this->scope_ptr += 1;
}

R_box *vm_alloc(R_vm *this) {
  if(this->stack_ptr >= this->stack_size) {
    this->stack_size *= 2;
    this->stack = GC_realloc(this->stack, sizeof(R_box) * this->stack_size);
  }

  R_set_null(&this->stack[this->stack_ptr]);
  this->stack_ptr += 1;

  return &this->stack[this->stack_ptr - 1];
}

R_box *vm_push(R_vm *this, R_box *val) {
  if(this->stack_ptr >= this->stack_size) {
    this->stack_size *= 2;
    this->stack = GC_realloc(this->stack, sizeof(R_box) * this->stack_size);
  }

  this->stack[this->stack_ptr] = *val;
  this->stack_ptr += 1;

  return &this->stack[this->stack_ptr - 1];
}
