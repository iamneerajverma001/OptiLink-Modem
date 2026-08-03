# Block Diagram

```text
User Input
   |
   v
TX Firmware
  - frame builder
  - codec / symbol mapper
  - timing / pacing
  - GPIO / optical output

Optical Channel
   |
   v
RX Firmware
  - ADC acquisition
  - signal processing
  - timing recovery
  - frame decode
  - diagnostics / telemetry
```

This diagram captures the intended modem flow from message generation to reception and link reporting.
