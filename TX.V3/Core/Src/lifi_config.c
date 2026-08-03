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
    g_cfg.maxChunk = 12;
    g_cfg.interPacketGapMs = 60;
    g_cfg.packetGapMs = 5;
    g_cfg.codecMode = LIFI_CODEC_MANCHESTER;
    g_cfg.debugEnabled = 1;

    *cfg = g_cfg;
}

const LIFI_CONFIG* LiFi_Config_Get(void) {
    return &g_cfg;
}
