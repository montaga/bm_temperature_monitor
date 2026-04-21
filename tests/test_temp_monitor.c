#include "unity.h"
#include "../project/app/temp_monitor.h"
#include "../project/common/ringbuffer.h"
#include "../project/hal/gpio.h"

// Mock GPIO functions
static bool mock_led_states[3] = {false, false, false};

void gpio_set(led_t led, bool value) {
    if (led >= 0 && led < LED_COUNT) {
        mock_led_states[led] = value;
    }
}

bool gpio_get(led_t led) {
    if (led >= 0 && led < LED_COUNT) {
        return mock_led_states[led];
    }
    return false;
}

void gpio_init(void) {
    for (int i = 0; i < LED_COUNT; ++i) {
        mock_led_states[i] = false;
    }
}

void test_temp_monitor_init(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);
    TEST_ASSERT_EQUAL(HW_REV_A, monitor.revision);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, monitor.filtered_temperature_c);
    TEST_ASSERT_EQUAL(TEMP_STATE_UNKNOWN, monitor.state);
    TEST_ASSERT_FALSE(monitor.has_sample);
}

void test_temp_monitor_state_normal_to_warning(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    // Simulate temperature at 85°C (warning threshold)
    rb_push(&rb, 85); // Raw value for Rev-A
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL(TEMP_STATE_WARNING, monitor.state);
}

void test_temp_monitor_state_warning_to_normal(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    // First go to warning
    rb_push(&rb, 85);
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL(TEMP_STATE_WARNING, monitor.state);

    // Then back to normal with hysteresis
    rb_push(&rb, 60); // Low enough to drop below 79
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL(TEMP_STATE_NORMAL, monitor.state);
}

void test_temp_monitor_state_to_critical_high(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    rb_push(&rb, 105); // Critical high
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL(TEMP_STATE_CRITICAL_HIGH, monitor.state);
}

void test_temp_monitor_state_to_critical_low(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    rb_push(&rb, 4); // Critical low
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL(TEMP_STATE_CRITICAL_LOW, monitor.state);
}

void test_temp_monitor_leds_update(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);

    // Normal state
    monitor.state = TEMP_STATE_NORMAL;
    temp_monitor_update_leds(&monitor);
    TEST_ASSERT_TRUE(gpio_get(LED_GREEN));
    TEST_ASSERT_FALSE(gpio_get(LED_YELLOW));
    TEST_ASSERT_FALSE(gpio_get(LED_RED));

    // Warning state
    monitor.state = TEMP_STATE_WARNING;
    temp_monitor_update_leds(&monitor);
    TEST_ASSERT_FALSE(gpio_get(LED_GREEN));
    TEST_ASSERT_TRUE(gpio_get(LED_YELLOW));
    TEST_ASSERT_FALSE(gpio_get(LED_RED));

    // Critical state
    monitor.state = TEMP_STATE_CRITICAL_HIGH;
    temp_monitor_update_leds(&monitor);
    TEST_ASSERT_FALSE(gpio_get(LED_GREEN));
    TEST_ASSERT_FALSE(gpio_get(LED_YELLOW));
    TEST_ASSERT_TRUE(gpio_get(LED_RED));
}

void test_temp_monitor_no_led_update_on_same_state(void) {
    temp_monitor_t monitor = {0};
    temp_monitor_init(&monitor, HW_REV_A);

    monitor.state = TEMP_STATE_NORMAL;
    temp_monitor_update_leds(&monitor);  // This should set LED_GREEN to true
    TEST_ASSERT_TRUE(gpio_get(LED_GREEN));

    monitor.previous_state = monitor.state;  // Now same as current
    temp_monitor_update_leds(&monitor);  // Should not change
    TEST_ASSERT_TRUE(gpio_get(LED_GREEN));  // Remains true
}

void test_temp_monitor_conversion_rev_a(void) {
    temp_monitor_t monitor;
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    rb_push(&rb, 25);
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, monitor.filtered_temperature_c);
}

void test_temp_monitor_conversion_rev_b(void) {
    temp_monitor_t monitor;
    temp_monitor_init(&monitor, HW_REV_B);
    ringbuffer_t rb;
    rb_init(&rb);

    rb_push(&rb, 250); // 25.0°C
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, monitor.filtered_temperature_c);
}

void test_temp_monitor_filtering(void) {
    temp_monitor_t monitor;
    temp_monitor_init(&monitor, HW_REV_A);
    ringbuffer_t rb;
    rb_init(&rb);

    rb_push(&rb, 20);
    temp_monitor_process(&monitor, &rb);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, monitor.filtered_temperature_c);

    rb_push(&rb, 30);
    temp_monitor_process(&monitor, &rb);
    // alpha = 0.1, so 20 + 0.1*(30-20) = 21
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 21.0f, monitor.filtered_temperature_c);
}