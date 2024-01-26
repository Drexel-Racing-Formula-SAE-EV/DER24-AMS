/*
 * fans.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Cassius Garcia
 */
#include "fans.h"

int fan_init(fan_t *fan, TIM_TypeDef *timer, uint64_t max_timer_val, volatile uint32_t *CCR){
	fan->timer = timer;
	fan->max_timer_val = max_timer_val;
	fan->CCR = CCR;
	return 0;
}

int set_fan_percent(fan_t *fan, float percent){
	if(percent > 100.0 || percent < 0.0) return 1;
	fan->duty_cycle = percent;
	*fan->CCR = (volatile uint32_t)fan->max_timer_val * (volatile uint32_t)percent / 100;
	return 0;
}


