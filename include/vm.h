#ifndef VM_H
#define VM_H

#include "ternary.h"
#include <stddef.h>

#define STACK_SIZE 256

enum Opcode {
    OP_PUSH, OP_ADD, OP_MUL, OP_JMP, OP_COND_JMP, OP_HALT
};

// Run bytecode on the ternary VM.
// Phase 1 (MVP): Uses integer arithmetic for correctness.
// Later phases will switch to full balanced ternary (trit arrays).
void vm_run(unsigned char *bytecode, size_t len);

#endif
