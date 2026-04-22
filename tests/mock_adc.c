/* Test-specific mock ADC implementation */

#include "hal/adc.h"
#include <stdint.h>

static int16_t mock_adc_value = 750; // Default 75°C in deci-°C

void adc_init(hw_revision_t revision)
{
    (void)revision; // Not used in test mock
}

// Set the mock ADC value for testing
void test_set_mock_adc_value(int16_t value)
{
    mock_adc_value = value;
}

// Mock ADC read function for testing
int16_t adc_read(void)
{
    return mock_adc_value;
}