#ifndef LIFI_DSP_H
#define LIFI_DSP_H

#include <stdint.h>

extern volatile float dcLevel;
extern volatile float signalAmplitude;
extern volatile uint8_t digitalState;
extern volatile uint32_t edgeCount;

void LiFi_DSP_Init(void);
void LiFi_DSP_ProcessBlock(uint16_t* adc_buffer, uint16_t length, uint32_t currentTick);

#endif