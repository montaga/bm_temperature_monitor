/* MOCK IMPLEMENTATION */

#include "hal/adc.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static hw_revision_t s_revision = HW_REV_B;
static uint32_t s_sample_index = 0;
static int16_t s_test_value = -1; // -1 means use normal simulation, otherwise return this value


void adc_init(hw_revision_t revision)
{
    s_revision = revision;
    s_sample_index = 0;
    s_test_value = -1;
}

// Test function to set a fixed ADC return value for testing
void test_set_mock_adc_value(int16_t value)
{
    s_test_value = value;
}

// Mock ADC read function simulating a temperature sensor with noise and a sine wave pattern
int16_t adc_read(void)
{
    // If test value is set, return it instead of simulation
    if (s_test_value != -1)
    {
        return s_test_value;
    }

    // alternating simulate temperature between 0 and 150°C
    const float time = s_sample_index * 0.0001f;
    const float noise = rand() / (float)RAND_MAX * 5.0f - 2.5f; // random noise between -1 and +1 °C
    float temperature_C = sinf(time * 5.0f) * 75.0f + 75.0f + noise;
    s_sample_index++;

    if (s_revision == HW_REV_A)
    {
        return (int16_t)(temperature_C + 0.5f);
    }
    else if (s_revision == HW_REV_B)
    {
        return (int16_t)(temperature_C * 10.0f + 0.5f);
    }
    return 0; // Default case
}
