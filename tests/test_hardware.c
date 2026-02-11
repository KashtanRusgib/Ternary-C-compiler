/*
 * test_hardware.c - Tests for TASK-017: Ternary Hardware Simulation
 *
 * Tests C-level ternary gate simulations that mirror the Verilog
 * ternary ALU (hw/ternary_alu.v). Verifies: trit add, mul, min,
 * max, inverter, carry propagation, word-level operations.
 */

#include "../include/test_harness.h"
#include "../include/ternary.h"

/* ---- Single-trit gate tests ---- */

TEST(test_trit_add_basic) {
    trit carry = TRIT_Z;
    /* P + N = Z, carry Z */
    ASSERT_EQ(trit_add(TRIT_P, TRIT_N, &carry), TRIT_Z);
    ASSERT_EQ(carry, TRIT_Z);

    /* Z + Z = Z, carry Z */
    carry = TRIT_Z;
    ASSERT_EQ(trit_add(TRIT_Z, TRIT_Z, &carry), TRIT_Z);
    ASSERT_EQ(carry, TRIT_Z);

    /* P + Z = P, carry Z */
    carry = TRIT_Z;
    ASSERT_EQ(trit_add(TRIT_P, TRIT_Z, &carry), TRIT_P);
    ASSERT_EQ(carry, TRIT_Z);
}

TEST(test_trit_add_carry) {
    trit carry = TRIT_Z;
    /* P + P = N, carry P (1+1=2, balanced: -1 with carry +1) */
    ASSERT_EQ(trit_add(TRIT_P, TRIT_P, &carry), TRIT_N);
    ASSERT_EQ(carry, TRIT_P);

    /* N + N = P, carry N (-1+-1=-2, balanced: +1 with carry -1) */
    carry = TRIT_Z;
    ASSERT_EQ(trit_add(TRIT_N, TRIT_N, &carry), TRIT_P);
    ASSERT_EQ(carry, TRIT_N);

    /* P + P + P(carry) = Z, carry P (1+1+1=3, balanced: 0 carry 1) */
    carry = TRIT_P;
    ASSERT_EQ(trit_add(TRIT_P, TRIT_P, &carry), TRIT_Z);
    ASSERT_EQ(carry, TRIT_P);
}

TEST(test_trit_mul) {
    /* trit multiplication truth table */
    ASSERT_EQ(trit_mul(TRIT_P, TRIT_P), TRIT_P);   /*  1 *  1 =  1 */
    ASSERT_EQ(trit_mul(TRIT_P, TRIT_N), TRIT_N);   /*  1 * -1 = -1 */
    ASSERT_EQ(trit_mul(TRIT_N, TRIT_P), TRIT_N);   /* -1 *  1 = -1 */
    ASSERT_EQ(trit_mul(TRIT_N, TRIT_N), TRIT_P);   /* -1 * -1 =  1 */
    ASSERT_EQ(trit_mul(TRIT_Z, TRIT_P), TRIT_Z);   /*  0 *  1 =  0 */
    ASSERT_EQ(trit_mul(TRIT_P, TRIT_Z), TRIT_Z);   /*  1 *  0 =  0 */
    ASSERT_EQ(trit_mul(TRIT_Z, TRIT_Z), TRIT_Z);   /*  0 *  0 =  0 */
}

TEST(test_trit_min) {
    /* MIN gate: returns lesser-valued trit */
    ASSERT_EQ(trit_min(TRIT_P, TRIT_N), TRIT_N);
    ASSERT_EQ(trit_min(TRIT_N, TRIT_P), TRIT_N);
    ASSERT_EQ(trit_min(TRIT_Z, TRIT_P), TRIT_Z);
    ASSERT_EQ(trit_min(TRIT_Z, TRIT_N), TRIT_N);
    ASSERT_EQ(trit_min(TRIT_P, TRIT_P), TRIT_P);
    ASSERT_EQ(trit_min(TRIT_N, TRIT_N), TRIT_N);
}

TEST(test_trit_max) {
    /* MAX gate: returns greater-valued trit */
    ASSERT_EQ(trit_max(TRIT_P, TRIT_N), TRIT_P);
    ASSERT_EQ(trit_max(TRIT_N, TRIT_P), TRIT_P);
    ASSERT_EQ(trit_max(TRIT_Z, TRIT_P), TRIT_P);
    ASSERT_EQ(trit_max(TRIT_Z, TRIT_N), TRIT_Z);
    ASSERT_EQ(trit_max(TRIT_P, TRIT_P), TRIT_P);
    ASSERT_EQ(trit_max(TRIT_N, TRIT_N), TRIT_N);
}

TEST(test_trit_inverter) {
    /* Ternary inverter: negate (-1 -> +1, 0 -> 0, +1 -> -1) */
    ASSERT_EQ((trit)(-(int)TRIT_P), TRIT_N);
    ASSERT_EQ((trit)(-(int)TRIT_N), TRIT_P);
    ASSERT_EQ((trit)(-(int)TRIT_Z), TRIT_Z);
}

/* ---- Word-level tests ---- */

TEST(test_trit_word_add) {
    trit_word a, b, res;

    /* 1 + 1 = 2 (balanced ternary: [N, P, Z, ...] = -1 + 3 = 2) */
    int_to_trit_word(1, a);
    int_to_trit_word(1, b);
    trit_word_add(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), 2);

    /* 13 + 14 = 27 = 3^3 */
    int_to_trit_word(13, a);
    int_to_trit_word(14, b);
    trit_word_add(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), 27);

    /* -1 + 1 = 0 */
    int_to_trit_word(-1, a);
    int_to_trit_word(1, b);
    trit_word_add(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), 0);
}

TEST(test_trit_word_mul) {
    trit_word a, b, res;

    /* 3 * 3 = 9 (ternary power) */
    int_to_trit_word(3, a);
    int_to_trit_word(3, b);
    trit_word_mul(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), 9);

    /* 1 * -1 = -1 */
    int_to_trit_word(1, a);
    int_to_trit_word(-1, b);
    trit_word_mul(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), -1);

    /* 0 * 42 = 0 */
    int_to_trit_word(0, a);
    int_to_trit_word(42, b);
    trit_word_mul(a, b, res);
    ASSERT_EQ(trit_word_to_int(res), 0);
}

TEST(test_trit_word_conversion_roundtrip) {
    /* Test int -> trit_word -> int roundtrip for several values */
    int test_values[] = {0, 1, -1, 3, -3, 9, -9, 13, 27, -27, 40, -40, 121};
    int count = sizeof(test_values) / sizeof(test_values[0]);
    trit_word w;

    for (int i = 0; i < count; i++) {
        int_to_trit_word(test_values[i], w);
        ASSERT_EQ(trit_word_to_int(w), test_values[i]);
    }
}

TEST(test_trit_word_sub_via_negate) {
    /* Subtraction: a - b = a + (-b) (negate then add) */
    trit_word a, b, neg_b, res;

    int_to_trit_word(10, a);
    int_to_trit_word(3, b);

    /* Negate b trit-by-trit */
    for (int i = 0; i < WORD_SIZE; i++) {
        neg_b[i] = (trit)(-(int)b[i]);
    }

    trit_word_add(a, neg_b, res);
    ASSERT_EQ(trit_word_to_int(res), 7);
}

/* ---- ALU simulation tests (C equivalent of Verilog ALU) ---- */

/* C simulation of ALU operations matching hw/ternary_alu.v */
static int alu_sim(int a, int b, int op) {
    switch (op) {
        case 0: return a + b;     /* ADD */
        case 1: return a - b;     /* SUB */
        case 2: return a * b;     /* MUL */
        default: return 0;
    }
}

TEST(test_alu_sim_add) {
    ASSERT_EQ(alu_sim(1, 1, 0), 2);
    ASSERT_EQ(alu_sim(13, 14, 0), 27);
    ASSERT_EQ(alu_sim(-1, 1, 0), 0);
}

TEST(test_alu_sim_sub) {
    ASSERT_EQ(alu_sim(1, 1, 1), 0);
    ASSERT_EQ(alu_sim(10, 3, 1), 7);
    ASSERT_EQ(alu_sim(0, 1, 1), -1);
}

TEST(test_alu_sim_mul) {
    ASSERT_EQ(alu_sim(3, 3, 2), 9);
    ASSERT_EQ(alu_sim(1, -1, 2), -1);
    ASSERT_EQ(alu_sim(-1, -1, 2), 1);
    ASSERT_EQ(alu_sim(0, 42, 2), 0);
}

int main(void) {
    TEST_SUITE_BEGIN("Ternary Hardware (TASK-017)");
    /* Single-trit gate tests */
    RUN_TEST(test_trit_add_basic);
    RUN_TEST(test_trit_add_carry);
    RUN_TEST(test_trit_mul);
    RUN_TEST(test_trit_min);
    RUN_TEST(test_trit_max);
    RUN_TEST(test_trit_inverter);
    /* Word-level tests */
    RUN_TEST(test_trit_word_add);
    RUN_TEST(test_trit_word_mul);
    RUN_TEST(test_trit_word_conversion_roundtrip);
    RUN_TEST(test_trit_word_sub_via_negate);
    /* ALU simulation */
    RUN_TEST(test_alu_sim_add);
    RUN_TEST(test_alu_sim_sub);
    RUN_TEST(test_alu_sim_mul);
    TEST_SUITE_END();
}
