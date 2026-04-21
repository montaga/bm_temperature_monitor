CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iproject -Itests -g
LDFLAGS = -pthread -lm
SRC = \
    project/main.c \
    project/app/temp_monitor.c \
    project/hal/adc.c \
    project/hal/gpio.c \
    project/hal/eeprom.c \
    project/bsp/isr.c \
    project/common/ringbuffer.c

SRC_TEST = \
    project/app/temp_monitor.c \
    project/hal/adc.c \
    project/hal/eeprom.c \
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
