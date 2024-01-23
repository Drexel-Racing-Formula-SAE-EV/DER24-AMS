/*
 * fans.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Cassius Garcia
 */

int fan_init(fan_t *fan, TIM_TypeDef *timer, uint64_t max_timer_val, volatile uint32_t *CCR){
	fan->timer = timer;
	fan->max_timer_val = max_timer_val;
	fan->CCR = CCR;
}

int set_fan_percent(fan_t *fan, float percent){
	if(percent > 100.0 || percent < 0.0) return 1;
	fan->duty_cycle = percent;
	fan->CCR = (float)fan->max_timer_val * percent / 100.0;
	return 0;
}


