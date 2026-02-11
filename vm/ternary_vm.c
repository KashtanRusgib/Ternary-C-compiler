#include <stdio.h>
#include "../include/vm.h"

// Phase 1 (MVP): Use int stack for correct decimal results.
// Phase 2+: Switch to trit arrays with trit_add/trit_mul.
static int stack[STACK_SIZE];
static int sp = 0;  // Stack pointer

static void push(int val) {
    if (sp < STACK_SIZE) stack[sp++] = val;
}

static int pop(void) {
    return (sp > 0) ? stack[--sp] : 0;
}

void vm_run(unsigned char *bytecode, size_t len) {
    sp = 0;  // Reset stack

    for (size_t pc = 0; pc < len; ) {
        switch (bytecode[pc++]) {
            case OP_PUSH: push((int)(signed char)bytecode[pc++]); break;
            case OP_ADD: {
                int b = pop(), a = pop();
                push(a + b);
                break;
            }
            case OP_MUL: {
                int b = pop(), a = pop();
                push(a * b);
                break;
            }
            case OP_JMP:
                pc = (size_t)bytecode[pc];
                break;
            case OP_COND_JMP: {
                int cond = pop();
                if (cond == 0) {
                    pc = (size_t)bytecode[pc];
                } else {
                    pc++;  /* skip address byte */
                }
                break;
            }
            case OP_HALT: printf("Result: %d\n", pop()); return;
        }
    }
}
