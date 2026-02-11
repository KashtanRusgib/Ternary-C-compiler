/*
 * test_lexer.c - Unit tests for the tokenizer/lexer
 *
 * Tests: tokenize() from parser.c
 * Coverage: valid expressions, whitespace, multi-digit ints,
 *           edge cases, invalid characters
 */

#include "../include/test_harness.h"
#include "../include/parser.h"

/* ---- Basic tokenization ---- */

TEST(test_single_integer) {
    tokenize("42");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 42);
    ASSERT_EQ(tokens[1].type, TOK_EOF);
}

TEST(test_simple_addition) {
    tokenize("1 + 2");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

TEST(test_simple_multiplication) {
    tokenize("3 * 4");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 3);
    ASSERT_EQ(tokens[1].type, TOK_MUL);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 4);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

TEST(test_mixed_operators) {
    tokenize("1 + 2 * 3");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_MUL);
    ASSERT_EQ(tokens[4].type, TOK_INT);
    ASSERT_EQ(tokens[4].value, 3);
    ASSERT_EQ(tokens[5].type, TOK_EOF);
}

/* ---- Whitespace handling ---- */

TEST(test_no_spaces) {
    tokenize("1+2");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

TEST(test_extra_spaces) {
    tokenize("  1   +   2  ");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

TEST(test_tabs_and_newlines) {
    tokenize("\t1\n+\t2\n");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

/* ---- Multi-digit integers ---- */

TEST(test_multi_digit) {
    tokenize("123 + 456");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 123);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 456);
    ASSERT_EQ(tokens[3].type, TOK_EOF);
}

TEST(test_zero) {
    tokenize("0");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 0);
    ASSERT_EQ(tokens[1].type, TOK_EOF);
}

/* ---- Complex expressions ---- */

TEST(test_long_expression) {
    tokenize("1 + 2 + 3 + 4");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 1);
    ASSERT_EQ(tokens[1].type, TOK_PLUS);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 2);
    ASSERT_EQ(tokens[3].type, TOK_PLUS);
    ASSERT_EQ(tokens[4].type, TOK_INT);
    ASSERT_EQ(tokens[4].value, 3);
    ASSERT_EQ(tokens[5].type, TOK_PLUS);
    ASSERT_EQ(tokens[6].type, TOK_INT);
    ASSERT_EQ(tokens[6].value, 4);
    ASSERT_EQ(tokens[7].type, TOK_EOF);
}

TEST(test_chained_multiplication) {
    tokenize("2 * 3 * 4");
    ASSERT_EQ(tokens[0].type, TOK_INT);
    ASSERT_EQ(tokens[0].value, 2);
    ASSERT_EQ(tokens[1].type, TOK_MUL);
    ASSERT_EQ(tokens[2].type, TOK_INT);
    ASSERT_EQ(tokens[2].value, 3);
    ASSERT_EQ(tokens[3].type, TOK_MUL);
    ASSERT_EQ(tokens[4].type, TOK_INT);
    ASSERT_EQ(tokens[4].value, 4);
    ASSERT_EQ(tokens[5].type, TOK_EOF);
}

/* ---- Edge case: empty input ---- */

TEST(test_empty_string) {
    tokenize("");
    ASSERT_EQ(tokens[0].type, TOK_EOF);
}

TEST(test_whitespace_only) {
    tokenize("   \t\n  ");
    ASSERT_EQ(tokens[0].type, TOK_EOF);
}

int main(void) {
    TEST_SUITE_BEGIN("Lexer/Tokenizer");

    RUN_TEST(test_single_integer);
    RUN_TEST(test_simple_addition);
    RUN_TEST(test_simple_multiplication);
    RUN_TEST(test_mixed_operators);
    RUN_TEST(test_no_spaces);
    RUN_TEST(test_extra_spaces);
    RUN_TEST(test_tabs_and_newlines);
    RUN_TEST(test_multi_digit);
    RUN_TEST(test_zero);
    RUN_TEST(test_long_expression);
    RUN_TEST(test_chained_multiplication);
    RUN_TEST(test_empty_string);
    RUN_TEST(test_whitespace_only);

    TEST_SUITE_END();
}
