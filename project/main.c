#define _XOPEN_SOURCE 600

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "app/application.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    app_context_t ctx;

    if (!app_init(&ctx))
    {
        fprintf(stderr, "ERROR: Failed to initialize application\n");
        return EXIT_FAILURE;
    }

    // main loop controlling the LEDs based on the latest temperature reading and state
    const int loop_interval_us = 10000; // no more than 100 Hz is needed for LED updates
    while (true)
    {
        app_process(&ctx);
        usleep(loop_interval_us);
    }

    app_cleanup(&ctx);
    return EXIT_SUCCESS;
}
