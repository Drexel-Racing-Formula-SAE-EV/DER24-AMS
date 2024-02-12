/*
 * fan_task.c
 *
 *  Created on: Feb 5, 2024
 *      Author: Cassius Garcia
 */
#include "tasks/fan_task.h"
#include "cmsis_os.h"
#include "app.h"


void fan_task_fn(void *argument)
{
	app_data_t *app_data = (app_data_t *) argument;

	for (int i = 0; i < NFANS; i++) {
		set_fan_percent(&app_data->board.fans[i],25.0);
	}



  for(;;)
  {
    osDelay(1);
  }
}

