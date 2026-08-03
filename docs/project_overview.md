# Project Overview

This repository documents an OptiLink modem-style firmware stack designed to be practical, modular, and configurable. The project focuses on STM32-based embedded implementation with a layered architecture for optical communication.

## Objectives

- provide a practical embedded modem approach,
- separate protocol and signal-processing concerns,
- present the work clearly for technical review and portfolio use.

## Project intent

The project is designed to show how data can be prepared for transmission, delivered through an optical link, received, decoded, and validated through structured firmware layers.

```mermaid
flowchart LR
    A[Payload Input] --> B[TX Firmware]
    B --> C[Frame Builder]
    C --> D[Optical Channel]
    D --> E[RX Firmware]
    E --> F[Decoder and Validator]
    F --> G[Telemetry Output]
```

## Direction

The current implementation provides a solid foundation for future improvement in reliability, adaptability, and communication performance.
