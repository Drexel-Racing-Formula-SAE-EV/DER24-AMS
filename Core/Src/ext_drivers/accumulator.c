/*
 * accumulator.c
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#include "ext_drivers/accumulator.h"

void segment_init(segment_t *seg, float default_val)
{
	seg->valid = false; // gets set once a measurement is taken

	for(int i = 0; i < NMEAS; i++)
	{
		seg->volts[i] = default_val;
		seg->temps[i] = default_val;
	}

	seg->max_temp = default_val;
	seg->max_volt = default_val;
	seg->min_volt = default_val;
}

int segment_read(segment_t *seg)
{
	// TODO: read LTC6813 and populate this stuct
	seg->valid = true;
	return 0;
}

void accumulator_init(accumulator_t *acc)
{
	for(int i = 0; i < NSEGS; i++)
	{
		segment_init(&acc->segments[i], 0.0);
	}
}

int accumulator_read(accumulator_t *acc)
{
	int ret = 0;
	for(int i = 0; i < NSEGS; i++)
	{
		ret |= segment_read(&acc->segments[i]);
	}
	return ret;
}

