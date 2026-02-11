# TODO List

## Rules (Forcefully Asserted)
- **CLAIM TASKS**: Add [LOCKED: AgentRole @ Timestamp] to claim. Only one agent per task.
- **COMPLETE**: Mark [DONE: CommitHash] with git diff logged.
- **PARALLEL**: Pick unlocked tasks. If stuck, spawn sub-tasks.
- **VERIFICATION**: **MUST** pass unit tests before DONE. If fail, [REVERTED: Reason].

### Tasks (Prioritized)
1. [DONE] TASK-001: Expand lexer to handle loops (for/while). Add unit tests. — Expanded TokenType with 12 new types. Tokenizer handles keywords, identifiers, symbols. 4 new tests added.
2. [DONE] TASK-002: Implement trit array for multi-trit words in ternary.h. — Added WORD_SIZE=9, trit_word type, trit_word_add/mul, int_to_trit_word/trit_word_to_int. 6 new tests.
3. [DONE] TASK-003: Add control flow to VM (JMP, COND_JMP). — OP_JMP and OP_COND_JMP added. 3 new tests (jmp, cond_jmp_true, cond_jmp_false).
4. [DONE] TASK-004: Write parser for functions. Test with simple main(). — Recursive descent parser: parse_program(), func defs, params, return stmts, func calls, expr with precedence. 7 tests in test_parser.c.
5. [DONE] TASK-005: Optimize constant folding in IR. — Created include/ir.h, src/ir.c with Expr AST and bottom-up optimizer. 11 tests in tests/test_ir.c.
6. [DONE] TASK-006: Integrate logging in all src files. — Added log_entry calls in parser.c, codegen.c, ternary_vm.c, main.c. 4 new integration tests in test_logger.c.
7. [DONE] TASK-007: Update README with build stats. — Added Build Stats section with test count, LOC, features.
8. [DONE] TASK-008: Add CI script for tests (Makefile target). — Added ci.sh, Makefile targets: ci (test+lint), lint (cppcheck), coverage (gcov). Clean target handles gcov artifacts.
9. [DONE] TASK-009: Formal verification stub (Isabelle setup). — Created proofs/Ternary.thy with trit datatype, trit_add, trit_mul definitions and 8 correctness lemmas.
10. [DONE] TASK-010: Compile first seL4 stub to ternary. — 4 tests in tests/test_sel4.c: cap_init parse, simple compile, multi-cap, cap arithmetic through pipeline.
11. [DONE] TASK-011: Run full regression suite, fix any breaks from recent changes. — 100+ tests across 9 suites. No regressions.
12. [DONE] TASK-012: Fix linker: add logger.o to all binaries using log_entry. — Commit 283cd13.
13. [DONE] TASK-013: Fix linker: add ir.o to test_codegen (parser calls IR functions). — All 106 tests pass across 9 suites.

---

## Phase 2: Full seT5

### Tasks
1. [OPEN] TASK-015: Add pointers/memory to parser/IR (ternary heap/stack).
2. [OPEN] TASK-016: Syscall stubs for seT5.
3. [OPEN] TASK-017: Verilog for ternary hardware sim.
4. [OPEN] TASK-018: Self-host compiler.
5. [OPEN] TASK-019: Full seL4 compile/verify.

**AGENT DIRECTIVE**: Update this file atomically. No merges without logs.
