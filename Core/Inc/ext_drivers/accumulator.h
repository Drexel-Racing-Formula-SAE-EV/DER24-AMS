/*
 * accumulator.h
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#ifndef INC_EXT_DRIVERS_ACCUMULATOR_H_
#define INC_EXT_DRIVERS_ACCUMULATOR_H_

#include <stdbool.h>

#define NSEGS 5
#define NMEAS 14

typedef struct
{
	bool valid;

	// ltc device here too!

	float volts[NMEAS];
	float temps[NMEAS];

	float max_temp;
	float max_volt;
	float min_volt;
} segment_t;

typedef struct
{
	segment_t segments[NSEGS];

	float max_temp;
	float max_volt;
	float min_volt;
} accumulator_t;

void segment_init(segment_t *seg, float default_val);
int segment_read(segment_t *seg);

void accumulator_init(accumulator_t *acc);
int accumulator_read(accumulator_t *acc);

#endif /* INC_EXT_DRIVERS_ACCUMULATOR_H_ */
