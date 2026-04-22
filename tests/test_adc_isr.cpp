#include <gtest/gtest.h>
#include "common/ringbuffer.h"
#include "bsp/isr.h"

// Forward declaration for test mock function
extern "C" {
void test_set_mock_adc_value(int16_t value);
}

class ADC_ISR_Test : public ::testing::Test {
protected:
    ringbuffer_t buffer;

    void SetUp() override {
        rb_init(&buffer);
        // Start ISR with the buffer (without EMULATED_ISR, this just sets the buffer pointer)
        isr_start(&buffer);
    }

    void TearDown() override {
        isr_stop();
    }
};

TEST_F(ADC_ISR_Test, ISR_FillsRingBuffer_SingleCall) {
    // Set mock ADC value
    test_set_mock_adc_value(850); // 85°C in deci-°C

    // Call adc_isr directly (simulating hardware interrupt)
    adc_isr();

    // Check that buffer has one item
    EXPECT_EQ(1u, rb_count(&buffer));

    // Pop and verify the value
    int16_t value;
    EXPECT_TRUE(rb_pop(&buffer, &value));
    EXPECT_EQ(850, value);
}

TEST_F(ADC_ISR_Test, ISR_FillsRingBuffer_MultipleCalls) {
    // Call adc_isr multiple times with different values
    test_set_mock_adc_value(700);
    adc_isr();

    test_set_mock_adc_value(800);
    adc_isr();

    test_set_mock_adc_value(900);
    adc_isr();

    // Check buffer has 3 items
    EXPECT_EQ(3u, rb_count(&buffer));

    // Pop and verify values (FIFO order)
    int16_t value;
    EXPECT_TRUE(rb_pop(&buffer, &value));
    EXPECT_EQ(700, value);

    EXPECT_TRUE(rb_pop(&buffer, &value));
    EXPECT_EQ(800, value);

    EXPECT_TRUE(rb_pop(&buffer, &value));
    EXPECT_EQ(900, value);

    // Buffer should be empty
    EXPECT_EQ(0u, rb_count(&buffer));
}

TEST_F(ADC_ISR_Test, ISR_BufferFull) {
    // Fill buffer to capacity
    for (int i = 0; i < RINGBUFFER_CAPACITY - 1; i++) {
        test_set_mock_adc_value(i);
        adc_isr();
    }

    EXPECT_EQ((size_t)RINGBUFFER_CAPACITY - 1, rb_count(&buffer));

    // One more call should still work (buffer not full yet)
    test_set_mock_adc_value(999);
    adc_isr();
    EXPECT_EQ((size_t)RINGBUFFER_CAPACITY - 1, rb_count(&buffer)); // Should still be max

    // Try to add one more - should fail silently (ringbuffer push fails)
    test_set_mock_adc_value(1000);
    adc_isr();
    EXPECT_EQ((size_t)RINGBUFFER_CAPACITY - 1, rb_count(&buffer)); // Should remain the same
}