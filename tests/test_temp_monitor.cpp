#include <gtest/gtest.h>
#include "app/temp_monitor.h"
#include "common/ringbuffer.h"
#include "hal/gpio.h"
#include "bsp/isr.h"
#include <cstring>
#include <thread>
#include <chrono>

// Forward declarations for test helper functions
extern "C" {
bool test_get_led_state(led_t led);
void test_reset_led_states(void);
}

class TempMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_reset_led_states();
    }
};

TEST_F(TempMonitorTest, InitRevA) {
    temp_monitor_t monitor;
    temp_monitor_init(&monitor, HW_REV_A);
    EXPECT_EQ(HW_REV_A, monitor.revision);
    EXPECT_EQ(TEMP_STATE_UNKNOWN, monitor.state);
    EXPECT_FALSE(monitor.has_sample);
}

TEST_F(TempMonitorTest, InitRevB) {
    temp_monitor_t monitor;
    temp_monitor_init(&monitor, HW_REV_B);
    EXPECT_EQ(HW_REV_B, monitor.revision);
    EXPECT_EQ(TEMP_STATE_UNKNOWN, monitor.state);
    EXPECT_FALSE(monitor.has_sample);
}

TEST_F(TempMonitorTest, StateTransitionNormalToWarning) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Simulate temperature rising to warning level (85°C)
    // For Rev-B, 85°C = 850 deci-°C
    int16_t warning_temp = 850;

    // Push warning temperature to buffer
    rb_push(&buffer, warning_temp);

    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(TEMP_STATE_WARNING, monitor.state);
    EXPECT_TRUE(monitor.has_sample);
    EXPECT_EQ(warning_temp, monitor.last_raw_value);
}

TEST_F(TempMonitorTest, StateTransitionWarningToNormal) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // First go to warning state
    int16_t warning_temp = 850;
    rb_push(&buffer, warning_temp);
    temp_monitor_process(&monitor, &buffer);
    EXPECT_EQ(TEMP_STATE_WARNING, monitor.state);

    // Then go back to normal (below hysteresis threshold)
    // Push multiple normal temperatures to let the filter converge
    int16_t normal_temp = 800; // Below 85°C - 1°C hysteresis
    for (int i = 0; i < 10; i++) {
        rb_push(&buffer, normal_temp);
    }
    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(TEMP_STATE_NORMAL, monitor.state);
}

TEST_F(TempMonitorTest, StateTransitionToCriticalHigh) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Temperature above critical high threshold (105°C = 1050 deci-°C)
    int16_t critical_temp = 1100;
    rb_push(&buffer, critical_temp);
    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(TEMP_STATE_CRITICAL_HIGH, monitor.state);
}

TEST_F(TempMonitorTest, StateTransitionToCriticalLow) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Temperature below critical low threshold (5°C = 50 deci-°C)
    int16_t critical_temp = 30;
    rb_push(&buffer, critical_temp);
    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(TEMP_STATE_CRITICAL_LOW, monitor.state);
}

TEST_F(TempMonitorTest, LEDUpdate) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Go to warning state
    int16_t warning_temp = 850;
    rb_push(&buffer, warning_temp);
    temp_monitor_process(&monitor, &buffer);
    temp_monitor_update_leds(&monitor);

    EXPECT_TRUE(test_get_led_state(LED_YELLOW));
    EXPECT_FALSE(test_get_led_state(LED_GREEN));
    EXPECT_FALSE(test_get_led_state(LED_RED));
}

TEST_F(TempMonitorTest, NoLEDUpdateOnSameState) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Go to warning state
    int16_t warning_temp = 850;
    rb_push(&buffer, warning_temp);
    temp_monitor_process(&monitor, &buffer);
    temp_monitor_update_leds(&monitor);

    // Reset mock states to check if they get updated again
    test_reset_led_states();

    // Process same temperature again
    rb_push(&buffer, warning_temp);
    temp_monitor_process(&monitor, &buffer);
    temp_monitor_update_leds(&monitor);

    // LEDs should not be updated since state didn't change
    EXPECT_FALSE(test_get_led_state(LED_YELLOW));
    EXPECT_FALSE(test_get_led_state(LED_GREEN));
    EXPECT_FALSE(test_get_led_state(LED_RED));
}

TEST_F(TempMonitorTest, TemperatureConversionRevA) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_A);

    // Rev-A: raw value is °C, should be converted to deci-°C
    int16_t raw_temp = 75; // 75°C
    rb_push(&buffer, raw_temp);
    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(750, monitor.filtered_temperature_dC); // 75°C = 750 deci-°C
}

TEST_F(TempMonitorTest, TemperatureConversionRevB) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Rev-B: raw value is already deci-°C
    int16_t raw_temp = 750; // 75.0°C
    rb_push(&buffer, raw_temp);
    temp_monitor_process(&monitor, &buffer);

    EXPECT_EQ(750, monitor.filtered_temperature_dC);
}

TEST_F(TempMonitorTest, TemperatureFiltering) {
    temp_monitor_t monitor;
    ringbuffer_t buffer;
    rb_init(&buffer);

    temp_monitor_init(&monitor, HW_REV_B);

    // Push multiple temperature readings
    rb_push(&buffer, 700); // 70.0°C
    rb_push(&buffer, 750); // 75.0°C
    rb_push(&buffer, 800); // 80.0°C

    temp_monitor_process(&monitor, &buffer);

    // Filtered temperature should be calculated (IIR filter)
    EXPECT_TRUE(monitor.has_sample);
    EXPECT_GE(monitor.filtered_temperature_dC, 700);
    EXPECT_LE(monitor.filtered_temperature_dC, 800);
}

TEST_F(TempMonitorTest, ISR_FillsRingBuffer) {
    ringbuffer_t buffer;
    rb_init(&buffer);

    // Start ISR with the buffer
    EXPECT_TRUE(isr_start(&buffer));

    // Let ISR run for a short time to collect some samples
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Stop ISR
    isr_stop();

    // Check that buffer has been filled
    EXPECT_GT(rb_count(&buffer), 0u);

    // Verify we can pop some values
    int16_t value;
    bool popped = rb_pop(&buffer, &value);
    EXPECT_TRUE(popped);
    // Value should be in reasonable range (mock ADC generates 0-150°C or 0-1500 d°C)
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 1500);
}