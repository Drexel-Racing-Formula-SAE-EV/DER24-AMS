/*
 * board.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "board.h"

void board_init(board_t *board)
{
	stm32f407g_init(&board->stm32f407g);

	fan_init(&board->fans[0], TIM1, 65535, &TIM1->CCR3);
	fan_init(&board->fans[1], TIM1, 65535, &TIM1->CCR4);
	fan_init(&board->fans[2], TIM1, 65535, &TIM1->CCR2);
	fan_init(&board->fans[3], TIM1, 65535, &TIM1->CCR1);
	fan_init(&board->fans[4], TIM3, 65535, &TIM3->CCR4);
	fan_init(&board->fans[5], TIM3, 65535, &TIM3->CCR3);
	fan_init(&board->fans[6], TIM3, 65535, &TIM3->CCR2);
	fan_init(&board->fans[7], TIM3, 65535, &TIM3->CCR1);
	fan_init(&board->fans[8], TIM4, 65535, &TIM4->CCR3);
	fan_init(&board->fans[9], TIM4, 65535, &TIM4->CCR4);

	return;
}
