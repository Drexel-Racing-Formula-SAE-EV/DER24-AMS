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
