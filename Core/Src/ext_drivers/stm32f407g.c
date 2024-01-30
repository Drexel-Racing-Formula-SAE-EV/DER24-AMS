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
	dev->hadc1 = hadc2;
	dev->hcan1 = hcan1;

	dev->hspi1 = hspi1;
	dev->hspi3 = hspi3;

	dev->htim1 = htim1;
	dev->htim3 = htim3;
	dev->htim4 = htim3;
	dev->htim5 = htim5;

	dev->huart2 = huart2;
}
