#include "hal/adc.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static hw_revision_t s_revision = HW_REV_B;
static uint32_t s_sample_index = 0;

void adc_init(hw_revision_t revision)
{
    s_revision = revision;
    s_sample_index = 0;
}

int16_t adc_read(void)
{
    // simulate temperature between 0 and 150°C
    const float time = s_sample_index * 0.0001f;
    const float noise = rand() / (float)RAND_MAX * 5.0f - 2.5f; // random noise between -1 and +1
    float temperature = sinf(time) * 75.0f + 75.0f + noise;
    s_sample_index++;

    if (s_revision == HW_REV_A)
    {
        return (int16_t)(temperature + 0.5f);
    }
    else if (s_revision == HW_REV_B)
    {
        return (int16_t)(temperature * 10.0f + 0.5f);
    }
    return 0; // Default case
}
