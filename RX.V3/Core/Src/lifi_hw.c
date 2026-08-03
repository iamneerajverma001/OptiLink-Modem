#include "lifi_hw.h"
#include "lifi_dsp.h"

/* EXTERN REFERENCES TO HARDWARE HANDLES IN MAIN.C */
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

// Define the size of the circular buffer
#define ADC_BUF_LEN 256

// Allocate the buffer in memory
uint16_t adc_buffer[ADC_BUF_LEN];

// Telemetry counters for the debugger
volatile uint32_t dma_half_count = 0;
volatile uint32_t dma_full_count = 0;

// Keep track of the absolute sample time for the DPLL
volatile uint32_t totalSamplesProcessed = 0;

void LiFi_HW_Init(void) {
    dma_half_count = 0;
    dma_full_count = 0;
    totalSamplesProcessed = 0;
    
    // Start the ADC in DMA mode, filling our circular buffer
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_LEN);
    
    // Start the hardware timer that triggers the ADC (12.8kHz)
    HAL_TIM_Base_Start(&htim3);
}

// Interrupt fired when the first half of the buffer is full (samples 0 to 127)
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc->Instance == ADC1) {
        dma_half_count++;
        // Send the first 128 samples to the DSP engine
        LiFi_DSP_ProcessBlock(&adc_buffer[0], ADC_BUF_LEN / 2, totalSamplesProcessed);
        totalSamplesProcessed += (ADC_BUF_LEN / 2);
    }
}

// Interrupt fired when the second half of the buffer is full (samples 128 to 255)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc->Instance == ADC1) {
        dma_full_count++;
        // Send the second 128 samples to the DSP engine
        LiFi_DSP_ProcessBlock(&adc_buffer[ADC_BUF_LEN / 2], ADC_BUF_LEN / 2, totalSamplesProcessed);
        totalSamplesProcessed += (ADC_BUF_LEN / 2);
    }
}