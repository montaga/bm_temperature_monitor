#include "app/application.h"
#include "hal/adc.h"
#include "hal/gpio.h"
#include "bsp/isr.h"
#include <stdio.h>
#include <string.h>

bool app_init(app_context_t *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }

    memset(ctx, 0, sizeof(app_context_t));

    // Load configuration
    if (!config_load(&ctx->config))
    {
        fprintf(stderr, "ERROR: Failed to load configuration\n");
        return false;
    }

    printf("[BOOT] Hardware revision: %s\n",
           ctx->config.revision == HW_REV_A ? "Rev-A" : "Rev-B");
    printf("[BOOT] Serial number: %s\n", ctx->config.serial);

    // Initialize hardware
    adc_init(ctx->config.revision);
    gpio_init();

    // Initialize ringbuffer
    rb_init(&ctx->sensor_buffer);

    // Start ISR
    if (!isr_start(&ctx->sensor_buffer))
    {
        fprintf(stderr, "ERROR: Failed to start ISR\n");
        return false;
    }

    // Initialize temperature monitor
    temp_monitor_init(&ctx->monitor, ctx->config.revision);

    ctx->initialized = true;
    return true;
}

void app_process(app_context_t *ctx)
{
    if (ctx == NULL || !ctx->initialized)
    {
        return;
    }

    temp_monitor_process(&ctx->monitor, &ctx->sensor_buffer);
    temp_monitor_update_leds(&ctx->monitor);
    temp_monitor_print_status(&ctx->monitor);
}

void app_cleanup(app_context_t *ctx)
{
    if (ctx == NULL || !ctx->initialized)
    {
        return;
    }

    isr_stop();
    ctx->initialized = false;
}
