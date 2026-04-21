#include "app/temp_monitor.h"
#include "hal/gpio.h"
#include <stdio.h>
#include <math.h>

#define HYSTERESIS 1.0f

#define TEMP_WARNING_ENTER 85.0f
#define TEMP_CRITICAL_HIGH_ENTER 105.0f
#define TEMP_CRITICAL_LOW_ENTER 5.0f

static float convert_raw_to_celsius(hw_revision_t revision, int16_t raw)
{
    if (revision == HW_REV_A)
    {
        return (float)raw;
    }
    else if (revision == HW_REV_B)
    {
        return raw * 0.1f;
    }
    return 0.0f; // Default case
}

static temp_state_t temperature_to_target_state(float temperature)
{
    // convert temperature to target state without hysteresis, used for state transitions
    if (temperature >= TEMP_CRITICAL_HIGH_ENTER)
    {
        return TEMP_STATE_CRITICAL_HIGH;
    }
    else if (temperature >= TEMP_WARNING_ENTER)
    {
        return TEMP_STATE_WARNING;
    }
    else if (temperature < TEMP_CRITICAL_LOW_ENTER)
    {
        return TEMP_STATE_CRITICAL_LOW;
    }
    else
    {
        return TEMP_STATE_NORMAL;
    }
}

static temp_state_t update_state_with_hysteresis(temp_state_t current, float temperature)
{
    // update state based on current state and temperature with hysteresis to prevent rapid toggling

    if (current == TEMP_STATE_UNKNOWN)
    {
        // no previous state, determine initial state without hysteresis
        return temperature_to_target_state(temperature);
    }
    if (current == TEMP_STATE_CRITICAL_LOW)
    {
        // go up states with hysteresis to normal state with hysteresis
        return temperature_to_target_state(temperature - HYSTERESIS);
    }
    else if (current == TEMP_STATE_NORMAL)
    {
        // go to warning or critical high/low immediately when crossing thresholds
        return temperature_to_target_state(temperature);
    }
    else // (current > TEMP_STATE_WARNING or current == TEMP_STATE_CRITICAL_HIGH)
    {
        temp_state_t up_state = temperature_to_target_state(temperature);
        temp_state_t down_state = temperature_to_target_state(temperature + HYSTERESIS);
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
    monitor->filtered_temperature_c = 20.0f;
    monitor->state = TEMP_STATE_UNKNOWN;
    monitor->previous_state = TEMP_STATE_UNKNOWN;
    monitor->has_sample = false;
}

void temp_monitor_process(temp_monitor_t *monitor, ringbuffer_t *buffer)
{
    const float alpha = 0.10f;
    int16_t raw_value;

    while (rb_pop(buffer, &raw_value))
    {
        const float measured = convert_raw_to_celsius(monitor->revision, raw_value);
        monitor->filtered_temperature_c = monitor->has_sample ? (1.0f - alpha) * monitor->filtered_temperature_c + alpha * measured : measured;
        monitor->has_sample = true;
        monitor->last_raw_value = raw_value;
    }
    monitor->state = update_state_with_hysteresis(monitor->state, monitor->filtered_temperature_c);
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
    printf("[STATUS] last_raw_value=%d filtered temperature=%.2f°C state=%s\n", monitor->last_raw_value, monitor->filtered_temperature_c, state_name);
}
