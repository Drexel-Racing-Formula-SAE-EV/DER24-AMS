/*
 * error_task.c
 *
 *  Created on: June 04, 2024
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
	bool prev_air = false;
	int errors;

	for(;;)
	{
		entry = osKernelGetTickCount();

		prev_air = data->air_state;
		data->air_state = HAL_GPIO_ReadPin(AIR_CONTROL_MCU_GPIO_Port, AIR_CONTROL_MCU_Pin);
		/*
		if(prev_air && !data->air_state)
		{
			set_bms(0);
			osDelay(50);
		}*/

		if(!data->ltc_fault)
		{
			errors = 0;
			errors += check_current(data);
			errors += check_volt(data);
			errors += check_temp(data);
			errors += data->charger_fault;

			if(errors > 0) data->hard_fault = true;
			data->soft_fault = check_soft_fault(data);

			set_bms(!data->hard_fault);
		}
		else set_bms(0);

		osDelayUntil(entry + (1000 / ERR_FREQ));
	}
}

int check_current(app_data_t *data)
{
	if(data->state == STATE_DISCHARGE) return data->current >= OVERCURR;
	else if( data->state == STATE_CHARGE) return data->current <= (-1.0 * OVERCURR);
	return 0;
}

int check_volt(app_data_t *data)
{
	return data->max_voltage >= OVERVOLT || data->min_voltage <= UNDERVOLT;
}

int check_temp(app_data_t *data)
{
	return data->max_temp >= TEMP_THRESH_C;
}

int check_soft_fault(app_data_t *data)
{
	if(data->canbus_fault
	   || data->current_fault
	   || data->fan_fault
	   || data->cli_fault
	  ) return 1;
	return 0;
}



