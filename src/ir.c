/*
 * ir.c - Intermediate Representation and optimizer
 *
 * Implements AST construction and constant folding optimization.
 * Phase 1 (MVP): Uses int values for correctness.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ir.h"

Expr *create_const(int val) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_CONST;
    e->val = val;
    e->name = NULL;
    e->left = NULL;
    e->right = NULL;
    return e;
}

Expr *create_var(const char *name) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_VAR;
    e->val = 0;
    e->name = strdup(name);
    e->left = NULL;
    e->right = NULL;
    return e;
}

Expr *create_binop(OpType op, Expr *left, Expr *right) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_BINOP;
    e->val = 0;
    e->name = NULL;
    e->op = op;
    e->left = left;
    e->right = right;
    return e;
}

void optimize(Expr *e) {
    if (e == NULL) return;
    if (e->type != NODE_BINOP) return;

    /* Recurse into children first (bottom-up) */
    optimize(e->left);
    optimize(e->right);

    /* Fold if both children are now constants */
    if (e->left->type == NODE_CONST && e->right->type == NODE_CONST) {
        int result = 0;
        if (e->op == OP_IR_ADD) {
            result = e->left->val + e->right->val;
        } else if (e->op == OP_IR_MUL) {
            result = e->left->val * e->right->val;
        }

        /* Convert this node to a constant */
        e->type = NODE_CONST;
        e->val = result;

        /* Free children */
        expr_free(e->left);
        expr_free(e->right);
        e->left = NULL;
        e->right = NULL;
    }
}

void expr_free(Expr *e) {
    if (e == NULL) return;
    if (e->left != NULL) expr_free(e->left);
    if (e->right != NULL) expr_free(e->right);
    if (e->name != NULL) free(e->name);
    free(e);
}
