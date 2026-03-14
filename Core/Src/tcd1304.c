/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tcd1304.c
  * @brief   TCD1304DG CCD sensor driver implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "tcd1304.h"
#include "dma.h"

static TCD1304_Data_t tcd1304_data;
static volatile uint8_t dma_complete = 0;

void TCD1304_Init(void)
{
    HAL_GPIO_WritePin(ICG_PORT, ICG_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SH_PORT, SH_PIN, GPIO_PIN_SET);
    
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    
    tcd1304_data.data_ready = 0;
    tcd1304_data.integration_time = TCD1304_MIN_INTEGRATION_TIME;
}

void TCD1304_WaitForPhiMHigh(void)
{
    while (TIM1->CNT > 41)
    {
    }
}

void TCD1304_DelayMicroseconds(uint32_t us)
{
    uint32_t start = TIM1->CNT;
    uint32_t elapsed = 0;
    uint32_t target = us * 2;
    
    while (elapsed < target)
    {
        uint32_t current = TIM1->CNT;
        if (current >= start)
        {
            elapsed = current - start;
        }
        else
        {
            elapsed = (65535 - start) + current;
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        dma_complete = 1;
        
        for (int i = 0; i < TCD1304_ACTIVE_PIXELS; i++)
        {
            tcd1304_data.active_data[i] = tcd1304_data.raw_data[TCD1304_DUMMY_PIXELS + TCD1304_SHIELD_PIXELS + i];
        }
        
        tcd1304_data.data_ready = 1;
    }
}

void TCD1304_StartCapture(uint32_t integration_time)
{
    if (integration_time < TCD1304_MIN_INTEGRATION_TIME)
    {
        integration_time = TCD1304_MIN_INTEGRATION_TIME;
    }
    
    tcd1304_data.integration_time = integration_time;
    tcd1304_data.data_ready = 0;
    dma_complete = 0;
    
    TCD1304_WaitForPhiMHigh();
    
    HAL_GPIO_WritePin(ICG_PORT, ICG_PIN, GPIO_PIN_RESET);
    
    TCD1304_DelayMicroseconds(1000);
    
    TCD1304_WaitForPhiMHigh();
    
    HAL_GPIO_WritePin(SH_PORT, SH_PIN, GPIO_PIN_SET);
    
    TCD1304_DelayMicroseconds(1000);
    
    TCD1304_WaitForPhiMHigh();
    
    HAL_GPIO_WritePin(SH_PORT, SH_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ICG_PORT, ICG_PIN, GPIO_PIN_SET);
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)tcd1304_data.raw_data, TCD1304_TOTAL_PIXELS);
    
    uint32_t capture_time = 2000;
    if (integration_time > capture_time)
    {
        TCD1304_DelayMicroseconds(integration_time - capture_time);
    }
}

void TCD1304_StopCapture(void)
{
    HAL_ADC_Stop_DMA(&hadc1);
    tcd1304_data.data_ready = 0;
}

uint8_t TCD1304_IsDataReady(void)
{
    return tcd1304_data.data_ready;
}

void TCD1304_GetActiveData(uint16_t *buffer)
{
    if (buffer != NULL && tcd1304_data.data_ready)
    {
        for (int i = 0; i < TCD1304_ACTIVE_PIXELS; i++)
        {
            buffer[i] = tcd1304_data.active_data[i];
        }
    }
}
