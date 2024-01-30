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
	int a;
} stm32f407g_t;

void stm32f407g_init(stm32f407g_t * dev);

#endif /* INC_EXT_DRIVERS_STM32F407G_H_ */
