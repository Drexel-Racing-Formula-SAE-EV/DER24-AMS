/*
 * cells.c
 *
 *  Created on: Jun 3, 2024
 *      Author: Cassius Garcia
 */

#include "tasks/cell_task.h"

void cell_task_fn(void *argument);

void stop_balancing(app_data_t *data);

void start_balancing(app_data_t *data, int cell);

TaskHandle_t cell_task_start(app_data_t *data){
	TaskHandle_t handle;
	xTaskCreate(cell_task_fn, "cell task", 128, (void *)data, CELL_PRIO, &handle);
	return handle;
}

void cell_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();
		//TODO: add logic to select which cell to balance

		int temp_cell_val; //TEMP VALUE

		if ( data->state == STATE_BALANCE ){
			start_balancing(data, temp_cell_val);
			// Keep balancing until the state changes
			while ( data->state == STATE_BALANCE ){
				osDelayUntil(entry + (1000 / CELL_FREQ));
			}
			stop_balancing(data);
		}

		osDelayUntil(entry + (1000 / CELL_FREQ));
	}
}

void start_balancing(app_data_t *data, int cell){
	wakeup_sleep(&data->acc.ltc);
	LTC6813_set_discharge(&data->acc.ltc, cell);
	LTC6813_wrcfg(&data->acc.ltc);
	LTC6813_wrcfgb(&data->acc.ltc);
	wakeup_idle(&data->acc.ltc);
}

void stop_balancing(app_data_t *data){
	wakeup_sleep(&data->acc.ltc);
	LTC6813_clear_discharge(&data->acc.ltc);
	LTC6813_wrcfg(&data->acc.ltc);
	LTC6813_wrcfgb(&data->acc.ltc);
	wakeup_idle(&data->acc.ltc);
}
