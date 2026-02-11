# Architecture of Ternary Compiler

## High-Level Design (Non-Negotiable)
This compiler **MUST** follow a classic pipeline: Frontend -> IR -> Backend -> VM. **NO SHORTCUTS**. Built in C for bootstrapping. Target: Balanced ternary (-1,0,1) ops, per Huawei patent insights. Expand from expressions to full C (loops, functions, pointers) modularly.

### Components (Implement One at a Time, with Tests)
1. **Frontend**:
   - Lexer: Tokenize C source. **MUST** handle keywords, ints, ops. File: `src/lexer.c`. Test: 100% coverage for valid/invalid inputs.
   - Parser: Build AST. Use recursive descent. File: `src/parser.c`. **VERIFY**: Parse errors **MUST** log and halt.

2. **Intermediate Representation (IR)**:
   - AST nodes as structs (e.g., `struct Expr { trit val; ... }`). File: `src/ir.c`.
   - Optimizations: Constant folding in ternary. **MUST** add pass for each new feature.

3. **Backend**:
   - Codegen: Emit ternary bytecode. File: `src/codegen.c`. **NO FAKING**: Generate for each AST node, test round-trip.

4. **VM Simulator**:
   - Stack-based, trit ops. File: `vm/ternary_vm.c`. **MUST** support add/mul/min/max. Expand to control flow.

5. **Data Types**:
   - Trit: `signed char` (-1= N, 0=Z, 1=P). Words: Arrays of trits (e.g., 9-trit for small ints).
   - **SCHEMA**: All structs **MUST** be defined in `include/ternary.h`.

### Parallelism and Specialization
- **Agent Roles** (Assign via harness):
  - Parser Agent: Locks parsing tasks.
  - Codegen Agent: Locks backend tasks.
  - Tester Agent: Runs CI, fixes regressions.
  - Doc Agent: Updates .md files.
- **Sync**: Use git for merges. **RESOLVE CONFLICTS IMMEDIATELY** or revert.

### Scalability for seT5
- Phase 1: Expressions.
- Phase 2: Control flow.
- Phase 3: Pointers, seL4 stubs.
- **VERIFICATION**: Each phase **MUST** compile sample seL4 code to ternary.

**AGENT RULE**: Implement exactly as described. If deviating, log reason and revert if tests fail. Update TODO after each task.
