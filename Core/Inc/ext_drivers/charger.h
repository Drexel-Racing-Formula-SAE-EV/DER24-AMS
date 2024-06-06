/*
 * charger.h
 *
 *  Created on: May 29, 2024
 *      Author: cole
 */

#ifndef _CHARGER_H__
#define _CHARGER_H__

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "ext_drivers/canbus.h"

typedef struct
{
	float target_voltage;
	float target_current;
	float read_voltage;
	float read_current;
	bool hardware_fail;
	bool overtemp_fail;
	bool input_volt_fail;
	bool voltage_sense_fail;
	bool communication_fail;
	canbus_device_t *canbus;
	uint32_t tx_count;
	uint32_t rx_count;
	uint8_t flags;
} charger_t;

void charger_init(charger_t *dev, canbus_device_t *canbus);

#endif /* _CHARGER_H__ */
