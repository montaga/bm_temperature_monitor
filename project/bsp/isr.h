#ifndef ISR_H
#define ISR_H

#include <stdbool.h>
#include "common/ringbuffer.h"

void adc_isr(void);
bool isr_start(ringbuffer_t *buffer);
void isr_stop(void);

#endif // ISR_H
