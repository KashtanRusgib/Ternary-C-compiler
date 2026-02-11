/*
 * ir.h - Intermediate Representation for the ternary compiler
 *
 * AST node types for expressions. Used by the optimizer pass
 * (constant folding) between parsing and codegen.
 *
 * Phase 1 (MVP): int values for correctness.
 * Phase 2+: Switch to trit arrays.
 */

#ifndef IR_H
#define IR_H

#include <stdlib.h>
#include <string.h>

/* AST node types */
typedef enum {
    NODE_CONST,     /* Integer constant */
    NODE_VAR,       /* Variable reference */
    NODE_BINOP,     /* Binary operation */
    NODE_FUNC_DEF,  /* Function definition */
    NODE_FUNC_CALL, /* Function call */
    NODE_RETURN,    /* Return statement */
    NODE_PROGRAM,   /* Top-level program (list of functions) */
    NODE_DEREF,     /* Pointer dereference: *expr */
    NODE_ADDR_OF,   /* Address-of: &var */
    NODE_ASSIGN,    /* Assignment: lhs = rhs */
    NODE_VAR_DECL   /* Variable declaration: int x = expr */
} NodeType;

/* Binary operator types */
typedef enum {
    OP_IR_ADD,
    OP_IR_MUL,
    OP_IR_SUB
} OpType;

/* Expression AST node */
typedef struct Expr {
    NodeType type;
    int val;              /* For NODE_CONST */
    char *name;           /* For NODE_VAR, NODE_FUNC_DEF, NODE_FUNC_CALL */
    OpType op;            /* For NODE_BINOP */
    struct Expr *left;    /* For NODE_BINOP left operand / NODE_RETURN expr */
    struct Expr *right;   /* For NODE_BINOP right operand */
    struct Expr *body;    /* For NODE_FUNC_DEF: function body */
    struct Expr **params; /* FUNC_DEF: param list / FUNC_CALL: args / PROGRAM: funcs */
    int param_count;      /* Number of params / args / funcs */
} Expr;

/* Create a constant node */
Expr *create_const(int val);

/* Create a variable node */
Expr *create_var(const char *name);

/* Create a binary operation node */
Expr *create_binop(OpType op, Expr *left, Expr *right);

/* Optimize: constant folding pass (recursive) */
void optimize(Expr *e);

/* Create a function definition node */
Expr *create_func_def(const char *name, Expr **params, int param_count, Expr *body);

/* Create a function call node */
Expr *create_func_call(const char *name, Expr **args, int arg_count);

/* Create a return statement node */
Expr *create_return(Expr *expr);

/* Create a program node (container for function definitions) */
Expr *create_program(void);

/* Add a function definition to a program node */
void program_add_func(Expr *prog, Expr *func);

/* Create a dereference node (*expr) */
Expr *create_deref(Expr *expr);

/* Create an address-of node (&var) */
Expr *create_addr_of(Expr *var);

/* Create an assignment node (lhs = rhs) */
Expr *create_assign(Expr *lhs, Expr *rhs);

/* Create a variable declaration node (int x = expr) */
Expr *create_var_decl(const char *name, Expr *init);

/* Free an expression tree */
void expr_free(Expr *e);

#endif /* IR_H */
