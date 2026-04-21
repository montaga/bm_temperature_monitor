# Baremetal ADC-Triggered LED Control Simulation

This project implements a PC-based simulation of a bare-metal temperature monitoring system.

## Features

- Simulated ADC sampling at approximately 10 kHz via ISR thread
- Lock-free single-producer single-consumer ringbuffer
- 100 Hz main loop processing
- Temperature conversion for two hardware revisions:
  - Rev-A: 1 digit = 1°C
  - Rev-B: 0.1°C per digit
- IIR filtering of temperature samples
- Hysteresis-based LED state machine for OK / Warning / Critical
- EEPROM configuration mock for hardware revision and serial number

## Build

From the repository root:

```sh
make
```

## Run

```sh
./simulated_temperature_monitor
```

To simulate Rev-A behavior:

```shn
SIM_HW_REV=A ./simulated_temperature_monitor
```

## Project Structure

- `project/main.c` - application entry and simulation orchestration
- `project/app/` - temperature monitor logic
- `project/hal/` - mocked ADC, GPIO, and EEPROM interfaces
- `project/bsp/` - ISR simulation thread
- `project/common/` - lock-free ringbuffer implementation

## Notes

- The ISR thread is intentionally minimal: it only reads the ADC and pushes to the ringbuffer.
- The main thread handles filtering, state transitions, and LED updates.
- No real hardware is required; the ADC and EEPROM are mocked for PC demonstration.
