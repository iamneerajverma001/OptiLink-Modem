#include "lifi_mac.h"
#include "lifi_config.h"
#include "lifi_diag.h"
#include "main.h"

#define RAW_SYNC_NORMAL   0xA5996A66
#define RAW_SYNC_INVERTED 0x5A669599

volatile uint8_t macSyncFound = 0;
volatile uint32_t macRawShiftRegister = 0;
volatile uint8_t macState = 0; 
volatile uint8_t macExpectedLength = 0;
volatile uint8_t macPayloadBuffer[32];
volatile uint8_t macPayloadReady = 0;
volatile uint16_t macReceivedCRC = 0;
volatile uint16_t macCalculatedCRC = 0;
volatile uint8_t macCRCPassed = 0;
volatile uint8_t macPacketSequence = 0;
volatile uint8_t macLastSequence = 0;
volatile uint8_t macExpectedSequence = 1;

uint8_t bitCounter = 0;
uint8_t byteBuffer = 0;
uint8_t bytesReceived = 0;
uint8_t crcBytesReceived = 0;
uint8_t lastHalfBit = 0;
uint8_t halfBitToggle = 0;
uint8_t invalidTransitionCount = 0;
volatile uint32_t macLastByteTick = 0;

void LiFi_MAC_Init(void) {
    LIFI_CONFIG cfg;
    LiFi_Config_Init(&cfg);
    macState = 0; macSyncFound = 0; macPayloadReady = 0; macRawShiftRegister = 0;
    macCRCPassed = 0;
    bitCounter = 0; byteBuffer = 0; bytesReceived = 0; crcBytesReceived = 0;
    lastHalfBit = 0; halfBitToggle = 0; invalidTransitionCount = 0;
    macPacketSequence = 0; macLastSequence = 0; macExpectedSequence = 1; macLastByteTick = 0;
    (void)cfg;
}

uint16_t Calculate_CRC16(uint8_t length, volatile uint8_t *payload) {
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

void LiFi_MAC_ProcessHalfBit(uint8_t currentDigitalState) {
    const LIFI_CONFIG* cfg = LiFi_Config_Get();

    if (macState != 0 && (HAL_GetTick() - macLastByteTick) > cfg->packetTimeoutMs) {
        macState = 0; macSyncFound = 0; macPayloadReady = 0; bitCounter = 0; halfBitToggle = 0; invalidTransitionCount = 0;
        LiFi_Diag_UpdateSyncState(0, 0, 0.0f);
    }

    if (macState == 0) {
        macRawShiftRegister = (macRawShiftRegister << 1) | (currentDigitalState & 0x01);
        if (macRawShiftRegister == cfg->syncWord || macRawShiftRegister == (uint32_t)(~cfg->syncWord)) {
            macSyncFound = 1; macState = 1;
            bitCounter = 0; halfBitToggle = 0; byteBuffer = 0;
            macCRCPassed = 0; macPayloadReady = 0; invalidTransitionCount = 0;
            macLastByteTick = HAL_GetTick();
            LiFi_Diag_UpdateSyncState(1, 1, 0.0f);
        }
    }
    else {
        if (halfBitToggle == 0) {
            lastHalfBit = currentDigitalState; halfBitToggle = 1; return;
        }

        uint8_t decodedBit = 0;
        if (cfg->codecMode == LIFI_CODEC_NRZ) {
            decodedBit = lastHalfBit;
        } else {
            if (lastHalfBit == 1 && currentDigitalState == 0) decodedBit = 1;
            else if (lastHalfBit == 0 && currentDigitalState == 1) decodedBit = 0;
            else {
                invalidTransitionCount++;
                if (invalidTransitionCount >= 3) {
                    macState = 0; macSyncFound = 0; bitCounter = 0; halfBitToggle = 0; invalidTransitionCount = 0;
                    LiFi_Diag_UpdateSyncState(0, 0, 0.0f);
                }
                return;
            }
        }
        halfBitToggle = 0; invalidTransitionCount = 0;

        byteBuffer = (byteBuffer << 1) | decodedBit;
        bitCounter++;

        if (bitCounter == 8) {
            macLastByteTick = HAL_GetTick();
            if (macState == 1) {
                macExpectedLength = byteBuffer;
                if (macExpectedLength == 0 || macExpectedLength > cfg->maxPayload) {
                    macState = 0; macSyncFound = 0;
                    LiFi_Diag_UpdateSyncState(0, 0, 0.0f);
                    return;
                }
                bytesReceived = 0; macState = 2;
            }
            else if (macState == 2) {
                macPacketSequence = byteBuffer;
                if (macPacketSequence == macLastSequence) {
                    LiFi_Diag_UpdateReliability(1, 0);
                    macState = 0; macSyncFound = 0; macPayloadReady = 0;
                    LiFi_Diag_UpdateSyncState(0, 0, 0.0f);
                    return;
                }
                if (macPacketSequence != macExpectedSequence && macLastSequence != 0u) {
                    LiFi_Diag_UpdateReliability(0, 1);
                    macState = 0; macSyncFound = 0; macPayloadReady = 0;
                    LiFi_Diag_UpdateSyncState(0, 0, 0.0f);
                    return;
                }
                macLastSequence = macPacketSequence;
                macExpectedSequence = (uint8_t)(macPacketSequence + 1u);
                bytesReceived = 0; macState = 3;
            }
            else if (macState == 3) {
                macPayloadBuffer[bytesReceived] = byteBuffer;
                bytesReceived++;
                if (bytesReceived == macExpectedLength) {
                    macPayloadBuffer[bytesReceived] = '\0';
                    macReceivedCRC = 0; crcBytesReceived = 0;
                    macState = 4;
                }
            }
            else if (macState == 4) {
                macReceivedCRC = (macReceivedCRC << 8) | byteBuffer;
                crcBytesReceived++;
                if (crcBytesReceived == 2) {
                    macCalculatedCRC = Calculate_CRC16(macExpectedLength, macPayloadBuffer);
                    if (macCalculatedCRC == macReceivedCRC) {
                        macCRCPassed = 1;
                        macPayloadReady = 1;
                        LiFi_Diag_UpdatePacketStats(1, macExpectedLength, 1, 0.0f, 0, invalidTransitionCount);
                    } else {
                        LiFi_Diag_UpdatePacketStats(0, macExpectedLength, 0, 0.0f, 1, invalidTransitionCount);
                    }
                    macState = 0; macSyncFound = 0;
                }
            }
            bitCounter = 0;
        }
    }
}