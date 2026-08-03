# Setup and Usage

## 1. Prerequisites

Before building the firmware, make sure you have:

- ARM GCC toolchain
- CMake
- Ninja
- an STM32 build environment compatible with the provided firmware projects

## 2. Build the projects

From the repository root, build each firmware target separately:

```bash
cmake -S RX.V3 -B RX.V3/build/Debug -G Ninja
cmake --build RX.V3/build/Debug

cmake -S TX.V3 -B TX.V3/build/Debug -G Ninja
cmake --build TX.V3/build/Debug
```

## 3. What to expect

- RX.V3 builds the receiver firmware
- TX.V3 builds the transmitter firmware
- the output binaries are generated in each project build directory

## 4. Basic configuration points

The project is structured so the main behavior can be adjusted through configuration values used by both TX and RX.

### TX-side configuration

Typical TX settings include:
- frame format
- sync pattern
- payload size
- symbol timing
- output amplitude or drive level

### RX-side configuration

Typical RX settings include:
- threshold level
- sampling timing offset
- timing recovery gain
- sync detection window
- packet timeout
- lock threshold

## 5. Synchronization concept

For the transmitter and receiver to work together, both sides must agree on the same timing model.

The sampling time is modeled as:

$$t_k = kT_s + \tau$$

where:
- $t_k$ is the sampling instant,
- $T_s$ is the symbol period,
- $k$ is the sample index,
- $\tau$ is the timing offset.

The receiver updates timing using:

$$\tau_{n+1} = \tau_n + \mu e[n]$$

where:
- $\mu$ is the timing recovery gain,
- $e[n]$ is the timing error estimate.

## 6. Practical usage flow

1. Build both firmware projects.
2. Flash the TX firmware to the transmitter board.
3. Flash the RX firmware to the receiver board.
4. Start the system and verify that the receiver detects and decodes transmitted frames.
5. Tune the TX/RX configuration values if the link is weak or unstable.

## 7. Notes

- The project is intended to be easy to understand and extend.
- The firmware layers are separated so the communication logic can be adjusted without rewriting the whole system.
