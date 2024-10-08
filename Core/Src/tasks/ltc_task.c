/*
 * ltc6813_task.h
 *
 *  Created on: Apr 10, 2024
 *      Author: Cole Bardin
 */

#include "tasks/ltc_task.h"
#include "ext_drivers/LTC6813.h"

void ltc_task_fn(void *argument);
void check_balance(app_data_t *data);

TaskHandle_t ltc_task_start(app_data_t *data)
{
	TaskHandle_t handle;
	xTaskCreate(ltc_task_fn, "ltc task", 512, (void *)data, LTC_PRIO, &handle);
	return handle;
}

void ltc_task_fn(void *argument)
{
	app_data_t *data = (app_data_t *) argument;
	accumulator_t *acc = &data->acc;
	uint8_t channel = 0;
	uint32_t entry;

	for(;;)
	{
        entry = osKernelGetTickCount();

        accumulator_read_volt(acc);
        data->total_voltage = acc->total_volt;
        data->max_voltage = acc->max_volt;
        data->min_voltage = acc->min_volt;
        accumulator_read_temp(acc, channel);
        if(++channel >= NTEMPCHS) channel = 0;
        data->max_temp = acc->max_temp;
        check_balance(data);

        data->ltc_fault = !(data->total_voltage != 0 && data->max_voltage < 6.5 && data->max_temp < 110.0);

        osDelayUntil(entry + (1000 / LTC_FREQ));
	}
}

void check_balance(app_data_t *data)
{
	accumulator_t *acc = &data->acc;
	ltc6813_driver_t *ltc = &acc->ltc;
	float min = data->min_voltage;
	acc->balance_cnt = 0;

	LTC6813_clear_discharge(ltc);
	if(data->state == STATE_BALANCE)
	{
		for(int seg = 0; seg < NSEGS; seg++)
		{
			ltc->ic_arr[seg].balance_cnt = 0;
			for(int cell = 0 + acc->balance_odds; cell < NCELLS; cell += 2)
			{
				if(ltc->ic_arr[seg].voltage[cell] - min > BALANCE_THRESH)
				{
					LTC6813_set_discharge_per_segment(ltc, cell, seg);
					acc->balance_cnt++;
					ltc->ic_arr[seg].balance_cnt++;
				}
			}
		}
		if(acc->balance_cnt == 0)
		{
			if(!acc->balance_odds) acc->balance_odds = true;
			else
			{
				acc->balance_odds = false;
				data->state = STATE_DISCHARGE;
				cli_printline(&data->board.cli, "Balancing Complete. Switching to Discharge Mode");
			}
		}
		wakeup_sleep(ltc);
		LTC6813_wrcfg(ltc);
		LTC6813_wrcfgb(ltc);
	}
	else if(acc->stop_balance)
	{
		acc->stop_balance = false;
		wakeup_sleep(ltc);
		LTC6813_wrcfg(ltc);
		LTC6813_wrcfgb(ltc);
	}

	return;
}

