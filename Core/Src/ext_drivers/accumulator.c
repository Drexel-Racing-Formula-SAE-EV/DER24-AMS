/*
 * accumulator.c
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#include "ext_drivers/accumulator.h"

void accumulator_init(accumulator_t *dev,
				      SPI_HandleTypeDef *hspi_a,
					  SPI_HandleTypeDef *hspi_b,
					  GPIO_TypeDef *cs_port_a,
					  GPIO_TypeDef *cs_port_b,
					  uint16_t cs_pin_a,
					  uint16_t cs_pin_b
					 )
{
	LTC6813_init(&dev->ltc,
				 hspi_a,
				 hspi_b,
				 cs_port_a,
				 cs_port_b,
				 cs_pin_a,
				 cs_pin_b,
				 NSEGS,
				 dev->arr
				);
}

int accumulator_read(accumulator_t *dev)
{
	int ret = 0;

	return ret;
}

