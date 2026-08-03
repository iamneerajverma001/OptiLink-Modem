#include "lifi_msg.h"
#include "lifi_config.h"
#include "main.h"
#include <string.h>

static uint16_t Calculate_CRC16(uint8_t length, uint8_t* payload) {
    uint8_t crcBuffer[33];
    crcBuffer[0] = length;
    for (uint8_t i = 0; i < length; i++) crcBuffer[1 + i] = payload[i];

    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length + 1; i++) {
        crc ^= (uint16_t)crcBuffer[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else crc <<= 1;
        }
    }
    return crc;
}

void LiFi_Msg_Init(LIFI_MSG* msg) {
    if (msg == 0) {
        return;
    }

    memset(msg, 0, sizeof(*msg));
}

void LiFi_Msg_BuildPacket(LIFI_MSG* msg, uint8_t* payload, uint8_t length) {
    if (msg == 0 || payload == 0) {
        return;
    }

    const LIFI_CONFIG* cfg = LiFi_Config_Get();
    uint32_t idx = 0;

    msg->txSequence = (uint8_t)((msg->txSequence + 1u) & 0xFFu);

    for (uint16_t i = 0; i < cfg->preambleBits; i++) msg->txBitBuffer[idx++] = 0;

    for (int i = 0; i < 32; i++) msg->txBitBuffer[idx++] = 0;

    uint16_t sync = (uint16_t)cfg->syncWord;
    for (int i = 15; i >= 0; i--) msg->txBitBuffer[idx++] = (sync >> i) & 1;
    for (int i = 7; i >= 0; i--) msg->txBitBuffer[idx++] = (length >> i) & 1;
    for (int i = 7; i >= 0; i--) msg->txBitBuffer[idx++] = (msg->txSequence >> i) & 1;

    for (int i = 0; i < length; i++) {
        for (int b = 7; b >= 0; b--) msg->txBitBuffer[idx++] = (payload[i] >> b) & 1;
    }

    uint16_t crc = Calculate_CRC16(length, payload);
    for (int i = 15; i >= 0; i--) msg->txBitBuffer[idx++] = (crc >> i) & 1;

    msg->txTotalBits = idx;
    msg->txBitIndex = 0;
}

void LiFi_Msg_SendFragmented(LIFI_MSG* msg, uint8_t* data, uint8_t length) {
    if (msg == 0 || data == 0) {
        return;
    }

    const LIFI_CONFIG* cfg = LiFi_Config_Get();
    uint8_t currentIndex = 0;

    while (currentIndex < length) {
        msg->txActive = 1;
        msg->currentFragmentIndex = currentIndex;
        uint8_t chunkLen = length - currentIndex;
        if (chunkLen > cfg->maxChunk) {
            chunkLen = cfg->maxChunk;
            for (int i = chunkLen; i > 0; i--) {
                if (data[currentIndex + i - 1] == ' ') {
                    chunkLen = i;
                    break;
                }
            }
        }

        msg->currentFragmentLength = chunkLen;
        LiFi_Msg_BuildPacket(msg, &data[currentIndex], chunkLen);
        msg->packetLoopCount = 0;
        while (msg->packetLoopCount < 1) {
            HAL_Delay(cfg->packetGapMs);
        }
        msg->totalPacketsFired++;
        msg->lastTransmitMs = HAL_GetTick();
        currentIndex += chunkLen;

        for (int i = 0; i < 32; i++) msg->txBitBuffer[i] = 0;
        msg->txTotalBits = 32;
        msg->txBitIndex = 0;
        HAL_Delay(cfg->interPacketGapMs);
    }

    msg->txActive = 0;
    msg->lastStatusMs = HAL_GetTick();
}
