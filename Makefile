CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iproject -Itests -g
LDFLAGS = -pthread -lm

# HAL Implementation: mock or hw
# Set HAL_IMPL=hw for hardware implementation, defaults to mock
HAL_IMPL ?= mock

# Select implementation files based on HAL_IMPL
ifeq ($(HAL_IMPL),mock)
    HAL_IMPL_FLAG = -DHAL_IMPL_MOCK
    HAL_ADC = project/hal/mock/adc.c
    HAL_GPIO = project/hal/mock/gpio.c
    HAL_I2C = project/hal/mock/i2c.c
else ifeq ($(HAL_IMPL),hw)
    HAL_IMPL_FLAG = -DHAL_IMPL_HW
    HAL_ADC = project/hal/hw/adc.c
    HAL_GPIO = project/hal/hw/gpio.c
    HAL_I2C = project/hal/hw/i2c.c
else
    $(error Invalid HAL_IMPL value: $(HAL_IMPL). Use 'mock' or 'hw')
endif

CFLAGS += $(HAL_IMPL_FLAG)

SRC = \
    project/main.c \
    project/app/config_loader.c \
    project/app/temp_monitor.c \
    $(HAL_ADC) \
    $(HAL_GPIO) \
    $(HAL_I2C) \
    project/bsp/isr.c \
    project/common/ringbuffer.c

SRC_TEST = \
    project/app/config_loader.c \
    project/app/temp_monitor.c \
    $(HAL_ADC) \
    $(HAL_I2C) \
    project/bsp/isr.c \
    project/common/ringbuffer.c

TARGET = simulated_temperature_monitor
TEST_TARGET = run_tests

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -D EMULATED_ISR

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): tests/all_tests.c tests/unity.c $(SRC_TEST)
	$(CC) $(CFLAGS) tests/all_tests.c tests/unity.c $(SRC_TEST) -o $(TEST_TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TEST_TARGET)

.PHONY: all test clean help

help:
	@echo "Make targets:"
	@echo "  make                  - Build with mock HAL (default)"
	@echo "  make HAL_IMPL=mock    - Build with mock HAL implementation"
	@echo "  make HAL_IMPL=hw      - Build with hardware HAL implementation"
	@echo "  make test             - Run unit tests with mock HAL"
	@echo "  make test HAL_IMPL=hw - Run unit tests with hardware HAL"
	@echo "  make clean            - Remove build artifacts"
