#include "unity.h"

// Include all test files
#include "test_ringbuffer.c"
#include "test_temp_monitor.c"
#include "test_adc.c"
#include "test_eeprom.c"
#include "test_gpio.c"

void setUp(void) {
    // Global setup if needed
}

void tearDown(void) {
    // Global cleanup if needed
}

int main(void) {
    UNITY_BEGIN();

    // Ringbuffer tests
    RUN_TEST(test_ringbuffer_init);
    RUN_TEST(test_ringbuffer_push_pop);
    RUN_TEST(test_ringbuffer_empty_pop);
    RUN_TEST(test_ringbuffer_full_push);
    RUN_TEST(test_ringbuffer_wrap_around);
    RUN_TEST(test_ringbuffer_count);

    // Temp monitor tests
    RUN_TEST(test_temp_monitor_init);
    RUN_TEST(test_temp_monitor_state_normal_to_warning);
    RUN_TEST(test_temp_monitor_state_warning_to_normal);
    RUN_TEST(test_temp_monitor_state_to_critical_high);
    RUN_TEST(test_temp_monitor_state_to_critical_low);
    RUN_TEST(test_temp_monitor_leds_update);
    RUN_TEST(test_temp_monitor_no_led_update_on_same_state);
    RUN_TEST(test_temp_monitor_conversion_rev_a);
    RUN_TEST(test_temp_monitor_conversion_rev_b);
    RUN_TEST(test_temp_monitor_filtering);

    // ADC tests
    RUN_TEST(test_adc_init_rev_a);
    RUN_TEST(test_adc_init_rev_b);
    RUN_TEST(test_adc_read_consistency);

    // EEPROM tests
    RUN_TEST(test_eeprom_read_config_default);

    // GPIO tests
    RUN_TEST(test_gpio_init);
    RUN_TEST(test_gpio_set_get);
    RUN_TEST(test_gpio_invalid_led);

    return UNITY_END();
}