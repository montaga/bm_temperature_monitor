#define _XOPEN_SOURCE 600

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "app/temp_monitor.h"
#include "bsp/isr.h"
#include "common/ringbuffer.h"
#include "hal/adc.h"
#include "hal/eeprom.h"
#include "hal/gpio.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    config_t config;
    if (!eeprom_read_config(&config))
    {
        fprintf(stderr, "ERROR: Failed to read EEPROM config\n");
        return EXIT_FAILURE;
    }

    printf("[BOOT] Hardware revision: %s\n",
           config.revision == HW_REV_A ? "Rev-A" : "Rev-B");
    printf("[BOOT] Serial number: %s\n", config.serial);

    adc_init(config.revision);
    gpio_init();

    ringbuffer_t sensor_buffer;
    rb_init(&sensor_buffer);

#ifdef EMULATED_ISR
    if (!isr_start(&sensor_buffer))
    {
        fprintf(stderr, "ERROR: Failed to start ISR thread\n");
        return EXIT_FAILURE;
    }
#endif

    temp_monitor_t monitor;
    temp_monitor_init(&monitor, config.revision);

    // main loop controlling the LEDs based on the latest temperature reading and state
    const int loop_interval_us = 10000; // no more than 100 Hz is needed for LED updates
    while (true)
    {
        temp_monitor_process(&monitor, &sensor_buffer);
        temp_monitor_update_leds(&monitor);
        temp_monitor_print_status(&monitor);
        usleep(loop_interval_us);
    }

#ifdef EMULATED_ISR
    isr_stop();
#endif
    return EXIT_SUCCESS;
}
