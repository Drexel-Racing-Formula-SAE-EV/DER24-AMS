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
	xTaskCreate(ltc_task_fn, "ltc task", 128, (void *)data, LTC_PRIO, &handle);
	return handle;
}

void ltc_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	accumulator_t *acc = &data->acc;
	uint32_t entry;

	for(;;)
	{
        entry = osKernelGetTickCount();
        accumulator_read_volt(acc);
        data->total_voltage = acc->total_volt;
        data->max_voltage = acc->max_volt;
        data->min_voltage = acc->min_volt;
        accumulator_read_temp(acc);
        data->max_temp = acc->max_temp;
        osDelayUntil(entry + (1000 / LTC_FREQ));
	}
}



