// Mock GPIO implementation for testing
#include "hal/gpio.h"

static bool mock_led_states[LED_COUNT] = {false, false, false};

void gpio_init(void) {
    for (int i = 0; i < LED_COUNT; ++i) {
        mock_led_states[i] = false;
    }
}

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

// Test helper functions
bool test_get_led_state(led_t led) {
    if (led >= 0 && led < LED_COUNT) {
        return mock_led_states[led];
    }
    return false;
}

void test_reset_led_states(void) {
    for (int i = 0; i < LED_COUNT; ++i) {
        mock_led_states[i] = false;
    }
}