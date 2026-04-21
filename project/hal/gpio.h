#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_GREEN,
    LED_YELLOW,
    LED_RED,
    LED_COUNT,
} led_t;

void gpio_init(void);
void gpio_set(led_t led, bool value);
bool gpio_get(led_t led);

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
