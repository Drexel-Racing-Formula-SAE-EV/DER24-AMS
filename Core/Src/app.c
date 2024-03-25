/*
 * app.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "app.h"

#include "cmsis_os.h"
#include "tasks/fan_task.h"
#include "tasks/cli_task.h"
#include "tasks/canbus_task.h"

app_data_t app = {0};

void app_create()
{
	app.hard_fault = false;
	app.soft_fault = false;
	app.fan_fault = false;
	app.cli_fault = false;
	app.canbus_fault = false;

	app.air_state = false;

	// TODO: revise app data struct

	app.imd_state = false;
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

	HAL_UART_Receive_IT(app.board.cli.huart, &app.board.cli.c, 1);

	set_bms(1);

	assert(app.cli_task = cli_task_start(&app));
	assert(app.fan_task = fan_task_start(&app));
	assert(app.canbus_task = canbus_task_start(&app));
}

void set_bms(bool state)
{
	HAL_GPIO_WritePin(BMS_SAFETY_OUT_GPIO_Port, BMS_SAFETY_OUT_Pin, state);
}
