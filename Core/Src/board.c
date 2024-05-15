/*
 * board.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "board.h"
#include "main.h"

#define FAN_MAX 3360
#define CUR_SEN_CH_L 14
#define CUR_SEN_CH_H 1

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

	canbus_device_init(&board->canbus, &board->stm32f407g.hcan1);

	cli_device_init(&board->cli, &board->stm32f407g.huart2);
	current_sensor_init(&board->current_sensor,
						&board->stm32f407g.hadc2,
						&board->stm32f407g.hadc1,
						CUR_SEN_CH_L,
						CUR_SEN_CH_H
					   );

	imd_init(&board->imd, 84000000, &board->stm32f407g.htim5, TIM5, TIM_CHANNEL_2, TIM_CHANNEL_1, IMD_STATUS_MCU_GPIO_Port, IMD_STATUS_MCU_Pin);

	return;
}
