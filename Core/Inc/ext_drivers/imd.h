/*
*   imd.h
*   Created: 2/5/2024
*   Author: Brendan Hoag
*   Purpose: Device driver for IR151-3204 ground fault monitoring system
*   datasheet: https://www.benderinc.com/products/ground-fault-monitoring-ungrounded/isometer-ir155-03-04-series/
*/
#ifndef INC_IMD_H
#define INC_IMD_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/*
* enum: imd_status_t
* ------------------
* Represents a status message read from the IMD
* Each status comes in as a multiple of 10Hz
* To find current status, divide reading from PWM by 10
*/
typedef enum 
{
    SHORT_TO_CHASSIS_GROUND_STATUS,
    NORMAL_STATUS,
    UNDERVOLT_STATUS,
    SPEED_START_STATUS,
    DEVICE_ERROR_STATUS,
    GROUND_FAULT_STATUS
} imd_status_t;

/*
* struct: imd_t
* -------------
* Represents an IR151-3204 ground fault monitoring system
*
* OK_HS: a boolean representing the high-level status of the system
*   - Corresponds to pin 8 OK_HS
*   - A high bit is good, a low bit is some error state
*
* status: an imd_status_t representing the status of the IMD
*   - Should be NORMAL_CONDITION (1) if OK_HS is 1
*   - Will be another status if OK_HS is 0
*   - Update based on frequency
*
* timer: a TIM_TypeDef pointer representing the associated timer of the IMD
*
* frequency: a float representing the data out PWM signal on the high side
*   - Corresponds to pin 5 M_HS on IR151-3204, pin 6 on IR151-3203
*   - Tells us what state the IMD is in
*
* duty_cycle: a float representing the duty cycle of the IMD (expressed as a percent)
*/
typedef struct
{
    bool OK_HS;
    imd_status_t status;
    TIM_TypeDef *timer;
    float frequency;
    float duty_cycle;
} imd_t;

/*
* function: imd_init
* ------------------
*
* imd: a pointer to and imd_t we want to initialize
*/
void imd_init(imd_t* imd);

/*
* function: set_imd_status_signal
* -------------------------------
*
* imd: a pointer to an imd_t we want to update
*/
void set_imd_status_signal(imd_t* imd);

/*
* function: HAL_TIM_IC_CaptureCallback
* ------------------------------------
*
* 
*/
void HAL_TIM_IC_CaptureCallback(imd_t* imd, TIM_HandleTypeDef *htim);

#endif