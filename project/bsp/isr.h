#ifndef ISR_H
#define ISR_H

#include <stdbool.h>
#include "common/ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_isr(void);
bool isr_start(ringbuffer_t *buffer);
void isr_stop(void);

#ifdef __cplusplus
}
#endif

#endif // ISR_H
