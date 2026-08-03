#ifndef LIFI_MAC_H
#define LIFI_MAC_H

#include <stdint.h>

extern volatile uint8_t macSyncFound;
extern volatile uint32_t macRawShiftRegister;
extern volatile uint8_t macState; 
extern volatile uint8_t macExpectedLength;
extern volatile uint8_t macPayloadBuffer[32];
extern volatile uint8_t macPayloadReady;
extern volatile uint16_t macReceivedCRC;
extern volatile uint16_t macCalculatedCRC;
extern volatile uint8_t macCRCPassed;
extern volatile uint8_t macPacketSequence;
extern volatile uint8_t macLastSequence;

void LiFi_MAC_Init(void);
void LiFi_MAC_ProcessHalfBit(uint8_t currentDigitalState);

#endif