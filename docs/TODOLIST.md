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
6. [OPEN] TASK-006: Integrate logging in all src files.
7. [DONE] TASK-007: Update README with build stats. — Added Build Stats section with test count, LOC, features.
8. [OPEN] TASK-008: Add CI script for tests (Makefile target). [NOTE: Partially done—expand to full pipeline.]
9. [OPEN] TASK-009: Formal verification stub (Isabelle setup).
10. [OPEN] TASK-010: Compile first seL4 stub to ternary.
11. [DONE] TASK-011: Run full regression suite, fix any breaks from recent changes. — 82 tests across 7 suites. No regressions.

**AGENT DIRECTIVE**: Update this file atomically. No merges without logs.
