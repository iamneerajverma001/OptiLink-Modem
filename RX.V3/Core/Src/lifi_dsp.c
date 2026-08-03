#include "lifi_dsp.h"
#include "lifi_dpll.h"
#include "lifi_config.h"
#include "lifi_diag.h"

volatile float dcLevel = 2048.0f;
volatile float signalAmplitude = 0.0f;
volatile uint8_t digitalState = 0;
volatile uint32_t edgeCount = 0;

float envMax = 2048.0f;
float envMin = 2048.0f;

void LiFi_DSP_Init(void) {
    LIFI_CONFIG cfg;
    LiFi_Config_Init(&cfg);

    dcLevel = 2048.0f;
    signalAmplitude = 0.0f;
    digitalState = 0;
    edgeCount = 0;
    envMax = 2048.0f;
    envMin = 2048.0f;

    (void)cfg;
}

void LiFi_DSP_ProcessBlock(uint16_t* adc_buffer, uint16_t length, uint32_t currentTick) {
    for (uint16_t i = 0; i < length; i++) {
        float sample = (float)adc_buffer[i];
        
        const LIFI_CONFIG* cfg = LiFi_Config_Get();

        // 1. Envelope Tracking
        if (sample > envMax) envMax = sample;
        if (sample < envMin) envMin = sample;
        
        envMax -= cfg->envelopeRise;
        envMin += cfg->envelopeFall;
        
        if (envMax < envMin) envMax = envMin + 1.0f;

        // 2. Metrics
        dcLevel = (envMax + envMin) / 2.0f;
        signalAmplitude = envMax - envMin;

        // 3. The Adaptive Slicer
        float adaptiveNoise = cfg->noiseWindow;
        if (cfg->adaptiveThresholdEnabled > 0.0f && signalAmplitude > 0.0f) {
            adaptiveNoise = cfg->noiseWindow + (signalAmplitude * 0.02f);
        }
        float noiseMargin = adaptiveNoise;
        float high_thresh = envMax - (signalAmplitude * cfg->hysteresisMargin) - noiseMargin;
        float low_thresh  = envMax - (signalAmplitude * (cfg->hysteresisMargin + 0.10f)) - noiseMargin;

        uint8_t previousState = digitalState;

        // 4. Hysteresis
        if (digitalState == 0 && sample > high_thresh) {
            digitalState = 1;
        } else if (digitalState == 1 && sample < low_thresh) {
            digitalState = 0;
        }

        // 5. Edge Detection -> Feed the Clock Recovery
        if (digitalState != previousState) {
            edgeCount++;
            LiFi_DPLL_OnEdge(currentTick + i, digitalState);
        }

        LiFi_Diag_UpdateFromSignal(signalAmplitude, noiseMargin, (high_thresh - low_thresh));

        // 6. Clock the DPLL once per ADC sample
        LiFi_DPLL_Tick();
    }
}