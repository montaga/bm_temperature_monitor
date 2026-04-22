#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

#include "common/ringbuffer.h"
#include "hal/eeprom.h"

typedef enum {
    TEMP_STATE_UNKNOWN = -1,
    TEMP_STATE_CRITICAL_LOW,
    TEMP_STATE_NORMAL,
    TEMP_STATE_WARNING,
    TEMP_STATE_CRITICAL_HIGH,
} temp_state_t;

typedef struct {
    hw_revision_t revision;
    int16_t filtered_temperature_dC; // Store temperature in deci-°C to avoid floating point
    temp_state_t state;
    temp_state_t previous_state;
    bool has_sample;
    int16_t last_raw_value;
} temp_monitor_t;

void temp_monitor_init(temp_monitor_t *monitor, hw_revision_t revision);
void temp_monitor_process(temp_monitor_t *monitor, ringbuffer_t *buffer);
void temp_monitor_update_leds(temp_monitor_t *monitor);
void temp_monitor_print_status(const temp_monitor_t *monitor);

#endif // TEMP_MONITOR_H
