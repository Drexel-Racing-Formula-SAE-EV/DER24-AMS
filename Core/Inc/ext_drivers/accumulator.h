/*
 * accumulator.h
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#ifndef INC_EXT_DRIVERS_ACCUMULATOR_H_
#define INC_EXT_DRIVERS_ACCUMULATOR_H_

#include <stdbool.h>
#include "ext_drivers/LTC6813.h"

#define NSEGS 5
#define NMEAS 14

typedef struct
{
	ltc681x_driver_t ltc681x_driver;
	cell_asic ic_arr[NSEGS];

	float max_temp;
	float max_volt;
	float min_volt;
} accumulator_t;

void accumulator_init(accumulator_t *acc);
int accumulator_read(accumulator_t *acc);

#endif /* INC_EXT_DRIVERS_ACCUMULATOR_H_ */
