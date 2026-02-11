CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# ---- Source objects ----
SRC_OBJS   = src/main.o src/parser.o src/codegen.o src/logger.o src/ir.o
VM_OBJS    = vm/ternary_vm.o

# ---- Shared objects (used by tests) ----
LIB_OBJS   = src/parser.o src/codegen.o src/logger.o src/ir.o $(VM_OBJS)

# ---- Test binaries ----
TEST_BINS  = test_trit test_lexer test_parser test_codegen test_vm test_logger test_ir test_sel4 test_integration

# ---- Default target ----
all: ternary_compiler vm_test $(TEST_BINS)

# ---- Main compiler binary ----
ternary_compiler: $(SRC_OBJS) $(VM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ---- Standalone VM test ----
vm_test: vm/vm_test.o $(VM_OBJS) src/logger.o
	$(CC) $(CFLAGS) -o $@ $^

# ---- Test binaries ----
test_trit: tests/test_trit.o
	$(CC) $(CFLAGS) -o $@ $^

test_lexer: tests/test_lexer.o src/parser.o src/ir.o src/logger.o
	$(CC) $(CFLAGS) -o $@ $^

test_parser: tests/test_parser.o src/parser.o src/ir.o src/logger.o
	$(CC) $(CFLAGS) -o $@ $^

test_codegen: tests/test_codegen.o src/parser.o src/codegen.o src/logger.o $(VM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_vm: tests/test_vm.o $(VM_OBJS) src/logger.o
	$(CC) $(CFLAGS) -o $@ $^

test_logger: tests/test_logger.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_ir: tests/test_ir.o src/ir.o
	$(CC) $(CFLAGS) -o $@ $^

test_sel4: tests/test_sel4.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_integration: tests/test_integration.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ---- Generic rule for .c -> .o ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Dependencies ----
src/main.o:           src/main.c include/parser.h include/codegen.h include/vm.h include/ir.h include/logger.h
src/parser.o:         src/parser.c include/parser.h include/ir.h include/logger.h
src/codegen.o:        src/codegen.c include/codegen.h include/parser.h include/vm.h include/logger.h
src/logger.o:         src/logger.c include/logger.h
src/ir.o:             src/ir.c include/ir.h
vm/ternary_vm.o:      vm/ternary_vm.c include/vm.h include/ternary.h include/logger.h
vm/vm_test.o:         vm/vm_test.c include/vm.h
tests/test_trit.o:    tests/test_trit.c include/test_harness.h include/ternary.h
tests/test_lexer.o:   tests/test_lexer.c include/test_harness.h include/parser.h
tests/test_parser.o:  tests/test_parser.c include/test_harness.h include/parser.h include/ir.h
tests/test_codegen.o: tests/test_codegen.c include/test_harness.h include/parser.h include/codegen.h include/vm.h
tests/test_vm.o:      tests/test_vm.c include/test_harness.h include/vm.h
tests/test_logger.o:  tests/test_logger.c include/test_harness.h include/logger.h include/parser.h include/codegen.h include/vm.h include/ir.h
tests/test_ir.o:      tests/test_ir.c include/test_harness.h include/ir.h
tests/test_sel4.o:    tests/test_sel4.c include/test_harness.h include/parser.h include/codegen.h include/vm.h include/ir.h include/logger.h
tests/test_integration.o: tests/test_integration.c include/test_harness.h include/parser.h include/codegen.h include/vm.h

# ---- Test targets ----
test: $(TEST_BINS)
	@echo "========================================"
	@echo "  Running All Test Suites"
	@echo "========================================"
	@PASS=0; FAIL=0; \
	for t in $(TEST_BINS); do \
		echo ""; echo "--- $$t ---"; \
		if ./$$t; then PASS=$$((PASS+1)); \
		else FAIL=$$((FAIL+1)); fi; \
	done; \
	echo ""; \
	echo "========================================"; \
	echo "  $$PASS passed, $$FAIL failed"; \
	echo "========================================"; \
	if [ $$FAIL -gt 0 ]; then exit 1; fi

# ---- CI target: test + lint ----
ci: test lint
	@echo ""
	@echo "=== CI: All checks passed ==="

lint:
	@echo "=== Running lint (cppcheck) ==="
	@which cppcheck > /dev/null 2>&1 && cppcheck --enable=warning,style --error-exitcode=1 -Iinclude src/ vm/ 2>&1 || echo "[WARN] cppcheck not installed, skipping lint"

coverage:
	@echo "=== Building with coverage ==="
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -fprofile-arcs -ftest-coverage" all
	$(MAKE) test
	@echo "=== Generating coverage report ==="
	gcov -b src/*.c vm/*.c 2>/dev/null || echo "[WARN] gcov not available"
	@echo "=== Coverage complete ==="

# ---- Clean ----
clean:
	rm -f src/*.o vm/*.o tests/*.o
	rm -f ternary_compiler vm_test $(TEST_BINS)
	rm -f test_basic
	rm -f *.gcov *.gcda *.gcno src/*.gcda src/*.gcno vm/*.gcda vm/*.gcno tests/*.gcda tests/*.gcno

.PHONY: all test ci lint coverage clean
