#ifndef EEPROM_H
#define EEPROM_H

#include <stdbool.h>

typedef enum {
    HW_REV_A = 0,
    HW_REV_B = 1,
} hw_revision_t;

typedef struct {
    hw_revision_t revision;
    char serial[16];
} config_t;

bool eeprom_read_config(config_t *config);

#endif // EEPROM_H
