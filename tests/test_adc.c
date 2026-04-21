#include "unity.h"
#include "../project/hal/adc.h"

void test_adc_init_rev_a(void) {
    adc_init(HW_REV_A);
    // Test by reading a value
    uint16_t value = adc_read();
    TEST_ASSERT_GREATER_OR_EQUAL(0, value);
    TEST_ASSERT_LESS_OR_EQUAL(130, value);
}

void test_adc_init_rev_b(void) {
    adc_init(HW_REV_B);
    uint16_t value = adc_read();
    TEST_ASSERT_GREATER_OR_EQUAL(0, value);
    TEST_ASSERT_LESS_OR_EQUAL(1300, value); // 130 * 10
}

void test_adc_read_consistency(void) {
    adc_init(HW_REV_A);
    uint16_t val1 = adc_read();
    uint16_t val2 = adc_read();
    // Values should be similar (simulated wave)
    TEST_ASSERT_UINT16_WITHIN(50, val1, val2);
}