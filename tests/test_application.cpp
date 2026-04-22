#include <gtest/gtest.h>
#include "app/application.h"
#include "hal/adc.h"
#include "bsp/isr.h"
#include "hal/gpio.h"
#include <thread>
#include <chrono>
#include "hal/gpio.h"

// Forward declarations for test helper functions
extern "C"
{
    void test_set_mock_adc_value(int16_t value);
    bool test_get_led_state(led_t led);
    void test_reset_led_states(void);
}

class ApplicationTest : public ::testing::Test
{
protected:
    app_context_t ctx;

    void SetUp() override
    {
        // Initialize with default config (Rev-B)
        setenv("SIM_HW_REV", "B", 1);
        test_reset_led_states();
    }

    void TearDown() override
    {
        app_cleanup(&ctx);
    }

    temp_state_t GetTempState()
    {
        return ctx.monitor.state;
    }
};

TEST_F(ApplicationTest, NormalTemperatureRange)
{
    // Set ADC to return normal temperature (25°C = 250 d°C)
    test_set_mock_adc_value(250);

    ASSERT_TRUE(app_init(&ctx));

    // Run several processing cycles to let the system stabilize
    for (int i = 0; i < 10; i++)
    {
        app_process(&ctx);
    }

    // Check that temperature state is normal
    EXPECT_EQ(TEMP_STATE_NORMAL, GetTempState());

    // Check that no LEDs are on (normal state)
    EXPECT_FALSE(test_get_led_state(LED_RED));
    EXPECT_FALSE(test_get_led_state(LED_YELLOW));
    EXPECT_TRUE(test_get_led_state(LED_GREEN));
}

TEST_F(ApplicationTest, WarningTemperatureRange)
{
    ASSERT_TRUE(app_init(&ctx));

    // Set ADC to return warning temperature (90°C = 900 d°C)
    test_set_mock_adc_value(900);

    // Run several processing cycles
    for (int i = 0; i < 10; i++)
    {
        adc_isr(); // Simulate ADC ISR filling the buffer
        app_process(&ctx);
    }

    // Check that temperature state is warning
    EXPECT_EQ(TEMP_STATE_WARNING, GetTempState());

    // Check that yellow LED is on
    EXPECT_FALSE(test_get_led_state(LED_RED));
    EXPECT_TRUE(test_get_led_state(LED_YELLOW));
    EXPECT_FALSE(test_get_led_state(LED_GREEN));
}

TEST_F(ApplicationTest, CriticalHighTemperatureRange)
{

    ASSERT_TRUE(app_init(&ctx));

    // Set ADC to return critical high temperature (110°C = 1100 d°C)
    test_set_mock_adc_value(1100);

    // Run several processing cycles
    for (int i = 0; i < 10; i++)
    {
        adc_isr();
        app_process(&ctx);
    }

    // Check that temperature state is critical high
    EXPECT_EQ(TEMP_STATE_CRITICAL_HIGH, GetTempState());

    // Check that red LED is on
    EXPECT_TRUE(test_get_led_state(LED_RED));
    EXPECT_FALSE(test_get_led_state(LED_YELLOW));
    EXPECT_FALSE(test_get_led_state(LED_GREEN));
}

TEST_F(ApplicationTest, TemperatureStateTransition)
{
    ASSERT_TRUE(app_init(&ctx));

    // Start with normal temperature
    test_set_mock_adc_value(200); // 20°C
    for (int i = 0; i < 100; i++)
    {
        adc_isr();
        app_process(&ctx);
    }
    EXPECT_EQ(TEMP_STATE_NORMAL, GetTempState());

    // Transition to warning
    test_set_mock_adc_value(900); // 90°C
    for (int i = 0; i < 100; i++)
    {
        adc_isr();
        app_process(&ctx);
    }
    EXPECT_EQ(TEMP_STATE_WARNING, GetTempState());

    // Transition back to normal (with hysteresis)
    test_set_mock_adc_value(700); // 70°C - should go back to normal
    for (int i = 0; i < 100; i++)
    { // Need more cycles for filtering
        adc_isr();
        app_process(&ctx);
    }
    EXPECT_EQ(TEMP_STATE_NORMAL, GetTempState());
}

TEST_F(ApplicationTest, HardwareRevisionA)
{
    // Set environment for Rev-A
    setenv("SIM_HW_REV", "A", 1);

    ASSERT_TRUE(app_init(&ctx));

    // Set ADC to return temperature in °C (not deci-°C)
    test_set_mock_adc_value(85); // 85°C

    // Run processing cycles
    for (int i = 0; i < 10; i++)
    {
        adc_isr();
        app_process(&ctx);
    }

    // Should be in warning state (85°C > 80°C threshold)
    EXPECT_EQ(TEMP_STATE_WARNING, GetTempState());
}