#include "app/temp_monitor.h"
#include "hal/gpio.h"
#include <stdio.h>
#include <math.h>

#define TEMP_HYSTERESIS_dC 10 // 1.0f in deci-°C

#define TEMP_WARNING_ENTER_dC 850        // 85°C in deci-°C
#define TEMP_CRITICAL_HIGH_ENTER_dC 1050 // 105°C in deci-°C
#define TEMP_CRITICAL_LOW_ENTER_dC 50    // 5.°C in deci-°C

static int16_t convert_raw_to_decicelsius(hw_revision_t revision, int16_t raw)
{
    if (revision == HW_REV_A)
    {
        return raw * 10; // Rev-A gives temperature in °C, convert to deci-°C for better precision
    }
    else if (revision == HW_REV_B)
    {
        return raw; // Rev-B already gives temperature in deci-°C
    }
    return 0; // Default case
}

static temp_state_t temperature_to_target_state(int16_t temperature_dC)
{
    // convert temperature in deci-°C to target state without hysteresis, used for state transitions
    if (temperature_dC >= TEMP_CRITICAL_HIGH_ENTER_dC)
    {
        return TEMP_STATE_CRITICAL_HIGH;
    }
    else if (temperature_dC >= TEMP_WARNING_ENTER_dC)
    {
        return TEMP_STATE_WARNING;
    }
    else if (temperature_dC < TEMP_CRITICAL_LOW_ENTER_dC)
    {
        return TEMP_STATE_CRITICAL_LOW;
    }
    else
    {
        return TEMP_STATE_NORMAL;
    }
}

static temp_state_t update_state_with_hysteresis(temp_state_t current, int16_t temperature_dC)
{
    // update state based on current state and temperature with hysteresis to prevent rapid toggling

    if (current == TEMP_STATE_UNKNOWN)
    {
        // no previous state, determine initial state without hysteresis
        return temperature_to_target_state(temperature_dC);
    }
    if (current == TEMP_STATE_CRITICAL_LOW)
    {
        // go up states with hysteresis to normal state with hysteresis
        return temperature_to_target_state(temperature_dC - TEMP_HYSTERESIS_dC);
    }
    else if (current == TEMP_STATE_NORMAL)
    {
        // go to warning or critical high/low immediately when crossing thresholds
        return temperature_to_target_state(temperature_dC);
    }
    else // (current > TEMP_STATE_WARNING or current == TEMP_STATE_CRITICAL_HIGH)
    {
        temp_state_t up_state = temperature_to_target_state(temperature_dC);
        temp_state_t down_state = temperature_to_target_state(temperature_dC + TEMP_HYSTERESIS_dC);
        if (up_state >= current)
        {
            return up_state; // go up immediately when crossing thresholds
        }
        else
        {
            return down_state; // go down with hysteresis
        }
    }
}

void temp_monitor_init(temp_monitor_t *monitor, hw_revision_t revision)
{
    monitor->revision = revision;
    monitor->filtered_temperature_dC = 200; // 20°C in deci-°C
    monitor->state = TEMP_STATE_UNKNOWN;
    monitor->previous_state = TEMP_STATE_UNKNOWN;
    monitor->has_sample = false;
}

void temp_monitor_process(temp_monitor_t *monitor, ringbuffer_t *buffer)
{
    int16_t raw_value;

    while (rb_pop(buffer, &raw_value))
    {
        const int16_t current_temperature_dC = convert_raw_to_decicelsius(monitor->revision, raw_value);
        if (monitor->has_sample)
        {
            // IIR low-pass filter with alpha = 1/8 for smoothing temperature readings
            // monitor->filtered_temperature_dC = (1.0f - alpha) * monitor->filtered_temperature_dC + alpha * current_temperature_dC
            // for alpha = 1/8, this can be implemented efficiently as:
            // monitor->filtered_temperature_dC = (monitor->filtered_temperature_dC * 7 + current_temperature_dC) / 8;
            // or even quicker as
            monitor->filtered_temperature_dC += (current_temperature_dC - monitor->filtered_temperature_dC) >> 3;
        }
        else
        {
            monitor->filtered_temperature_dC = current_temperature_dC; // Initialize filter with first sample
            monitor->has_sample = true;
        }

        monitor->last_raw_value = raw_value;
    }
    monitor->state = update_state_with_hysteresis(monitor->state, monitor->filtered_temperature_dC);
}

void temp_monitor_update_leds(temp_monitor_t *monitor)
{
    // Only update GPIO if state has changed
    if (monitor->state == monitor->previous_state)
    {
        return;
    }

    gpio_set(LED_GREEN, monitor->state == TEMP_STATE_NORMAL);
    gpio_set(LED_YELLOW, monitor->state == TEMP_STATE_WARNING);
    gpio_set(LED_RED, monitor->state == TEMP_STATE_CRITICAL_HIGH || monitor->state == TEMP_STATE_CRITICAL_LOW);

    monitor->previous_state = monitor->state;
}

void temp_monitor_print_status(const temp_monitor_t *monitor)
{
    // only for testing purposes, should not be used in production code
    const char *state_name = "UNKNOWN";
    switch (monitor->state)
    {
    case TEMP_STATE_UNKNOWN:
        state_name = "UNKNOWN";
        break;
    case TEMP_STATE_NORMAL:
        state_name = "GREEN";
        break;
    case TEMP_STATE_WARNING:
        state_name = "YELLOW";
        break;
    case TEMP_STATE_CRITICAL_LOW:
        state_name = "RED (LOW)";
        break;
    case TEMP_STATE_CRITICAL_HIGH:
        state_name = "RED (HIGH)";
        break;
    }
    printf("[STATUS] last_raw_value=%d filtered temperature=%.2f°C state=%s\n", monitor->last_raw_value, monitor->filtered_temperature_dC / 10.0f, state_name);
}
