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

#define NSEGS 1
#define NMEAS 14

typedef struct
{
	float max_temp;
	float max_volt;
	float min_volt;
	ltc6813_driver_t ltc;
	cell_asic arr[NSEGS];
	ltc681x_conf_t cfg;
} accumulator_t;

void accumulator_init(accumulator_t *dev,
				      SPI_HandleTypeDef *hspi_a,
					  SPI_HandleTypeDef *hspi_b,
					  GPIO_TypeDef *cs_port_a,
					  GPIO_TypeDef *cs_port_b,
					  uint16_t cs_pin_a,
					  uint16_t cs_pin_b
					  );
int accumulator_read(accumulator_t *dev);

#endif /* INC_EXT_DRIVERS_ACCUMULATOR_H_ */
