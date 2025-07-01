# Code-for-Fall-Detection-System-w-STM32
STM32-based fall detection system using the ADXL345 accelerometer. Implements real-time I2C data acquisition, acceleration magnitude analysis, UART debug output, and GPIO signaling for embedded safety applications.

# STM32 Fall Detection System Using ADXL345

This repository contains embedded C code for a fall detection system built on an STM32 microcontroller using the ADXL345 accelerometer. The project demonstrates I2C sensor interfacing, real-time acceleration magnitude analysis, fall detection logic, GPIO signaling, and UART-based serial output.

## Project Overview

The repository includes two implementations:

1. **Fall Detection System (`main.c`)**  
   A complete system that initializes the ADXL345, reads and scales acceleration data, calculates total acceleration magnitude, and detects falls based on a configurable threshold. Fall events are signaled through a GPIO pin and logged via UART.

2. **Accelerometer Data Demo (`accelerometer_demo.c`)**  
   A minimal working example that initializes the ADXL345, reads raw acceleration data, computes the magnitude, and outputs the result via UART. This is useful as a standalone test or a foundation for further development.

Both files are ready to run on real STM32 hardware. With the appropriate peripheral configuration using STM32CubeMX and correct wiring to the ADXL345 sensor, these programs can be compiled and flashed directly onto supported STM32 boards.

## Hardware Requirements

- STM32 microcontroller (e.g., STM32F4 series; adaptable to other STM32 families)
- ADXL345 accelerometer (I2C version)
- Optional: LED connected to GPIO (e.g., PC7) to indicate fall detection
- UART-to-USB adapter or onboard ST-Link for serial output
- 3.3V supply (do not use 5V with the ADXL345 unless it has onboard regulation)

## Sensor Wiring

| ADXL345 Pin | STM32 Connection    |
|-------------|---------------------|
| VCC         | 3.3V                |
| GND         | GND                 |
| SDA         | I2C1_SDA (e.g., PB9)|
| SCL         | I2C1_SCL (e.g., PB8)|

Match the wiring to your STM32 pinout and ensure the `.ioc` file reflects the correct I2C pin configuration.

## STM32 Peripheral Configuration

Using STM32CubeMX or STM32CubeIDE, ensure the following are configured:

- I2C1: Enabled and mapped to the SDA/SCL pins used above
- USART2: Enabled for UART debug output (TX pin for output; typically PA2)
- GPIO: Configure a digital output pin (e.g., PC7) for signaling fall detection

Then generate the project code and replace the default `main.c` with one of the versions provided here.

## Fall Detection Logic (main.c)

The fall detection logic computes the total acceleration magnitude using the formula:

magnitude = sqrt(x² + y² + z²)

If the magnitude exceeds a defined threshold (e.g., 2.5 g), it is treated as a fall. The system then:
- Sets a GPIO pin high (e.g., lighting an LED)
- Sends a UART message indicating the fall

### UART Output Example

Accel Magnitude: 2.68 g  
Fall Detected!

### GPIO Output Behavior

- GPIO pin (e.g., PC7) is set HIGH during a detected fall, LOW otherwise.

## Accelerometer Demo (accelerometer_demo.c)

This file is a lightweight program that:
- Initializes the ADXL345 via I2C
- Reads raw X, Y, Z acceleration values
- Computes and prints the magnitude in g units via UART

This demo is fully hardware-ready. Once peripherals are configured using CubeMX, the code can be compiled and flashed directly to a board. It is ideal for quick sensor verification or integration into larger embedded systems.

## Usage Instructions

1. Clone this repository and open the project in STM32CubeIDE.
2. Use the `.ioc` file or manually configure:
   - I2C1 for ADXL345 communication
   - USART2 for UART output (baud rate: 115200)
   - GPIO (e.g., PC7) for fall detection signaling (only needed for `main.c`)
3. Wire the ADXL345 to the STM32 board according to the pinout table above.
4. Build the project and flash it to your board.
5. Open a UART terminal (e.g., PuTTY, Tera Term) at 115200 baud to observe output.
6. Move or shake the board to simulate motion or a fall.

## Files Included

- `main.c`: Complete fall detection system with threshold detection, GPIO signal output, and UART logging.
- `accelerometer_demo.c`: Minimal sensor reading and magnitude calculation with UART output only.

Only one of these files should be used as `main.c` in your project at a time.

## License

This project may include STM32CubeMX-generated code, which is subject to STMicroelectronics' licensing terms. All original code is provided for academic, educational, and demonstration purposes.





