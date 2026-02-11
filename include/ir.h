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
    NODE_CONST,   /* Integer constant */
    NODE_VAR,     /* Variable reference */
    NODE_BINOP    /* Binary operation */
} NodeType;

/* Binary operator types */
typedef enum {
    OP_IR_ADD,
    OP_IR_MUL
} OpType;

/* Expression AST node */
typedef struct Expr {
    NodeType type;
    int val;              /* For NODE_CONST */
    char *name;           /* For NODE_VAR */
    OpType op;            /* For NODE_BINOP */
    struct Expr *left;    /* For NODE_BINOP */
    struct Expr *right;   /* For NODE_BINOP */
} Expr;

/* Create a constant node */
Expr *create_const(int val);

/* Create a variable node */
Expr *create_var(const char *name);

/* Create a binary operation node */
Expr *create_binop(OpType op, Expr *left, Expr *right);

/* Optimize: constant folding pass (recursive) */
void optimize(Expr *e);

/* Free an expression tree */
void expr_free(Expr *e);

#endif /* IR_H */
