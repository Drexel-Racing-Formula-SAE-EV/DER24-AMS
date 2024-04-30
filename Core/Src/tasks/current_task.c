/*
 * currentt_task.c
 *
 *  Created on: Apr 15, 2024
 *      Author: Justin Nguyen
 */
#include "tasks/current_task.h"
//#include "main.h"

void current_task_fn(void *argument);

TaskHandle_t current_task_start(app_data_t *data){
	TaskHandle_t handle;
	xTaskCreate(current_task_fn, "current task", 128, (void *)data, 7, &handle);
	return handle;
}

void current_task_fn(void *argument)
{
	app_data_t *app_data = (app_data_t *) argument;
	current_sensor_t *current_sensor_e = &app_data->board.current_sensor;
	uint32_t entry ;



	for(;;)
	{ 
		entry = osKernelGetTickCount();
		
		// use the currrent sensor here
		// check the app fault and  chec the error task and check the macro as well 

		stm32f407g_adc_switch_channel(current_sensor_e->hadc_high,current_sensor_e->channel_high);
		current_sensor_e->count_high = stm32f407g_adc_read(current_sensor_e->hadc_high);
		stm32f407g_adc_switch_channel(current_sensor_e->hadc_low,current_sensor_e->channel_low);
		current_sensor_e->count_low  = stm32f407g_adc_read(current_sensor_e->hadc_low);

		current_sensor_convert(&app_data->board.current_sensor);

		app_data->current = current_sensor_e->current;


		osDelayUntil(entry +(1000/CURRENT_FREQ));
	}
}



