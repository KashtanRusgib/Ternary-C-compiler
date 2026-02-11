#include <stdio.h>
#include "../include/vm.h"
#include "../include/logger.h"

// Phase 1 (MVP): Use int stack for correct decimal results.
// Phase 2+: Switch to trit arrays with trit_add/trit_mul.
static int stack[STACK_SIZE];
static int sp = 0;  // Stack pointer

/* Phase 2: ternary-addressable memory (729 cells = 3^6) */
static int memory[MEMORY_SIZE];

static void push(int val) {
    if (sp < STACK_SIZE) stack[sp++] = val;
}

static int pop(void) {
    return (sp > 0) ? stack[--sp] : 0;
}

/* Public memory access for tests */
int vm_memory_read(int addr) {
    if (addr >= 0 && addr < MEMORY_SIZE) return memory[addr];
    return 0;
}

void vm_memory_write(int addr, int value) {
    if (addr >= 0 && addr < MEMORY_SIZE) memory[addr] = value;
}

void vm_memory_reset(void) {
    for (int i = 0; i < MEMORY_SIZE; i++) memory[i] = 0;
    sp = 0;
}

void vm_run(unsigned char *bytecode, size_t len) {
    sp = 0;  // Reset stack
    LOG_DEBUG_MSG("VM", "TASK-006", "vm_run entered");

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
            case OP_SUB: {
                int b = pop(), a = pop();
                push(a - b);
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
            case OP_LOAD: {
                int addr = pop();
                if (addr >= 0 && addr < MEMORY_SIZE)
                    push(memory[addr]);
                else
                    push(0);
                break;
            }
            case OP_STORE: {
                int val = pop();
                int addr = pop();
                if (addr >= 0 && addr < MEMORY_SIZE)
                    memory[addr] = val;
                break;
            }
            case OP_SYSCALL: {
                int sysno = pop();
                LOG_DEBUG_MSG("VM", "TASK-016", "syscall dispatched");
                /* Syscall dispatch — see seT5 stubs */
                switch (sysno) {
                    case 0: /* t_exit */
                        LOG_DEBUG_MSG("VM", "TASK-016", "t_exit");
                        return;
                    case 1: { /* t_write: pop fd, pop addr, pop len */
                        int fd = pop();
                        int addr = pop();
                        int slen = pop();
                        (void)fd; (void)addr; (void)slen;
                        push(slen); /* return bytes written */
                        break;
                    }
                    case 2: { /* t_read: pop fd, pop addr, pop len */
                        int fd = pop();
                        int addr = pop();
                        int slen = pop();
                        (void)fd; (void)addr; (void)slen;
                        push(0); /* stub: return 0 bytes read */
                        break;
                    }
                    case 3: { /* t_mmap: pop size -> return base addr */
                        int sz = pop();
                        /* Simple bump allocator from top of memory */
                        static int heap_top = MEMORY_SIZE / 2;
                        int base = heap_top;
                        heap_top += sz;
                        if (heap_top > MEMORY_SIZE) heap_top = MEMORY_SIZE;
                        push(base);
                        break;
                    }
                    case 4: { /* t_cap_send: pop cap, pop msg */
                        int cap = pop();
                        int msg = pop();
                        (void)cap; (void)msg;
                        push(0); /* success */
                        break;
                    }
                    case 5: { /* t_cap_recv: pop cap -> push msg */
                        int cap = pop();
                        (void)cap;
                        push(42); /* stub: always receive 42 */
                        break;
                    }
                    default:
                        push(-1); /* unknown syscall */
                        break;
                }
                break;
            }
            case OP_HALT: printf("Result: %d\n", pop()); LOG_DEBUG_MSG("VM", "TASK-006", "vm_run HALT"); return;
        }
    }
}
