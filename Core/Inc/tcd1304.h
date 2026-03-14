/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tcd1304.h
  * @brief   TCD1304DG CCD sensor driver header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __TCD1304_H__
#define __TCD1304_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "adc.h"
#include "tim.h"

#define TCD1304_TOTAL_PIXELS      3694
#define TCD1304_DUMMY_PIXELS      12
#define TCD1304_SHIELD_PIXELS     16
#define TCD1304_ACTIVE_PIXELS     3648
#define TCD1304_MIN_INTEGRATION_TIME 1000

#define ICG_PIN  GPIO_PIN_10
#define SH_PIN   GPIO_PIN_9
#define ICG_PORT GPIOA
#define SH_PORT  GPIOA

typedef struct {
    uint16_t raw_data[TCD1304_TOTAL_PIXELS];
    uint16_t active_data[TCD1304_ACTIVE_PIXELS];
    uint32_t integration_time;
    uint8_t data_ready;
} TCD1304_Data_t;

void TCD1304_Init(void);
void TCD1304_StartCapture(uint32_t integration_time);
void TCD1304_StopCapture(void);
uint8_t TCD1304_IsDataReady(void);
void TCD1304_GetActiveData(uint16_t *buffer);
void TCD1304_WaitForPhiMHigh(void);
void TCD1304_DelayMicroseconds(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* __TCD1304_H__ */
