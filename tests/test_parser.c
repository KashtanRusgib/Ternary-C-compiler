/*
 * test_parser.c - Unit tests for the recursive descent function parser
 *
 * Tests: parse_program() from parser.c
 * Coverage: simple function, return statement, parameters, function calls,
 *           multiple functions, error handling, operator precedence
 */

#include "../include/test_harness.h"
#include "../include/parser.h"
#include "../include/ir.h"

/* ---- Parse simple main() ---- */

TEST(test_parse_simple_main) {
    Expr *prog = parse_program("int main() { return 42; }");
    ASSERT_NOT_NULL(prog);
    ASSERT_EQ(prog->type, NODE_PROGRAM);
    ASSERT_EQ(prog->param_count, 1);

    Expr *fn = prog->params[0];
    ASSERT_EQ(fn->type, NODE_FUNC_DEF);
    ASSERT_STR_EQ(fn->name, "main");
    ASSERT_EQ(fn->param_count, 0);

    Expr *body = fn->body;
    ASSERT_NOT_NULL(body);
    ASSERT_EQ(body->type, NODE_RETURN);
    ASSERT_NOT_NULL(body->left);
    ASSERT_EQ(body->left->type, NODE_CONST);
    ASSERT_EQ(body->left->val, 42);

    expr_free(prog);
}

/* ---- Parse return with arithmetic expression ---- */

TEST(test_parse_return_expr) {
    Expr *prog = parse_program("int main() { return 1 + 2; }");
    ASSERT_NOT_NULL(prog);

    Expr *fn = prog->params[0];
    Expr *ret = fn->body;
    ASSERT_EQ(ret->type, NODE_RETURN);

    Expr *expr = ret->left;
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->op, OP_IR_ADD);
    ASSERT_EQ(expr->left->type, NODE_CONST);
    ASSERT_EQ(expr->left->val, 1);
    ASSERT_EQ(expr->right->type, NODE_CONST);
    ASSERT_EQ(expr->right->val, 2);

    expr_free(prog);
}

/* ---- Parse function with parameter ---- */

TEST(test_parse_func_with_param) {
    Expr *prog = parse_program("int foo(int x) { return x; }");
    ASSERT_NOT_NULL(prog);

    Expr *fn = prog->params[0];
    ASSERT_EQ(fn->type, NODE_FUNC_DEF);
    ASSERT_STR_EQ(fn->name, "foo");
    ASSERT_EQ(fn->param_count, 1);
    ASSERT_EQ(fn->params[0]->type, NODE_VAR);
    ASSERT_STR_EQ(fn->params[0]->name, "x");

    Expr *ret = fn->body;
    ASSERT_EQ(ret->type, NODE_RETURN);
    ASSERT_EQ(ret->left->type, NODE_VAR);
    ASSERT_STR_EQ(ret->left->name, "x");

    expr_free(prog);
}

/* ---- Parse function call ---- */

TEST(test_parse_func_call) {
    Expr *prog = parse_program("int main() { return foo(5); }");
    ASSERT_NOT_NULL(prog);

    Expr *fn = prog->params[0];
    Expr *ret = fn->body;
    ASSERT_EQ(ret->type, NODE_RETURN);

    Expr *call = ret->left;
    ASSERT_EQ(call->type, NODE_FUNC_CALL);
    ASSERT_STR_EQ(call->name, "foo");
    ASSERT_EQ(call->param_count, 1);
    ASSERT_EQ(call->params[0]->type, NODE_CONST);
    ASSERT_EQ(call->params[0]->val, 5);

    expr_free(prog);
}

/* ---- Parse multiple functions ---- */

TEST(test_parse_multi_func) {
    Expr *prog = parse_program(
        "int foo(int x) { return x + 1; } "
        "int main() { return foo(5); }");
    ASSERT_NOT_NULL(prog);
    ASSERT_EQ(prog->type, NODE_PROGRAM);
    ASSERT_EQ(prog->param_count, 2);

    ASSERT_STR_EQ(prog->params[0]->name, "foo");
    ASSERT_STR_EQ(prog->params[1]->name, "main");

    /* foo's body: return x + 1 */
    Expr *foo_ret = prog->params[0]->body;
    ASSERT_EQ(foo_ret->type, NODE_RETURN);
    ASSERT_EQ(foo_ret->left->type, NODE_BINOP);
    ASSERT_EQ(foo_ret->left->op, OP_IR_ADD);

    /* main's body: return foo(5) */
    Expr *main_ret = prog->params[1]->body;
    ASSERT_EQ(main_ret->type, NODE_RETURN);
    ASSERT_EQ(main_ret->left->type, NODE_FUNC_CALL);
    ASSERT_STR_EQ(main_ret->left->name, "foo");

    expr_free(prog);
}

/* ---- Parse invalid function (missing closing paren) ---- */

TEST(test_parse_invalid_func) {
    Expr *prog = parse_program("int main( { return 42; }");
    ASSERT_NULL(prog);
}

/* ---- Parse return with operator precedence (* > +) ---- */

TEST(test_parse_return_precedence) {
    /* return 1 + 2 * 3; should parse as 1 + (2 * 3) */
    Expr *prog = parse_program("int main() { return 1 + 2 * 3; }");
    ASSERT_NOT_NULL(prog);

    Expr *ret = prog->params[0]->body;
    Expr *expr = ret->left;
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->op, OP_IR_ADD);

    /* Left is 1 */
    ASSERT_EQ(expr->left->type, NODE_CONST);
    ASSERT_EQ(expr->left->val, 1);

    /* Right is 2 * 3 */
    ASSERT_EQ(expr->right->type, NODE_BINOP);
    ASSERT_EQ(expr->right->op, OP_IR_MUL);
    ASSERT_EQ(expr->right->left->val, 2);
    ASSERT_EQ(expr->right->right->val, 3);

    expr_free(prog);
}

int main(void) {
    TEST_SUITE_BEGIN("Parser (Functions)");

    RUN_TEST(test_parse_simple_main);
    RUN_TEST(test_parse_return_expr);
    RUN_TEST(test_parse_func_with_param);
    RUN_TEST(test_parse_func_call);
    RUN_TEST(test_parse_multi_func);
    RUN_TEST(test_parse_invalid_func);
    RUN_TEST(test_parse_return_precedence);

    TEST_SUITE_END();
}
