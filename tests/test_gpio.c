#include "unity.h"
#include "../project/hal/gpio.h"

void test_gpio_init(void) {
    gpio_init();
    TEST_ASSERT_FALSE(gpio_get(LED_GREEN));
    TEST_ASSERT_FALSE(gpio_get(LED_YELLOW));
    TEST_ASSERT_FALSE(gpio_get(LED_RED));
}

void test_gpio_set_get(void) {
    gpio_set(LED_GREEN, true);
    TEST_ASSERT_TRUE(gpio_get(LED_GREEN));
    gpio_set(LED_GREEN, false);
    TEST_ASSERT_FALSE(gpio_get(LED_GREEN));
}

void test_gpio_invalid_led(void) {
    gpio_set((led_t)-1, true); // Invalid
    TEST_ASSERT_FALSE(gpio_get((led_t)-1));
}