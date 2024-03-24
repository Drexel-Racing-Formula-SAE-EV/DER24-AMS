/*
 * stm32f407g.h
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#ifndef INC_EXT_DRIVERS_STM32F407G_H_
#define INC_EXT_DRIVERS_STM32F407G_H_

#include <stm32f4xx_hal.h>
#include "cmsis_os.h"

typedef struct
{
	ADC_HandleTypeDef hadc1;
	ADC_HandleTypeDef hadc2;

	CAN_HandleTypeDef hcan1;

	SPI_HandleTypeDef hspi1;
	SPI_HandleTypeDef hspi3;

	TIM_HandleTypeDef htim1;
	TIM_HandleTypeDef htim3;
	TIM_HandleTypeDef htim4;
	TIM_HandleTypeDef htim5;

	UART_HandleTypeDef huart2;
} stm32f407g_t;

void stm32f407g_init(stm32f407g_t * dev);
uint16_t stm32f767_adc_read(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef stm32f767_adc_switch_channel(ADC_HandleTypeDef *hadc, uint32_t channel);

#endif /* INC_EXT_DRIVERS_STM32F407G_H_ */
