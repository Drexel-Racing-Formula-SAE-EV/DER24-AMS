/*
 * board.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "board.h"

#define FAN_MAX 3360

void board_init(board_t *board)
{
	stm32f407g_init(&board->stm32f407g);

	fan_init(&board->fans[0], TIM1, &board->stm32f407g.htim1, FAN_MAX, &TIM1->CCR3, 3);
	fan_init(&board->fans[1], TIM1, &board->stm32f407g.htim1, FAN_MAX, &TIM1->CCR4, 4);
	fan_init(&board->fans[2], TIM1, &board->stm32f407g.htim1, FAN_MAX, &TIM1->CCR2, 2);
	fan_init(&board->fans[3], TIM1, &board->stm32f407g.htim1, FAN_MAX, &TIM1->CCR1, 1);
	fan_init(&board->fans[4], TIM3, &board->stm32f407g.htim3, FAN_MAX, &TIM3->CCR4, 4);
	fan_init(&board->fans[5], TIM3, &board->stm32f407g.htim3, FAN_MAX, &TIM3->CCR3, 3);
	fan_init(&board->fans[6], TIM3, &board->stm32f407g.htim3, FAN_MAX, &TIM3->CCR2, 2);
	fan_init(&board->fans[7], TIM3, &board->stm32f407g.htim3, FAN_MAX, &TIM3->CCR1, 1);
	fan_init(&board->fans[8], TIM4, &board->stm32f407g.htim4, FAN_MAX, &TIM4->CCR3, 3);
	fan_init(&board->fans[9], TIM4, &board->stm32f407g.htim4, FAN_MAX, &TIM4->CCR4, 4);

	return;
}
