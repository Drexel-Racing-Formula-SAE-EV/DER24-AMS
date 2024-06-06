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

int error_check(app_data_t *data);

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


		int cell_to_discharge = 0; //TEMP VALUE

		if ( data->state == STATE_BALANCE ){
			//TODO: add logic to select which cell to balance
			start_balancing(data, cell_to_discharge);
			error_check(data);
			// Keep balancing until the state changes
			while ( data->state == STATE_BALANCE ){
				//TODO: add a print statement for cells to balance???
				osDelayUntil(entry + (1000 / CELL_FREQ));
			}
			stop_balancing(data);
			error_check(data);
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

int error_check(app_data_t *data){
	int error = 0;
	error = LTC6813_rdcfg(&data->acc.ltc);
	if (error == -1){
		data->cell_fault = true;
		return -1;
	}
	return 0;
}
