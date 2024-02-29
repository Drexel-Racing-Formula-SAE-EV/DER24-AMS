/*
 * fan_task.c
 *
 *  Created on: Feb 5, 2024
 *      Author: Cassius Garcia
 */
#include "tasks/fan_task.h"
//#include "main.h"

void fan_task_fn(void *argument);

TaskHandle_t fan_task_start(app_data_t *data){
	TaskHandle_t handle;
	xTaskCreate(fan_task_fn, "fan task", 128, (void *)data, 7, &handle);
	return handle;
}

void fan_task_fn(void *argument)
{
	app_data_t *app_data = (app_data_t *) argument;
	float duty = 0.0;

	/*
	for (int i = 0; i < NFANS; i++) {
		set_fan_percent(&app_data->board.fans[i], 0.0);
	}
	*/

	for(;;)
	{
		for (int i = 0; i < NFANS; i++) {
			set_fan_percent(&app_data->board.fans[i], duty);
		}
		duty += 10.0;
		if(duty > 100.0) duty = 0.0;

		osDelay(5000);
	}
}



