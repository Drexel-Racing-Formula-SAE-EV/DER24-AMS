/*
 * error_task.c
 *
 *  Created on: May 30, 2024
 *      Author: Cassius Garcia
 */

#include "tasks/error_task.h"

void error_task_fn(void *argument);

int check_soft_fault(app_data_t *data);

int check_current(app_data_t *data);

int check_volt(app_data_t *data);

int check_temp(app_data_t *data);

TaskHandle_t error_task_start(app_data_t *data){
	TaskHandle_t handle;
	xTaskCreate(error_task_fn, "error task", 128, (void *)data, ERR_PRIO, &handle);
	return handle;
}

void error_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();

		int errors;
		errors = errors + check_current(data);
		errors = errors + check_volt(data);
		errors = errors + check_temp(data);

		if ( errors > 0 ) {
			data->hard_fault = true;
		}

		if ( check_soft_fault(data) ) {
			data->soft_fault = true;
		}

		if ( data->hard_fault ) {
			set_bms(0);
		}

		osDelayUntil(entry + (1000 / ERR_FREQ));
	}
}

int check_current(app_data_t *data){
	if ( data->state == STATE_DISCHARGE ){
		if ( data->current >= OVERCURR ) return 1;
	}
	else if( data->state == STATE_CHARGE) {
		if ( data->current <= ( -1.0 * OVERCURR ) ) return 1;
	}

	return 0;
}

int check_volt(app_data_t *data){
	if ( data->max_voltage >= OVERVOLT || data->min_voltage <= UNDERVOLT){
		return 1;
	}
	return 0;
}

int check_temp(app_data_t *data){
	if ( data->max_temp >= TEMP_THRESH_C ){
		return 1;
	}
	return 0;
}

int check_soft_fault(app_data_t *data){
	if ( data->canbus_fault
				|| data->current_fault
				|| data->fan_fault
				|| data->cli_fault ){
		return 1;
	}
	return 0;
}



