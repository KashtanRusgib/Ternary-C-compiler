/*
 * test_vm.c - Unit tests for the ternary VM
 *
 * Tests: vm_run() stack execution with various bytecode programs
 * Coverage: PUSH, ADD, MUL, HALT, stack behavior
 *
 * Note: We capture VM output by redirecting stdout to a buffer,
 * then verify the printed "Result: N" string.
 */

#include "../include/test_harness.h"
#include "../include/vm.h"
#include <string.h>
#include <unistd.h>

/* Helper: Run bytecode and capture stdout to buffer */
static char output_buf[256];

static void run_and_capture(unsigned char *code, size_t len) {
    /* Redirect stdout to a temp file, run, read back */
    FILE *tmp = tmpfile();
    if (tmp == NULL) {
        output_buf[0] = '\0';
        return;
    }

    /* Save stdout, redirect */
    int saved_stdout = dup(fileno(stdout));
    fflush(stdout);
    dup2(fileno(tmp), fileno(stdout));

    vm_run(code, len);

    /* Restore stdout */
    fflush(stdout);
    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    /* Read captured output */
    rewind(tmp);
    size_t n = fread(output_buf, 1, sizeof(output_buf) - 1, tmp);
    output_buf[n] = '\0';
    fclose(tmp);
}

/* ---- PUSH + HALT ---- */

TEST(test_vm_push_halt) {
    unsigned char code[] = {OP_PUSH, 5, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 5\n");
}

TEST(test_vm_push_zero) {
    unsigned char code[] = {OP_PUSH, 0, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 0\n");
}

/* ---- ADD ---- */

TEST(test_vm_simple_add) {
    unsigned char code[] = {OP_PUSH, 3, OP_PUSH, 4, OP_ADD, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 7\n");
}

TEST(test_vm_add_zero) {
    unsigned char code[] = {OP_PUSH, 5, OP_PUSH, 0, OP_ADD, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 5\n");
}

/* ---- MUL ---- */

TEST(test_vm_simple_mul) {
    unsigned char code[] = {OP_PUSH, 3, OP_PUSH, 4, OP_MUL, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 12\n");
}

TEST(test_vm_mul_zero) {
    unsigned char code[] = {OP_PUSH, 5, OP_PUSH, 0, OP_MUL, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 0\n");
}

TEST(test_vm_mul_one) {
    unsigned char code[] = {OP_PUSH, 7, OP_PUSH, 1, OP_MUL, OP_HALT};
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 7\n");
}

/* ---- Combined: 1 + 2 * 3 = 7 ---- */

TEST(test_vm_expression_1_plus_2_mul_3) {
    unsigned char code[] = {
        OP_PUSH, 1,
        OP_PUSH, 2,
        OP_PUSH, 3,
        OP_MUL,
        OP_ADD,
        OP_HALT
    };
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 7\n");
}

/* ---- Chained ADD: 1 + 2 + 3 = 6 ---- */

TEST(test_vm_chained_add) {
    unsigned char code[] = {
        OP_PUSH, 1,
        OP_PUSH, 2,
        OP_ADD,
        OP_PUSH, 3,
        OP_ADD,
        OP_HALT
    };
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 6\n");
}

/* ---- Chained MUL: 2 * 3 * 4 = 24 ---- */

TEST(test_vm_chained_mul) {
    unsigned char code[] = {
        OP_PUSH, 2,
        OP_PUSH, 3,
        OP_MUL,
        OP_PUSH, 4,
        OP_MUL,
        OP_HALT
    };
    run_and_capture(code, sizeof(code));
    ASSERT_STR_EQ(output_buf, "Result: 24\n");
}

int main(void) {
    TEST_SUITE_BEGIN("VM Execution");

    RUN_TEST(test_vm_push_halt);
    RUN_TEST(test_vm_push_zero);
    RUN_TEST(test_vm_simple_add);
    RUN_TEST(test_vm_add_zero);
    RUN_TEST(test_vm_simple_mul);
    RUN_TEST(test_vm_mul_zero);
    RUN_TEST(test_vm_mul_one);
    RUN_TEST(test_vm_expression_1_plus_2_mul_3);
    RUN_TEST(test_vm_chained_add);
    RUN_TEST(test_vm_chained_mul);

    TEST_SUITE_END();
}
