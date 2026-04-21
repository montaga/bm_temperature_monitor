#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "hal/eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_init(hw_revision_t revision);
int16_t adc_read(void);

#ifdef __cplusplus
}
#endif

#endif // ADC_H
