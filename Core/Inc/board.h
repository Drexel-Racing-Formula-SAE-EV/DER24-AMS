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
#include "ext_drivers/imd.h"
#include "ext_drivers/cli.h"
#include "ext_drivers/canbus.h"
#include "ext_drivers/current_sensor.h"
#include "ext_drivers/accumulator.h"
#include "ext_drivers/charger.h"

#define NFANS 10

typedef struct
{
	stm32f407g_t stm32f407g;
	charger_t charger;
	current_sensor_t current_sensor;
	fan_t fans[NFANS];
	imd_t imd;
	cli_t cli;
	canbus_t canbus;
} board_t;

void board_init(board_t *board);

#endif /* INC_BOARD_H_ */
