# Baremetal ADC-Triggered LED Control Simulation

This project implements a PC-based simulation of a bare-metal temperature monitoring system.


## Minimalistic Solution

A first intuitive implementation could look like this:

```c

static volatile bool hardware_revision_is_rev_B;

int main() {
    hardware_revision_is_rev_B = read_hardware_revision_from_eeprom_via_i2c();

    adc_init();          // config ADC
    timer_init();        // config 100µs trigger
    enable_interrupts(); // now ISR can fire

    while(1) {}
    return 0;
}

// interrupt triggered every 100µs by hardware timer
void adc_isr(void)
{
    int16_t raw = adc_read();

    // apply hardware-dependent scaling
    int16_t temp_dC = hardware_revision_is_rev_B ? raw : (10 * raw);

    // direct decision + output in ISR
    gpio_set(LED_GREEN,  temp_dC < 850);
    gpio_set(LED_YELLOW, temp_dC >= 850);
    gpio_set(LED_RED,    temp_dC >= 1050 || temp_dC < 50);
}
```

While this solution is very compact, it does not scale well with increasing system complexity. 

Furthremore, task clarification is needed: Is only one LED active or can be multiple LEDs active. Is some flickering in the LEDs due to adc noise acceptable?

## Features of a more complex solution

- Only one LED is active.
- Simulated ADC sampling at approximately 10 kHz via ISR thread
- Lock-free single-producer single-consumer ringbuffer
- 100 Hz main loop processing
- Temperature conversion for two hardware revisions:
  - Rev-A: 1 digit = 1°C
  - Rev-B: 0.1°C per digit
- IIR filtering of temperature samples
- Hysteresis-based LED state machine for OK / Warning / Critical
- Configuration loading for hardware revision and serial number


## Dataflow diagram

![dataflow diagram](./doc/dataflow_diagram.png)

## Architecture diagram

![architecture_diagram](./doc/architecture_diagram.png)

## Build

From the repository root, build with the mock HAL implementation (default):

```sh
make
```

Or explicitly specify the HAL implementation:

```sh
# Build with mock HAL (PC simulation)
make HAL_IMPL=mock

# Build with hardware HAL (bare-metal target)
make HAL_IMPL=hw
```

For more options:

```sh
make help
```
## Running the mock application

After `make`, the mock-application can be executed using

`SIM_HW_REV=B ./simulated_temperature_monitor`
or
`SIM_HW_REV=B ./simulated_temperature_monitor`
depending on the needed hardware revision.

## Testing

The project uses Google Test for unit testing of application logic.

```sh
make test
```

This will:
- Download and build Google Test automatically
- Compile and run tests for temperature monitoring logic
- Compile and run tests for configuration loading
- All HAL implementations are mocked during testing

## Project Structure

- `project/main.c` - application entry and simulation orchestration
- `project/app/` - application logic
  - `application.h/c` - the main application loop and component orchestration
  - `config_loader.h/c` - configuration loading from I2C EEPROM
  - `temp_monitor.h/c` - temperature monitoring and LED control logic
- `project/hal/` - Hardware Abstraction Layer
  - `*.h` - platform-independent HAL interfaces
  - `mock/` - mock implementations for PC simulation
  - `hw/` - hardware placeholder implementations for bare-metal targets
- `project/bsp/` - ISR simulation thread
- `project/common/` - lock-free ringbuffer implementation
- `tests/` - Google Test suite for application logic testing

## Hardware Abstraction Layer (HAL)

The HAL provides a clean, platform-independent interface for hardware operations:

### Interfaces
- `adc.h` - ADC sampling and initialization
- `gpio.h` - LED control (GREEN, YELLOW, RED)
- `i2c.h` - I2C communication protocol

### Implementations

**Mock Implementation** (`hal/mock/`)
- Used in PC simulation mode (default)
- Simulates sensors with realistic data patterns
- Provides debug output for testing

**Hardware Implementation** (`hal/hw/`)
- Placeholder for bare-metal targets
- Ready for real hardware drivers
- Use with `make HAL_IMPL=hw`

### Design

The application depends only on HAL interfaces (`.h` files), not implementation details. This allows seamless switching between mock and hardware implementations at compile time via the `HAL_IMPL` flag.


## Notes

- The ISR thread is intentionally minimal: it only reads the ADC and pushes to the ringbuffer.
- The main thread handles filtering, state transitions, and LED updates.
- Application code is completely decoupled from HAL implementation details.
- Mock implementation provides realistic sensor simulation for PC testing.
- Hardware implementation is ready for expansion with real device drivers.
