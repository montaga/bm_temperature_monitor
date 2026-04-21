#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "hal/eeprom.h"

void adc_init(hw_revision_t revision);
int16_t adc_read(void);


#endif // ADC_H
