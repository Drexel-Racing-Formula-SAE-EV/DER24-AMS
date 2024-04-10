/*
 * app.c
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#include "app.h"

#include "cmsis_os.h"
#include "tasks/fan_task.h"
#include "tasks/ltc_task.h"

app_data_t app = {0};

void app_create()
{
	app.hardFault = false;
	app.softFault = false;
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
	accumulator_init(&app.acc,
					 &app.board.stm32f407g.hspi1,
					 &app.board.stm32f407g.hspi3,
					 STRINGA_CS_GPIO_Port,
					 STRINGB_CS_GPIO_Port,
					 STRINGA_CS_Pin,
					 STRINGB_CS_Pin
					);

	assert(app.fan_task = fan_task_start(&app));
	assert(app.ltc_task = ltc_task_start(&app));
}
