/*
 * board.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "board.h"
#include "stm32f407g.h"

void board_init(board_t *board)
{
	// todo init stm

	// todo: add values
	fan_init(&board->fans[0], NULL, 65535, NULL);
	fan_init(&board->fans[1], NULL, 65535, NULL);
	fan_init(&board->fans[2], NULL, 65535, NULL);
	fan_init(&board->fans[3], NULL, 65535, NULL);
	fan_init(&board->fans[4], NULL, 65535, NULL);
	fan_init(&board->fans[5], NULL, 65535, NULL);
	fan_init(&board->fans[6], NULL, 65535, NULL);
	fan_init(&board->fans[7], NULL, 65535, NULL);
	fan_init(&board->fans[8], NULL, 65535, NULL);
	fan_init(&board->fans[9], NULL, 65535, NULL);

	return;
}
