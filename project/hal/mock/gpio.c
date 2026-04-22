/* MOCK IMPLEMENTATION */

#include "hal/gpio.h"
#include <stdio.h>

static bool s_led_state[LED_COUNT] = { false };

void gpio_init(void)
{
    for (int i = 0; i < LED_COUNT; ++i) {
        s_led_state[i] = false;
    }
}

void gpio_set(led_t led, bool value)
{
    if (led < 0 || led >= LED_COUNT) {
        return;
    }

    if (s_led_state[led] == value) {
        return;
    }

    s_led_state[led] = value;
    const char *name = "UNKNOWN";
    switch (led) {
        case LED_GREEN: name = "GREEN"; break;
        case LED_YELLOW: name = "YELLOW"; break;
        case LED_RED: name = "RED"; break;
        default: break;
    }
    printf("[GPIO] LED %s = %s\n", name, value ? "ON" : "OFF");
}

bool gpio_get(led_t led)
{
    if (led < 0 || led >= LED_COUNT) {
        return false;
    }
    return s_led_state[led];
}
