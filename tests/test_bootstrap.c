/*
 * test_bootstrap.c - Tests for TASK-018: Self-Hosting Bootstrap
 *
 * Tests: symbol table, AST-to-bytecode emission, bootstrap compile,
 * self-test (compile & run mini-program), variable support.
 */

#include "../include/test_harness.h"
#include "../include/bootstrap.h"
#include "../include/vm.h"
#include "../include/ir.h"

/* ---- Symbol table tests ---- */

TEST(test_symtab_init) {
    BootstrapSymTab tab;
    symtab_init(&tab);
    ASSERT_EQ(tab.count, 0);
    ASSERT_EQ(tab.next_offset, 0);
}

TEST(test_symtab_add) {
    BootstrapSymTab tab;
    symtab_init(&tab);
    int off = symtab_add(&tab, "x", 0);
    ASSERT_EQ(off, 0);
    ASSERT_EQ(tab.count, 1);
    int off2 = symtab_add(&tab, "y", 1);
    ASSERT_EQ(off2, 1);
    ASSERT_EQ(tab.count, 2);
}

TEST(test_symtab_lookup) {
    BootstrapSymTab tab;
    symtab_init(&tab);
    symtab_add(&tab, "alpha", 0);
    symtab_add(&tab, "beta", 0);
    ASSERT_EQ(symtab_lookup(&tab, "alpha"), 0);
    ASSERT_EQ(symtab_lookup(&tab, "beta"), 1);
    ASSERT_EQ(symtab_lookup(&tab, "gamma"), -1);
}

TEST(test_symtab_full) {
    BootstrapSymTab tab;
    symtab_init(&tab);
    /* Fill up symbol table */
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        char name[8];
        snprintf(name, sizeof(name), "v%d", i);
        ASSERT_TRUE(symtab_add(&tab, name, 0) >= 0);
    }
    /* Next add should fail */
    ASSERT_EQ(symtab_add(&tab, "overflow", 0), -1);
}

/* ---- Bootstrap compile tests ---- */

TEST(test_bootstrap_simple_const) {
    unsigned char code[256];
    int len = bootstrap_compile("int main() { return 42; }", code, 256);
    ASSERT_TRUE(len > 0);
    /* Should contain at least OP_PUSH, 42, OP_HALT */
    ASSERT_TRUE(len >= 3);
}

TEST(test_bootstrap_addition) {
    unsigned char code[256];
    int len = bootstrap_compile("int main() { return 1 + 2; }", code, 256);
    ASSERT_TRUE(len > 0);
    /* Constant folding: 1+2=3, so should be PUSH 3, HALT */
    /* Actually, optimize is called, so it should fold. */
    ASSERT_TRUE(len >= 3);
}

TEST(test_bootstrap_var_decl) {
    unsigned char code[256];
    int len = bootstrap_compile("int main() { int x = 5; return x; }", code, 256);
    ASSERT_TRUE(len > 0);
}

TEST(test_bootstrap_multi_var) {
    unsigned char code[256];
    int len = bootstrap_compile(
        "int main() { int a = 1 + 2; int b = a * 3; return b; }",
        code, 256
    );
    ASSERT_TRUE(len > 0);
}

TEST(test_bootstrap_self_test) {
    int result = bootstrap_self_test();
    ASSERT_EQ(result, 0);
}

TEST(test_bootstrap_subtraction) {
    unsigned char code[256];
    int len = bootstrap_compile("int main() { return 10 - 3; }", code, 256);
    ASSERT_TRUE(len > 0);
    /* After constant folding: 10-3=7, so PUSH 7, HALT */
}

TEST(test_bootstrap_nested_expr) {
    unsigned char code[256];
    int len = bootstrap_compile(
        "int main() { int x = 2 * 3 + 1; return x; }",
        code, 256
    );
    ASSERT_TRUE(len > 0);
}

TEST(test_bootstrap_pointer_syntax) {
    unsigned char code[256];
    int len = bootstrap_compile(
        "int main() { int x = 10; return &x; }",
        code, 256
    );
    /* Should parse and compile, even if address-of is simple */
    ASSERT_TRUE(len > 0);
}

int main(void) {
    TEST_SUITE_BEGIN("Bootstrap Self-Host (TASK-018)");
    /* Symbol table */
    RUN_TEST(test_symtab_init);
    RUN_TEST(test_symtab_add);
    RUN_TEST(test_symtab_lookup);
    RUN_TEST(test_symtab_full);
    /* Compilation */
    RUN_TEST(test_bootstrap_simple_const);
    RUN_TEST(test_bootstrap_addition);
    RUN_TEST(test_bootstrap_var_decl);
    RUN_TEST(test_bootstrap_multi_var);
    RUN_TEST(test_bootstrap_subtraction);
    RUN_TEST(test_bootstrap_nested_expr);
    RUN_TEST(test_bootstrap_pointer_syntax);
    /* Self-test */
    RUN_TEST(test_bootstrap_self_test);
    TEST_SUITE_END();
}
