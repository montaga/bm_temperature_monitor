#include <gtest/gtest.h>
#include "common/ringbuffer.h"

class RingBufferTest : public ::testing::Test {
protected:
    ringbuffer_t buffer;

    void SetUp() override {
        rb_init(&buffer);
    }
};

TEST_F(RingBufferTest, Init) {
    EXPECT_EQ(0u, rb_count(&buffer));
}

TEST_F(RingBufferTest, PushAndPopSingle) {
    int16_t value = 42;
    EXPECT_TRUE(rb_push(&buffer, value));
    EXPECT_EQ(1u, rb_count(&buffer));

    int16_t popped;
    EXPECT_TRUE(rb_pop(&buffer, &popped));
    EXPECT_EQ(value, popped);
    EXPECT_EQ(0u, rb_count(&buffer));
}

TEST_F(RingBufferTest, PushAndPopMultiple) {
    const int num_items = 5;
    int16_t values[num_items] = {10, 20, 30, 40, 50};

    // Push all values
    for (int i = 0; i < num_items; i++) {
        EXPECT_TRUE(rb_push(&buffer, values[i]));
        EXPECT_EQ((size_t)(i + 1), rb_count(&buffer));
    }

    // Pop all values
    for (int i = 0; i < num_items; i++) {
        int16_t popped;
        EXPECT_TRUE(rb_pop(&buffer, &popped));
        EXPECT_EQ(values[i], popped);
        EXPECT_EQ((size_t)(num_items - i - 1), rb_count(&buffer));
    }
}

TEST_F(RingBufferTest, PopEmpty) {
    int16_t popped;
    EXPECT_FALSE(rb_pop(&buffer, &popped));
    EXPECT_EQ(0u, rb_count(&buffer));
}

TEST_F(RingBufferTest, PushFull) {
    // Fill the buffer to capacity - 1 (max storable items)
    for (int i = 0; i < RINGBUFFER_CAPACITY - 1; i++) {
        EXPECT_TRUE(rb_push(&buffer, (int16_t)i));
    }
    EXPECT_EQ((size_t)RINGBUFFER_CAPACITY - 1, rb_count(&buffer));

    // Try to push one more (should fail)
    EXPECT_FALSE(rb_push(&buffer, 9999));
    EXPECT_EQ((size_t)RINGBUFFER_CAPACITY - 1, rb_count(&buffer));
}

TEST_F(RingBufferTest, WrapAround) {
    // Fill buffer to max capacity
    for (int i = 0; i < RINGBUFFER_CAPACITY - 1; i++) {
        EXPECT_TRUE(rb_push(&buffer, (int16_t)i));
    }

    // Pop half
    const int half = (RINGBUFFER_CAPACITY - 1) / 2;
    for (int i = 0; i < half; i++) {
        int16_t popped;
        EXPECT_TRUE(rb_pop(&buffer, &popped));
        EXPECT_EQ((int16_t)i, popped);
    }

    // Push more to test wrap-around
    for (int i = 0; i < half; i++) {
        EXPECT_TRUE(rb_push(&buffer, (int16_t)(i + 1000)));
    }

    // Pop remaining original
    for (int i = half; i < RINGBUFFER_CAPACITY - 1; i++) {
        int16_t popped;
        EXPECT_TRUE(rb_pop(&buffer, &popped));
        EXPECT_EQ((int16_t)i, popped);
    }

    // Pop new values
    for (int i = 0; i < half; i++) {
        int16_t popped;
        EXPECT_TRUE(rb_pop(&buffer, &popped));
        EXPECT_EQ((int16_t)(i + 1000), popped);
    }

    EXPECT_EQ(0u, rb_count(&buffer));
}

TEST_F(RingBufferTest, Reset) {
    // Fill buffer
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(rb_push(&buffer, (int16_t)i));
    }
    EXPECT_EQ(10u, rb_count(&buffer));

    // Reset
    rb_reset(&buffer);
    EXPECT_EQ(0u, rb_count(&buffer));

    // Should be able to pop nothing
    int16_t popped;
    EXPECT_FALSE(rb_pop(&buffer, &popped));
}

TEST_F(RingBufferTest, Count) {
    EXPECT_EQ(0u, rb_count(&buffer));

    rb_push(&buffer, 1);
    EXPECT_EQ(1u, rb_count(&buffer));

    rb_push(&buffer, 2);
    EXPECT_EQ(2u, rb_count(&buffer));

    int16_t popped;
    rb_pop(&buffer, &popped);
    EXPECT_EQ(1u, rb_count(&buffer));

    rb_pop(&buffer, &popped);
    EXPECT_EQ(0u, rb_count(&buffer));
}