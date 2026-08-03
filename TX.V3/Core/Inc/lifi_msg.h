#ifndef LIFI_MSG_H
#define LIFI_MSG_H

#include <stdint.h>

typedef struct {
    uint8_t txBitBuffer[512];
    uint32_t txTotalBits;
    uint32_t txBitIndex;
    uint8_t txHalfBitState;
    volatile uint8_t packetLoopCount;
    uint32_t totalPacketsFired;
    uint32_t currentFragmentLength;
    uint32_t currentFragmentIndex;
    uint32_t lastTransmitMs;
    uint32_t transmitErrors;
    uint8_t txActive;
    uint32_t lastStatusMs;
    uint8_t txSequence;
} LIFI_MSG;

void LiFi_Msg_Init(LIFI_MSG* msg);
void LiFi_Msg_BuildPacket(LIFI_MSG* msg, uint8_t* payload, uint8_t length);
void LiFi_Msg_SendFragmented(LIFI_MSG* msg, uint8_t* data, uint8_t length);

#endif
