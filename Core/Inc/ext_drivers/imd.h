/*
* imd.h
*   Created: 2/5/2024
*   Author: Brendan Hoag
*   Purpose: Device driver for IR151-3204 ground fault monitoring system
*                              TBD version: B91068139V4 (non customizable)
*                                           B91068139CV4 (customizable)
*   datasheet: https://www.benderinc.com/products/ground-fault-monitoring-ungrounded/isometer-ir155-03-04-series/
*/
#ifndef INC_IMD_H
#define INC_IMD_H

#include <stdint.h>

// todo: Non dummy values
#define OK_HI 0 // no fault; RF > response value
#define OK_LO 0  // insulation resistance <= response value
                  // Device error; fault in earth connection
                  // undervoltage detected, device switched off

typedef struct
{
    // status signal
    bool OK_HS; // Supply - 2V

    // pwm signals
    float measurement_High;
    float measurement_Low;
} imd_t;



#endif