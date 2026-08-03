# Usage Guide

## Hardware Setup

- Use the TX firmware on the transmitter board
- Use the RX firmware on the receiver board
- connect the UART console for runtime telemetry

## Running the Firmware

1. Build the RX firmware
2. Build the TX firmware
3. Flash both boards
4. Open a serial terminal at 115200 baud
5. Send text from the TX terminal to observe RX decoding and diagnostics

## Expected Output

The transmitter will print transmit progress and message status.
The receiver will print decoded payloads along with link metrics such as:
- lock state
- sync state
- packet success/failure
- CRC and error counts
- amplitude and noise information
