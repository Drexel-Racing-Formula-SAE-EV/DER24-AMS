/*
*   imd_task.h
*   Created: 3/25/2024
*   Author: Brendan Hoag
*   Purpose: Device driver task for IR151-3204 ground fault monitoring system
*   datasheet: https://www.benderinc.com/products/ground-fault-monitoring-ungrounded/isometer-ir155-03-04-series/
*/

#include "tasks/imd_task.h"
#define EVER ;;
/*
* function: imd_task_fn
* ---------------------
* The actual function the task will run
* - Reads from data pins
* - Update status, frequency, duty cycle 
* - Call shutdown on error -> might be job of super manager task rather than imd
*
* argument - a void pointer to be cast back to an app_data_t to get appropriate data from
*/
void imd_task_fn(void *argument);


TaskHandle_t imd_task_start(app_data_t *data){
    TaskHandle_t handle;
    // todo: appropriate task priority
    xTaskCreate(imd_task_fn, "imd task", 128, (void *) data, 6, &handle);
    return handle;
}

// todo: fix channel #, not sure which channel this should exist on atm
void imd_task_fn(void *argument){
    app_data_t *data;
    imd_t *imd;
    
    data = (app_data_t *) argument;
    imd = data->board->imd;

    for(EVER){
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {  // If the interrupt is triggered by channel 1 (fix channel #)
            uint32 ICValue = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            if (ICValue != 0) {
                //add 1 to duty cycle for some reason! but its accurate
                imd->duty_cycle = (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) *100)/ICValue;
                imd->frequency = 168000000/ICValue;
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

}
