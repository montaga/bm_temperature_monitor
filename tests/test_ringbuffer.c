#include "unity.h"
#include "../project/common/ringbuffer.h"

void test_ringbuffer_init(void) {
    ringbuffer_t rb;
    rb_init(&rb);
    TEST_ASSERT_EQUAL(0, rb.head);
    TEST_ASSERT_EQUAL(0, rb.tail);
}

void test_ringbuffer_push_pop(void) {
    ringbuffer_t rb;
    rb_init(&rb);
    int16_t value;

    TEST_ASSERT_TRUE(rb_push(&rb, 42));
    TEST_ASSERT_TRUE(rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL(42, value);
}

void test_ringbuffer_empty_pop(void) {
    ringbuffer_t rb;
    rb_init(&rb);
    int16_t value;

    TEST_ASSERT_FALSE(rb_pop(&rb, &value));
}

void test_ringbuffer_full_push(void) {
    ringbuffer_t rb;
    rb_init(&rb);

    for (int i = 0; i < RINGBUFFER_CAPACITY; ++i) {
        TEST_ASSERT_TRUE(rb_push(&rb, i));
    }
    TEST_ASSERT_TRUE(rb_push(&rb, 999)); // Should succeed, dropping oldest
}

void test_ringbuffer_wrap_around(void) {
    ringbuffer_t rb;
    rb_init(&rb);
    int16_t value;

    // Fill buffer
    for (int i = 0; i < RINGBUFFER_CAPACITY; ++i) {
        rb_push(&rb, i);
    }

    // Pop one to make space
    rb_pop(&rb, &value);
    TEST_ASSERT_EQUAL(1, value);  // First item was dropped when full, so starts from 1

    // Push another, should succeed
    TEST_ASSERT_TRUE(rb_push(&rb, 1000));

    // Pop all remaining (from 2 to 255, then 1000)
    for (int i = 2; i < RINGBUFFER_CAPACITY; ++i) {
        TEST_ASSERT_TRUE(rb_pop(&rb, &value));
        TEST_ASSERT_EQUAL(i, value);
    }
    TEST_ASSERT_TRUE(rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL(1000, value);
}

void test_ringbuffer_count(void) {
    ringbuffer_t rb;
    rb_init(&rb);

    TEST_ASSERT_EQUAL(0, rb_count(&rb));
    rb_push(&rb, 1);
    TEST_ASSERT_EQUAL(1, rb_count(&rb));
    rb_push(&rb, 2);
    TEST_ASSERT_EQUAL(2, rb_count(&rb));
    int16_t val;
    rb_pop(&rb, &val);
    TEST_ASSERT_EQUAL(1, rb_count(&rb));
}