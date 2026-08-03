#ifndef LIFI_DPLL_H
#define LIFI_DPLL_H

#include <stdint.h>

extern volatile float pllPhaseError;
extern volatile uint32_t pllLockCounter;
extern volatile uint8_t pllLocked;
extern volatile uint32_t manchesterBitsRecovered;

void LiFi_DPLL_Init(void);
void LiFi_DPLL_OnEdge(uint32_t currentSampleTick, uint8_t edgeType);
void LiFi_DPLL_Tick(void);

#endif