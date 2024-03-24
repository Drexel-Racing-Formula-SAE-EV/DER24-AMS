/*
 * app.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "app.h"

#include "cmsis_os.h"
#include "tasks/fan_task.h"

app_data_t app = {0};

void app_create()
{
	app.hard_fault = false;
	app.soft_fault = false;
	app.fan_fault = false;
	app.cli_fault = false;

	// TODO: revise app data struct

	app.IMD_fault = false;
	app.IMD_freq = 0.0;
	app.IMD_duty = 0.0;

	app.state = STATE_START;

	app.max_temp = 0.0;
	app.avg_temp = 0.0;
	app.max_voltage = 0.0;
	app.min_voltage = 0.0;
	app.current = 0.0;

	board_init(&app.board);
	accumulator_init(&app.accumulator);

	assert(app.fan_task = fan_task_start(&app));
}
