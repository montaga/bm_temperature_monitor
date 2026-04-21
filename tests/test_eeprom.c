#include "unity.h"
#include "../project/hal/eeprom.h"
#include <stdlib.h>

void test_eeprom_read_config_default(void) {
    config_t config;
    TEST_ASSERT_TRUE(eeprom_read_config(&config));
    TEST_ASSERT_EQUAL(HW_REV_B, config.revision); // Default
    TEST_ASSERT_EQUAL_STRING("ABC1234", config.serial);
}