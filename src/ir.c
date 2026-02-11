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
    e->body = NULL;
    e->params = NULL;
    e->param_count = 0;
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
    e->body = NULL;
    e->params = NULL;
    e->param_count = 0;
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
    e->body = NULL;
    e->params = NULL;
    e->param_count = 0;
    return e;
}

void optimize(Expr *e) {
    if (e == NULL) return;

    switch (e->type) {
        case NODE_CONST:
        case NODE_VAR:
            return;

        case NODE_BINOP:
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
            break;

        case NODE_FUNC_DEF:
            optimize(e->body);
            break;

        case NODE_FUNC_CALL:
            for (int i = 0; i < e->param_count; i++) {
                optimize(e->params[i]);
            }
            break;

        case NODE_RETURN:
            optimize(e->left);
            break;

        case NODE_PROGRAM:
            for (int i = 0; i < e->param_count; i++) {
                optimize(e->params[i]);
            }
            break;
    }
}

void expr_free(Expr *e) {
    if (e == NULL) return;
    if (e->left != NULL) expr_free(e->left);
    if (e->right != NULL) expr_free(e->right);
    if (e->body != NULL) expr_free(e->body);
    if (e->params != NULL) {
        for (int i = 0; i < e->param_count; i++) {
            expr_free(e->params[i]);
        }
        free(e->params);
    }
    if (e->name != NULL) free(e->name);
    free(e);
}

Expr *create_func_def(const char *name, Expr **params, int param_count, Expr *body) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_FUNC_DEF;
    e->val = 0;
    e->name = strdup(name);
    e->op = OP_IR_ADD;
    e->left = NULL;
    e->right = NULL;
    e->body = body;
    e->params = params;
    e->param_count = param_count;
    return e;
}

Expr *create_func_call(const char *name, Expr **args, int arg_count) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_FUNC_CALL;
    e->val = 0;
    e->name = strdup(name);
    e->op = OP_IR_ADD;
    e->left = NULL;
    e->right = NULL;
    e->body = NULL;
    e->params = args;
    e->param_count = arg_count;
    return e;
}

Expr *create_return(Expr *expr) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_RETURN;
    e->val = 0;
    e->name = NULL;
    e->op = OP_IR_ADD;
    e->left = expr;
    e->right = NULL;
    e->body = NULL;
    e->params = NULL;
    e->param_count = 0;
    return e;
}

Expr *create_program(void) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (e == NULL) {
        fprintf(stderr, "ir: malloc failed\n");
        exit(1);
    }
    e->type = NODE_PROGRAM;
    e->val = 0;
    e->name = NULL;
    e->op = OP_IR_ADD;
    e->left = NULL;
    e->right = NULL;
    e->body = NULL;
    e->params = NULL;
    e->param_count = 0;
    return e;
}

void program_add_func(Expr *prog, Expr *func) {
    prog->param_count++;
    prog->params = (Expr **)realloc(prog->params, prog->param_count * sizeof(Expr *));
    if (prog->params == NULL) {
        fprintf(stderr, "ir: realloc failed\n");
        exit(1);
    }
    prog->params[prog->param_count - 1] = func;
}
