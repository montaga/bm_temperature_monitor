#include "hal/eeprom.h"
#include "hal/i2c.h"
#include <string.h>

#define EEPROM_ADDR 0x50

#define REG_HW_REV 0x00
#define REG_SERIAL 0x10

bool eeprom_read_config(config_t *config)
{
    if (config == NULL)
        return false;

    uint8_t rev;

    if (!i2c_read(EEPROM_ADDR, REG_HW_REV, &rev, 1))
        return false;

    config->revision = (rev == 0) ? HW_REV_A : HW_REV_B;

    if (!i2c_read(EEPROM_ADDR, REG_SERIAL,
                  (uint8_t *)config->serial,
                  sizeof(config->serial)))
        return false;

    config->serial[sizeof(config->serial) - 1] = '\0';

    return true;
}