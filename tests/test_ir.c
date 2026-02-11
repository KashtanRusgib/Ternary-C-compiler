/*
 * test_ir.c - Unit tests for the IR and constant folding optimizer
 *
 * Tests: create_const, create_var, create_binop, optimize, expr_free
 * Coverage: folding add, folding mul, nested fold, no-fold with vars,
 *           single const passthrough, chained operations
 */

#include "../include/test_harness.h"
#include "../include/ir.h"

/* ---- Constant folding: addition ---- */

TEST(test_fold_add) {
    /* 1 + 2 = 3 */
    Expr *e = create_binop(OP_IR_ADD, create_const(1), create_const(2));
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 3);
    expr_free(e);
}

/* ---- Constant folding: multiplication ---- */

TEST(test_fold_mul) {
    /* 3 * 4 = 12 */
    Expr *e = create_binop(OP_IR_MUL, create_const(3), create_const(4));
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 12);
    expr_free(e);
}

/* ---- No fold when variable present ---- */

TEST(test_no_fold_vars) {
    /* x + 2 should NOT fold */
    Expr *e = create_binop(OP_IR_ADD, create_var("x"), create_const(2));
    optimize(e);
    ASSERT_EQ(e->type, NODE_BINOP);
    ASSERT_EQ(e->left->type, NODE_VAR);
    ASSERT_EQ(e->right->type, NODE_CONST);
    ASSERT_EQ(e->right->val, 2);
    expr_free(e);
}

/* ---- Nested constant folding ---- */

TEST(test_fold_nested) {
    /* (1 + 2) * (3 + 4) = 3 * 7 = 21 */
    Expr *left = create_binop(OP_IR_ADD, create_const(1), create_const(2));
    Expr *right = create_binop(OP_IR_ADD, create_const(3), create_const(4));
    Expr *e = create_binop(OP_IR_MUL, left, right);
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 21);
    expr_free(e);
}

/* ---- Partial fold: one branch foldable ---- */

TEST(test_fold_partial) {
    /* (1 + 2) * x -> 3 * x (partial: left folds, top stays binop) */
    Expr *left = create_binop(OP_IR_ADD, create_const(1), create_const(2));
    Expr *e = create_binop(OP_IR_MUL, left, create_var("x"));
    optimize(e);
    ASSERT_EQ(e->type, NODE_BINOP);
    ASSERT_EQ(e->left->type, NODE_CONST);
    ASSERT_EQ(e->left->val, 3);
    ASSERT_EQ(e->right->type, NODE_VAR);
    expr_free(e);
}

/* ---- Single constant: no-op optimize ---- */

TEST(test_single_const) {
    Expr *e = create_const(42);
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 42);
    expr_free(e);
}

/* ---- Single var: no-op optimize ---- */

TEST(test_single_var) {
    Expr *e = create_var("y");
    optimize(e);
    ASSERT_EQ(e->type, NODE_VAR);
    expr_free(e);
}

/* ---- Fold with zero ---- */

TEST(test_fold_add_zero) {
    /* 0 + 5 = 5 */
    Expr *e = create_binop(OP_IR_ADD, create_const(0), create_const(5));
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 5);
    expr_free(e);
}

TEST(test_fold_mul_zero) {
    /* 7 * 0 = 0 */
    Expr *e = create_binop(OP_IR_MUL, create_const(7), create_const(0));
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 0);
    expr_free(e);
}

/* ---- Deeply nested chain ---- */

TEST(test_fold_deep_chain) {
    /* ((2 * 3) + (4 * 5)) + 1 = (6 + 20) + 1 = 27 */
    Expr *a = create_binop(OP_IR_MUL, create_const(2), create_const(3));
    Expr *b = create_binop(OP_IR_MUL, create_const(4), create_const(5));
    Expr *c = create_binop(OP_IR_ADD, a, b);
    Expr *e = create_binop(OP_IR_ADD, c, create_const(1));
    optimize(e);
    ASSERT_EQ(e->type, NODE_CONST);
    ASSERT_EQ(e->val, 27);
    expr_free(e);
}

/* ---- NULL safety ---- */

TEST(test_optimize_null) {
    /* Should not crash */
    optimize(NULL);
    ASSERT_TRUE(1);
}

int main(void) {
    TEST_SUITE_BEGIN("IR / Constant Folding");

    RUN_TEST(test_fold_add);
    RUN_TEST(test_fold_mul);
    RUN_TEST(test_no_fold_vars);
    RUN_TEST(test_fold_nested);
    RUN_TEST(test_fold_partial);
    RUN_TEST(test_single_const);
    RUN_TEST(test_single_var);
    RUN_TEST(test_fold_add_zero);
    RUN_TEST(test_fold_mul_zero);
    RUN_TEST(test_fold_deep_chain);
    RUN_TEST(test_optimize_null);

    TEST_SUITE_END();
}
