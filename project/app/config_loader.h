#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdbool.h>


#define EEPROM_ADDR 0x50

#define REG_HW_REV 0x00
#define REG_SERIAL 0x10

typedef enum {
    HW_REV_A = 0,
    HW_REV_B = 1,
} hw_revision_t;

typedef struct {
    hw_revision_t revision;
    char serial[16];
} config_t;

bool config_load(config_t *config);

#endif // CONFIG_LOADER_H
