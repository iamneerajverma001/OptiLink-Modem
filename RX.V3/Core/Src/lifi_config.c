#include "lifi_config.h"

static LIFI_CONFIG g_cfg;

void LiFi_Config_Init(LIFI_CONFIG* cfg) {
    if (cfg == 0) {
        return;
    }

    g_cfg.bitrate = 400;
    g_cfg.adcRate = 12800;
    g_cfg.samplesPerBit = 32;
    g_cfg.samplesPerHalfBit = 16;
    g_cfg.preambleBits = 32;
    g_cfg.syncWord = 0xA5996A66u;
    g_cfg.maxPayload = 31;
    g_cfg.crcPolynomial = 0x1021;

    g_cfg.dcFilterAlpha = 0.02f;
    g_cfg.envelopeRise = 0.85f;
    g_cfg.envelopeFall = 0.10f;
    g_cfg.noiseWindow = 8.0f;
    g_cfg.hysteresisMargin = 0.20f;
    g_cfg.adaptiveThresholdEnabled = 1.0f;

    g_cfg.acquireGain = 0.25f;
    g_cfg.trackGain = 0.05f;
    g_cfg.lockThreshold = 4.0f;
    g_cfg.unlockThreshold = 8.0f;
    g_cfg.eyeOffset = 14.0f;

    g_cfg.packetTimeoutMs = 2500;
    g_cfg.fragmentLength = 12;
    g_cfg.interPacketGapMs = 60;
    g_cfg.codecMode = LIFI_CODEC_MANCHESTER;
    g_cfg.debugEnabled = 1;

    *cfg = g_cfg;
}

const LIFI_CONFIG* LiFi_Config_Get(void) {
    return &g_cfg;
}
