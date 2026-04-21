CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iproject -g
LDFLAGS = -pthread -lm
SRC = \
    project/main.c \
    project/app/temp_monitor.c \
    project/hal/adc.c \
    project/hal/gpio.c \
    project/hal/eeprom.c \
    project/bsp/isr.c \
    project/common/ringbuffer.c

TARGET = simulated_temperature_monitor

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) -D EMULATED_ISR

clean:
	rm -f $(TARGET)
