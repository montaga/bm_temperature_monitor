#include "hal/eeprom.h"
#include <stdlib.h>
#include <string.h>

bool eeprom_read_config(config_t *config)
{
    if (config == NULL) {
        return false;
    }

    const char *env_rev = getenv("SIM_HW_REV");
    if (env_rev != NULL && env_rev[0] == 'A') {
        config->revision = HW_REV_A;
    } else {
        config->revision = HW_REV_B;
    }

    strncpy(config->serial, "ABC1234", sizeof(config->serial));
    config->serial[sizeof(config->serial) - 1] = '\0';
    return true;
}
