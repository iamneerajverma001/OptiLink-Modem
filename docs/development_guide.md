# Development Guide

This guide describes the practical setup and build flow for the OptiLink modem project.

## Build Requirements

- ARM GCC toolchain
- CMake
- STM32CubeMX-style HAL setup
- A compatible STM32F412 build environment

## Building

Run the build from each project folder:

```bash
cmake -S . -B build/Debug -G Ninja
cmake --build build/Debug
```

## Project Notes

- The transmitter and receiver are designed as standalone firmware targets.
- The codebase is intentionally structured to support future hardware validation and protocol upgrades.
