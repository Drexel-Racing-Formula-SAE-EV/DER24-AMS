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
#define NCELLS 14
#define NTEMPS 16
#define MUX_ADDR7_00 0x4C
#define MUX_ADDR7_01 0x4D
#define VNTC 5.0

typedef struct
{
	float total_volt;
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
int accumulator_read_volt(accumulator_t *dev);
int accumulator_read_temp(accumulator_t *dev);
int accumulator_convert_volt(accumulator_t *dev);
int accumulator_convert_temp(accumulator_t *dev, int channel);
int accumulator_set_temp_ch(accumulator_t *dev, uint8_t channel);
int accumulator_stat_temp(accumulator_t *dev);
int accumulator_set_mux_ch(accumulator_t *dev, uint8_t channel, uint8_t addr7);
float NXFT15XV103FEAB050_convert(float ratio);

#endif /* INC_EXT_DRIVERS_ACCUMULATOR_H_ */
