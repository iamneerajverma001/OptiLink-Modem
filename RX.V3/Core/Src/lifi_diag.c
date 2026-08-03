#include "lifi_diag.h"
#include <string.h>

static LIFI_DIAG g_diag;

void LiFi_Diag_Init(LIFI_DIAG* diag) {
    memset(&g_diag, 0, sizeof(g_diag));
    if (diag != 0) {
        *diag = g_diag;
    }
}

void LiFi_Diag_UpdateFromSignal(float amplitude, float noise, float eyeMargin) {
    g_diag.noiseSamples++;
    g_diag.lastAmplitude = amplitude;
    g_diag.lastNoise = noise;
    g_diag.lastEyeMargin = eyeMargin;
}

void LiFi_Diag_UpdateLinkState(uint8_t lockState, float phaseError, uint32_t packetLength, uint32_t crcErrors, uint32_t invalidTransitions) {
    uint8_t previousLockState = g_diag.lastLockState;
    g_diag.lastLockState = lockState;
    g_diag.lastPhaseError = phaseError;
    g_diag.lastPacketLength = packetLength;
    g_diag.crcErrors += crcErrors;
    g_diag.invalidTransitionCount += invalidTransitions;

    if (previousLockState == 0u && lockState != 0u) {
        g_diag.pllLockEvents++;
    } else if (previousLockState != 0u && lockState == 0u) {
        g_diag.pllLostEvents++;
    }
}

void LiFi_Diag_UpdateSyncState(uint8_t syncDetected, uint8_t lockState, float phaseError) {
    if (syncDetected) {
        g_diag.syncFound++;
    } else {
        g_diag.syncLost++;
    }

    g_diag.lastLockState = lockState;
    g_diag.lastPhaseError = phaseError;
}

void LiFi_Diag_UpdatePacketStats(uint8_t packetValid, uint32_t packetLength, uint8_t lockState, float phaseError, uint32_t crcErrors, uint32_t invalidTransitions) {
    g_diag.packetsSeen++;
    g_diag.lastPacketLength = packetLength;
    g_diag.lastLockState = lockState;
    g_diag.lastPhaseError = phaseError;
    g_diag.crcErrors += crcErrors;
    g_diag.invalidTransitionCount += invalidTransitions;

    if (packetValid) {
        g_diag.packetsGood++;
    } else {
        g_diag.packetsBad++;
    }
}

void LiFi_Diag_UpdateReliability(uint8_t duplicate, uint8_t outOfOrder) {
    if (duplicate) {
        g_diag.duplicatePackets++;
    }
    if (outOfOrder) {
        g_diag.outOfOrderPackets++;
    }
}

const LIFI_DIAG* LiFi_Diag_Get(void) {
    return &g_diag;
}
