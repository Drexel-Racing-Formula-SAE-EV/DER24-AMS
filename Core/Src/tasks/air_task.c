/*
 * air_task.c
 *
 *  Created on: Apr 3, 2024
 *      Author: Cole Bardin
 */
#include "tasks/air_task.h"

void air_task_fn(void *argument);

TaskHandle_t air_task_start(app_data_t *data){
	TaskHandle_t handle;
	xTaskCreate(air_task_fn, "air task", 128, (void *)data, 7, &handle);
	return handle;
}

void air_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();
		data->air_state = HAL_GPIO_ReadPin(AIR_CONTROL_MCU_GPIO_Port, AIR_CONTROL_MCU_Pin);
		osDelayUntil(entry + (1000 / AIR_FREQ));
	}
}



