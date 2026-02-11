#ifndef VM_H
#define VM_H

#include "ternary.h"
#include <stddef.h>

#define STACK_SIZE 256
#define MEMORY_SIZE 729   /* 3^6 = 729 ternary-addressable cells */

enum Opcode {
    OP_PUSH, OP_ADD, OP_MUL, OP_JMP, OP_COND_JMP, OP_HALT,
    /* Phase 2: memory ops */
    OP_LOAD,    /* pop addr, push memory[addr] */
    OP_STORE,   /* pop value, pop addr, memory[addr] = value */
    OP_SUB,     /* pop b, pop a, push a-b */
    /* Phase 2: syscall */
    OP_SYSCALL  /* pop syscall number, dispatch */
};

// Run bytecode on the ternary VM.
// Phase 1 (MVP): Uses integer arithmetic for correctness.
// Later phases will switch to full balanced ternary (trit arrays).
void vm_run(unsigned char *bytecode, size_t len);

/* Phase 2: memory access for tests */
int vm_memory_read(int addr);
void vm_memory_write(int addr, int value);
void vm_memory_reset(void);

#endif
