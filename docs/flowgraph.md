# Flow Graph

## Transmitter Flow

1. Accept text input from UART
2. Fragment and package payload
3. Build frame with sync, length, sequence, payload, CRC
4. Encode symbols according to selected codec mode
5. Emit physical waveform through GPIO / output hardware

## Receiver Flow

1. Acquire ADC samples
2. Track envelope and estimate signal quality
3. Apply slicer and timing recovery
4. Decode symbols into bits
5. Reconstruct frame, validate CRC, and report results
