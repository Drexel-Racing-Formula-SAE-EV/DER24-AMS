/*
 * board.h
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

#include "ext_drivers/fans.h"
#include "ext_drivers/stm32f407g.h"
#include "ext_drivers/cli.h"

#define NFANS 10

typedef struct
{
	stm32f407g_t stm32f407g;
	// todo: change actual fan struct org. this is a demo
	fan_t fans[NFANS];
	cli_device_t cli;
} board_t;

void board_init(board_t *board);

#endif /* INC_BOARD_H_ */
