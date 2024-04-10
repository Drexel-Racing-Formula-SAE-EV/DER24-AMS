/*
 * ltc6813_task.h
 *
 *  Created on: Apr 10, 2024
 *      Author: Cole Bardin
 */

#include "tasks/ltc_task.h"
#include "ext_drivers/LTC6813.h"

void ltc_task_fn(void *argument);

TaskHandle_t ltc_task_start(app_data_t *data)
{
	TaskHandle_t handle;
	xTaskCreate(ltc_task_fn, "ltc task", 128, (void *)data, 9, &handle);
	return handle;
}

void ltc_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	accumulator_t *acc = &data->acc;
	ltc6813_driver_t *ltc = &acc->ltc;
	uint32_t entry;

	for(;;)
	{
        entry = osKernelGetTickCount();

        osDelayUntil(entry + (1000 / LTC_FREQ));
	}
}



