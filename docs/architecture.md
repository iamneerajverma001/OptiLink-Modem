# Architecture Overview

## System Overview

The project implements a simple but extensible OptiLink modem stack across two STM32 firmware targets:
- TX.V3: generates framed packets and emits them through the hardware output path
- RX.V3: acquires the incoming signal, recovers timing, decodes payload, and reports link health

## Layered Architecture

### 1. Signal Processing Layer
Responsible for ADC acquisition, envelope tracking, slicing, and basic signal quality estimation.

### 2. Timing Recovery Layer
Responsible for clock recovery and lock tracking through the DPLL logic.

### 3. MAC / Framing Layer
Responsible for packet framing, payload extraction, CRC validation, and sequence awareness.

### 4. Diagnostics Layer
Responsible for runtime telemetry, lock-state reporting, sequence tracking, and reliability counters.

### 5. Configuration Layer
Responsible for centralizing modem parameters such as sync words, timing values, thresholds, and codec selection.

## Design Goals

- modularity
- configurability
- extensibility to future PHY schemes
- clean telemetry for debugging and validation
- maintainability for personal or academic portfolio use
