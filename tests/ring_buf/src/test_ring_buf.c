/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 *   west twister -T tests/ring_buf -p qemu_x86 -s homework.unit.ring_buf --short-build-path -vvv
 *
 * Coverage run with:
 * (from git bash terminal, Windows env)
 *  subst Z: <current project dir>
 *  cd /z
 *  west twister -T tests/ring_buf -p qemu_x86 -s homework.unit.ring_buf --short-build-path -vvv --coverage --coverage-tool gcovr --coverage-basedir app/modules/ring_buf --gcov-tool x86_64-zephyr-elf-gcov.exe -x 'CONFIG_COVERAGE_DUMP_PATH_EXCLUDE="*zephyr*"' -x CONFIG_COVERAGE_GCOV_HEAP_SIZE=65536
 *
 *  path handling is terrible
 *  doesn't work without --short-build-path
 *  doesn't work without path aliasing
 *  too long paths -> gcovr fails to find .gcda files
 *
 *  location to x86_64-zephyr-elf-gcov.exe was also added to PATH
 *  otherwise twister/gcovr couldn't parse it's Windows path correctly
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void* f) {
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state) {
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state) {
	zassume_ok(rb_push(45), "Push should succeed");

	zassert_ok(rb_init(4), "Re-init should succeed");

	zassert_true(rb_is_empty(), "Re-initialised buffer must be empty");
	zassert_equal(rb_count(), 0, "Re-initialised buffer count must be 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop) {
	zassert_ok(rb_push(42), "Push should succeed");

	int v;
	zassert_ok(rb_pop(&v), "Pop should succeed");
	zassert_equal(v, 42, "Popped value should match pushed value");
	zassert_true(rb_is_empty(), "Buffer should be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order) {
	for (int i = 1; i <= 3; i++) {
		zassert_ok(rb_push(i), "Push should succeed");
	}

	for (int i = 1; i <= 3; i++) {
		int v;
		zassert_ok(rb_pop(&v), "Pop should succeed");
		zassert_equal(v, i, "Popped value should match pushed value");
	}

	zassert_true(rb_is_empty(), "Buffer should be empty after popping all values");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc) {
	for (int i = 1; i <= 4; i++) {
		zassume_ok(rb_push(i), "Push should succeed");
	}

	zassert_equal(rb_push(5), -ENOSPC, "Push should fail when buffer is full");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume) {
	zassume_ok(rb_push(7), "Push should succeed");

	int v;

	for (int i = 0; i < 2; i++) {
		zassert_ok(rb_peek(&v), "Peek should succeed");
		zassert_equal(v, 7, "Peeked value should match pushed value");
		zassert_equal(rb_count(), 1, "Buffer count should remain 1 after peek");
	}
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval) {
	zassert_equal(rb_pop(NULL), -EINVAL, "Pop with NULL pointer should return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill) {
	for (int i = 1; i <= 4; i++) {
		zassume_ok(rb_push(i), "Push should succeed");
	}
	zassert_true(rb_is_full(), "Buffer should be full");
	zassert_equal(rb_count(), 4, "Buffer count should equal capacity when full");
}
