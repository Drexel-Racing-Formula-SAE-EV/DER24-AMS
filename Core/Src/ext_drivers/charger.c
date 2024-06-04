/*
 * charger.c
 *
 *  Created on: May 29, 2024
 *      Author: cole
 */

#include "ext_drivers/charger.h"

void charger_init(charger_t *dev, canbus_device_t *canbus)
{
	dev->target_voltage = 0.0;
	dev->target_current = 0.0;
	dev->read_voltage = 0.0;
	dev->read_current = 0.0;
	dev->hardware_fail = 0;
	dev->overtemp_fail = 0;
	dev->input_volt_fail = 0;
	dev->voltage_sense_fail = 0;
	dev->communication_fail = 0;
	dev->canbus = canbus;
	dev->tx_count = 0;
	dev->rx_count = 0;
	dev->flags = 0;
}
