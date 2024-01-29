/*
 * fans.h
 *
 *  Created on: Jan 22, 2024
 *      Author: Cassius Garcia
 */

#ifndef INC_FANS_H_
#define INC_FANS_H_
#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct
{
	TIM_TypeDef *timer;
	uint64_t max_timer_val;
	volatile uint32_t *CCR;
	float duty_cycle;
} fan_t;

int set_fan_percent(fan_t *fan, float percent);

int fan_init(fan_t *fan, TIM_TypeDef *timer, uint64_t max_timer_val, volatile uint32_t *CCR);

#endif /* INC_FANS_H_ */
