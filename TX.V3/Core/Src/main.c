/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "lifi_config.h"
#include "lifi_msg.h"

TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart3;
PCD_HandleTypeDef hpcd_USB_OTG_FS;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);

/* USER CODE BEGIN 0 */
#define MAX_TX_BITS 512
LIFI_MSG g_txMsg;
uint8_t txBitBuffer[MAX_TX_BITS];
uint32_t txTotalBits = 0;
uint32_t txBitIndex = 0;
uint8_t txHalfBitState = 0;
volatile uint8_t packetLoopCount = 0; 
uint32_t totalPacketsFired = 0;

extern UART_HandleTypeDef huart3; 
uint8_t uartRxByte;
uint8_t pcMessageBuffer[128]; 
uint8_t pcMessageIndex = 0;
uint8_t newPcMessageReady = 0;

void Send_Fragmented_Message(uint8_t* msg, uint8_t total_len) {
    const LIFI_CONFIG* cfg = LiFi_Config_Get();
    char txStatus[128];
    HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n[FIRING CHUNK]: ", 18, 100);
    LiFi_Msg_SendFragmented(&g_txMsg, msg, total_len);
    snprintf(txStatus, sizeof(txStatus), "\r\n[TX STAT] fragments=%lu chunk=%lu gap=%u\r\n> ",
             (unsigned long)g_txMsg.totalPacketsFired,
             (unsigned long)g_txMsg.currentFragmentLength,
             cfg->interPacketGapMs);
    HAL_UART_Transmit(&huart3, (uint8_t*)txStatus, strlen(txStatus), 100);
    HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n[MESSAGE COMPLETE]\r\n> ", 24, 100);

    for (int i = 0; i < 32; i++) txBitBuffer[i] = 0;
    txTotalBits = 32;
    txBitIndex = 0;
    (void)cfg;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) { 
        HAL_UART_Transmit(&huart3, &uartRxByte, 1, 10);
        if (uartRxByte == '\r' || uartRxByte == '\n') {
            if (pcMessageIndex > 0) newPcMessageReady = 1;
        } 
        else {
            if (pcMessageIndex < 127) pcMessageBuffer[pcMessageIndex++] = uartRxByte;
        }
        HAL_UART_Receive_IT(&huart3, &uartRxByte, 1); 
    }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();

  /* USER CODE BEGIN 2 */
  LIFI_CONFIG cfg;
  LiFi_Config_Init(&cfg);
  LiFi_Msg_Init(&g_txMsg);

  for(int i=0; i<MAX_TX_BITS; i++) txBitBuffer[i] = 0;
  txTotalBits = 32;
  
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_UART_Receive_IT(&huart3, &uartRxByte, 1); // Arm the PC listener
  
  HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n[TX BOOT] LiFi modem online. Type a message and press Enter.\r\n> ", 69, 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t lastStatusReportMs = 0;
  while (1)
  {
      if ((HAL_GetTick() - lastStatusReportMs) > 2000) {
          char idleStatus[96];
          snprintf(idleStatus, sizeof(idleStatus), "\r\n[TX STATE] active=%u fragments=%lu lastTx=%lu\r\n> ",
                   g_txMsg.txActive,
                   (unsigned long)g_txMsg.totalPacketsFired,
                   (unsigned long)g_txMsg.lastTransmitMs);
          HAL_UART_Transmit(&huart3, (uint8_t*)idleStatus, strlen(idleStatus), 100);
          lastStatusReportMs = HAL_GetTick();
      }

      // 1. Check for Keyboard Input
      if (newPcMessageReady == 1) {
          
          // Send the message
          Send_Fragmented_Message(pcMessageBuffer, pcMessageIndex);
          
          // Print dynamic TX Telemetry ONLY after the laser finishes firing
          char txDash[128];
          sprintf(txDash, "[TX SUCCESS] Fragments Fired: %lu | Payload Size: %d Bytes\r\n> ", 
                  g_txMsg.totalPacketsFired, pcMessageIndex);
          HAL_UART_Transmit(&huart3, (uint8_t*)txDash, strlen(txDash), 100);
          
          // Reset
          pcMessageIndex = 0;
          newPcMessageReady = 0;
      }

      HAL_Delay(10);
    /* USER CODE END WHILE */
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) { Error_Handler(); }
}

static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 95;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 2499;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK) { Error_Handler(); }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) { Error_Handler(); }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

static void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK) { Error_Handler(); }
}

static void MX_USB_OTG_FS_PCD_Init(void)
{
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) { Error_Handler(); }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        const LIFI_CONFIG* cfg = LiFi_Config_Get();
        uint8_t currentDataBit = txBitBuffer[txBitIndex];
        uint8_t firstHalf = currentDataBit;
        uint8_t secondHalf = currentDataBit;

        if (cfg->codecMode == LIFI_CODEC_MANCHESTER) {
            secondHalf = (currentDataBit == 0) ? 1u : 0u;
        }
        
        if (txHalfBitState == 0) {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, (firstHalf == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            txHalfBitState = 1;
        } else {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, (secondHalf == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
            txHalfBitState = 0;
            
            txBitIndex++;
            if (txBitIndex >= txTotalBits) {
                txBitIndex = 0; 
                g_txMsg.packetLoopCount++;
            }
        }
    }
}
/* USER CODE END 4 */

void Error_Handler(void) { __disable_irq(); while (1) { } }
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif