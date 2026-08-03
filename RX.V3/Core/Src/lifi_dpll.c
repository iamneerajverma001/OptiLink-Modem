#include "lifi_dpll.h"
#include "lifi_mac.h"
#include "lifi_config.h"
#include "lifi_diag.h"
#include <math.h>

extern volatile uint8_t digitalState; 


volatile float pllPhaseError = 0.0f;
volatile uint32_t pllLockCounter = 0;
volatile uint8_t pllLocked = 0;
volatile uint32_t manchesterBitsRecovered = 0;

float currentPhase = 0.0f;
float phaseStep = 1.0f; 
uint32_t lastEdgeTick = 0;
uint8_t eyeSampled = 0; 

void LiFi_DPLL_Init(void) {
    LIFI_CONFIG cfg;
    LiFi_Config_Init(&cfg);

    currentPhase = 0.0f; 
    pllLockCounter = 0; 
    pllLocked = 0; 
    eyeSampled = 0;
    (void)cfg;
}

void LiFi_DPLL_OnEdge(uint32_t currentSampleTick, uint8_t edgeType) {
    if (edgeType == 0) return; 

    const LIFI_CONFIG* cfg = LiFi_Config_Get();
    uint32_t deltaSamples = currentSampleTick - lastEdgeTick;
    lastEdgeTick = currentSampleTick;

    if (deltaSamples > (cfg->samplesPerBit * 4u)) {
        pllLockCounter = 0; pllLocked = 0; currentPhase = 0.0f; eyeSampled = 0; return;
    }

    float nominalHalfBit = (float)cfg->samplesPerHalfBit;
    float currentPos = fmodf(currentPhase, nominalHalfBit);
    
    if (currentPos > (nominalHalfBit / 2.0f)) {
        pllPhaseError = nominalHalfBit - currentPos; 
    } else {
        pllPhaseError = -currentPos; 
    }

    float adaptiveGain = pllLocked ? cfg->trackGain : cfg->acquireGain;
    if (cfg->adaptiveThresholdEnabled > 0.0f) {
        adaptiveGain *= (pllLocked ? 1.15f : 0.90f);
    }
    currentPhase += (pllPhaseError * adaptiveGain);

    if (fabsf(pllPhaseError) < cfg->lockThreshold) {
        if (pllLockCounter < 100) pllLockCounter++;
        if (pllLockCounter > 10) pllLocked = 1;
    } else if (fabsf(pllPhaseError) > cfg->unlockThreshold) {
        if (pllLockCounter > 0) pllLockCounter--;
        if (pllLockCounter == 0) pllLocked = 0;
    }

    LiFi_Diag_UpdateLinkState(pllLocked ? 1u : 0u, pllPhaseError, 0, 0, 0);
}

void LiFi_DPLL_Tick(void) {
    const LIFI_CONFIG* cfg = LiFi_Config_Get();
    currentPhase += phaseStep;

    if (currentPhase >= cfg->eyeOffset && eyeSampled == 0) {
        if (pllLocked) {
            LiFi_MAC_ProcessHalfBit(digitalState);
        }
        eyeSampled = 1;
    }

    if (currentPhase >= cfg->samplesPerHalfBit) {
        currentPhase -= cfg->samplesPerHalfBit;
        eyeSampled = 0;

        if (pllLocked) {
            manchesterBitsRecovered++;
        } else {
            macSyncFound = 0;
        }
    }
}