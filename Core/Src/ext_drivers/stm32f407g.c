/*
 * stm32f407g.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "ext_drivers/stm32f407g.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

extern CAN_HandleTypeDef hcan1;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

extern UART_HandleTypeDef huart2;

void stm32f407g_init(stm32f407g_t * dev)
{
	dev->hadc1 = hadc1;
	dev->hadc2 = hadc2;

	dev->hcan1 = hcan1;

	dev->hspi1 = hspi1;
	dev->hspi3 = hspi3;

	dev->htim1 = htim1;
	dev->htim3 = htim3;
	dev->htim4 = htim4;
	dev->htim5 = htim5;

	dev->huart2 = huart2;
}

uint16_t stm32f407g_adc_read(ADC_HandleTypeDef *hadc)
{
	uint16_t count;

	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
	count = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);
	return count;
}

HAL_StatusTypeDef stm32f407g_adc_switch_channel(ADC_HandleTypeDef *hadc, uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	return HAL_ADC_ConfigChannel(hadc, &sConfig);
}

