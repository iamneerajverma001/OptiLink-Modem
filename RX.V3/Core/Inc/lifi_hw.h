#ifndef LIFI_HW_H
#define LIFI_HW_H

#include "main.h"
#include <stdint.h>

/* The Circular DMA Buffer */
#define ADC_BUF_LEN 256
extern uint16_t adc_buffer[ADC_BUF_LEN];

/* Public Functions */
void LiFi_HW_Init(void);

#endif /* LIFI_HW_H */