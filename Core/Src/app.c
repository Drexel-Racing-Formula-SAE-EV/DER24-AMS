/*
 * app.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "app.h"

app_data_t app_data = {0};

void app_create()
{
	app_data.hardFault = false;
	app_data.softFault = false;
	app_data.IMD_fault = false;
	app_data.IMD_freq = 0.0;
	app_data.IMD_duty = 0.0;

	app_data.state = STATE_START;

	app_data.max_temp = 0.0;
	app_data.avg_temp = 0.0;
	app_data.max_voltage = 0.0;
	app_data.min_voltage = 0.0;
	app_data.current = 0.0;

	board_init(&app_data.board);
	accumulator_init(&app_data.accumulator);
}
