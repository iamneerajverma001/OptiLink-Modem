#ifndef LIFI_CONFIG_H
#define LIFI_CONFIG_H

#include <stdint.h>

typedef enum {
    LIFI_CODEC_MANCHESTER = 0,
    LIFI_CODEC_NRZ = 1
} LIFI_CODEC_MODE;

typedef struct {
    uint32_t bitrate;
    uint32_t adcRate;
    uint16_t samplesPerBit;
    uint16_t samplesPerHalfBit;
    uint16_t preambleBits;
    uint32_t syncWord;
    uint8_t maxPayload;
    uint16_t crcPolynomial;
    uint8_t maxChunk;
    uint16_t interPacketGapMs;
    uint16_t packetGapMs;
    uint8_t codecMode;
    uint8_t debugEnabled;
} LIFI_CONFIG;

void LiFi_Config_Init(LIFI_CONFIG* cfg);
const LIFI_CONFIG* LiFi_Config_Get(void);

#endif
