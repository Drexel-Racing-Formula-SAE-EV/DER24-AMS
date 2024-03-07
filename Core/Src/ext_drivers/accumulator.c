/*
 * accumulator.c
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#include "ext_drivers/accumulator.h"

void accumulator_init(accumulator_t *acc)
{

}

int accumulator_read(accumulator_t *acc)
{
	int ret = 0;
	ltc6813_readcv(&acc->ltc681x_driver);

	return ret;
}

