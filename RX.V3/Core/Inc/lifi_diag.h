#ifndef LIFI_DIAG_H
#define LIFI_DIAG_H

#include <stdint.h>

typedef struct {
    uint32_t packetsSeen;
    uint32_t packetsGood;
    uint32_t packetsBad;
    uint32_t syncFound;
    uint32_t syncLost;
    uint32_t pllLockEvents;
    uint32_t pllLostEvents;
    uint32_t manchesterErrors;
    uint32_t crcErrors;
    uint32_t timeoutEvents;
    uint32_t noiseSamples;
    uint32_t invalidTransitionCount;
    uint32_t duplicatePackets;
    uint32_t outOfOrderPackets;
    uint32_t lastPacketLength;
    float lastAmplitude;
    float lastNoise;
    float lastEyeMargin;
    float lastPhaseError;
    uint8_t lastLockState;
} LIFI_DIAG;

void LiFi_Diag_Init(LIFI_DIAG* diag);
void LiFi_Diag_UpdateFromSignal(float amplitude, float noise, float eyeMargin);
void LiFi_Diag_UpdateLinkState(uint8_t lockState, float phaseError, uint32_t packetLength, uint32_t crcErrors, uint32_t invalidTransitions);
void LiFi_Diag_UpdateSyncState(uint8_t syncDetected, uint8_t lockState, float phaseError);
void LiFi_Diag_UpdatePacketStats(uint8_t packetValid, uint32_t packetLength, uint8_t lockState, float phaseError, uint32_t crcErrors, uint32_t invalidTransitions);
void LiFi_Diag_UpdateReliability(uint8_t duplicate, uint8_t outOfOrder);
const LIFI_DIAG* LiFi_Diag_Get(void);

#endif
