/*
*   imd.c
*   Created: 2/5/2024
*   Author: Brendan Hoag
*   Purpose: Device driver for IR151-3204 ground fault monitoring system
*   datasheet: https://www.benderinc.com/products/ground-fault-monitoring-ungrounded/isometer-ir155-03-04-series/
*/
#include "imd.h"

// todo: better dummy init values
void imd_init(imd_t* imd){
	imd->duty_cycle = 0.5;
	imd->frequency = 10.0;
	imd->OK_HS = true;
	imd->status = NORMAL_STATUS;
}

void set_imd_status_signal(imd_t* imd){
	imd->status = imd->frequency / 10;
}

void HAL_TIM_IC_CaptureCallback(imd_t* imd, TIM_HandleTypeDef *htim) {
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {  // If the interrupt is triggered by channel 1
		// Read the IC value
		uint32 ICValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		if (ICValue != 0) {
			// calculate the Duty Cycle
			imd->duty_cycle = (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) *100)/ICValue;//add 1 to duty cycle for some reason! but its accurate

			// calculate frequency
			imd->frequency = 168000000/ICValue;

			// calculate status
			imd->status = imd->frequency / 10;

			// set OK_HS state - can be read from pin 8 but should take ~ similar time to do calculation
			imd->OK_HS = imd->status == NORMAL_STATUS ? true : false;
			
			// old debug?
			/*if(i<5){
				printf("Duty:%f Freq:%ul\r\n",*a_d.Duty,*a_d.Freq);i++;
			}*/
		}
	}
}
