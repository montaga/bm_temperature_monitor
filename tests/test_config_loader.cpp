#include <gtest/gtest.h>
#include "app/config_loader.h"
#include <cstring>

TEST(ConfigLoaderTest, LoadDefaultConfig) {
    config_t config;

    // Set environment variable for Rev-B (default)
    setenv("SIM_HW_REV", "B", 1);

    bool result = config_load(&config);

    EXPECT_TRUE(result);
    EXPECT_EQ(HW_REV_B, config.revision);
    EXPECT_STREQ("ABC1234", config.serial);
}

TEST(ConfigLoaderTest, LoadRevAConfig) {
    config_t config;

    // Set environment variable for Rev-A
    setenv("SIM_HW_REV", "A", 1);

    bool result = config_load(&config);

    EXPECT_TRUE(result);
    EXPECT_EQ(HW_REV_A, config.revision);
    EXPECT_STREQ("ABC1234", config.serial);
}

TEST(ConfigLoaderTest, LoadRevBConfig) {
    config_t config;

    // Set environment variable for Rev-B
    setenv("SIM_HW_REV", "B", 1);

    bool result = config_load(&config);

    EXPECT_TRUE(result);
    EXPECT_EQ(HW_REV_B, config.revision);
    EXPECT_STREQ("ABC1234", config.serial);
}

TEST(ConfigLoaderTest, SerialNullTermination) {
    config_t config;

    setenv("SIM_HW_REV", "B", 1);
    bool result = config_load(&config);

    EXPECT_TRUE(result);
    // Check that serial is null-terminated and doesn't exceed buffer size
    EXPECT_LT(strlen(config.serial), sizeof(config.serial));
    EXPECT_EQ('\0', config.serial[sizeof(config.serial) - 1]);
}