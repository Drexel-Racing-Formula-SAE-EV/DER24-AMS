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
	dev->cfg.OV_THRESHOLD = 0;

	dev->cfg.REFON = 1; //!< Reference Powered Up Bit
	dev->cfg.ADCOPT = 0; //!< ADC Mode option bit
	//!< GPIO Pin Control // Gpio 1,2,3,4,5
	dev->cfg.GPIOBITS_A[0] = 1;
	dev->cfg.GPIOBITS_A[1] = 1;
	dev->cfg.GPIOBITS_A[2] = 1;
	dev->cfg.GPIOBITS_A[3] = 1;
	dev->cfg.GPIOBITS_A[4] = 1;
	//!< GPIO Pin Control // Gpio 6,7,8,9
	for(int i = 0; i<4;i++){
		dev->cfg.GPIOBITS_B[i]=1;
	}
	dev->cfg.UV=dev->cfg.UV_THRESHOLD; //!< Under voltage Comparison Voltage
	dev->cfg.OV=dev->cfg.OV_THRESHOLD; //!< Over voltage Comparison Voltage
	//!< Discharge cell switch //Dcc 1,2,3,4,5,6,7,8,9,10,11,12
	for(int i = 0; i<4;i++){
		dev->cfg.DCCBITS_A[i]=0;
	}
	//!< Discharge cell switch //Dcc 0,13,14,15
	for(int i = 0; i<7;i++){
		dev->cfg.DCCBITS_B[i]=0;
	}
	//!< Discharge time value //Dcto 0,1,2,3  // Programed for 4 min
	dev->cfg.DCTOBITS[0] = 1;
	dev->cfg.DCTOBITS[1] = 0;
	dev->cfg.DCTOBITS[2] = 1;
	dev->cfg.DCTOBITS[3] = 0;
	/*Ensure that Dcto bits are set according to the required discharge time. Refer to the data sheet */
	dev->cfg.FDRF = 0; //!< Force Digital Redundancy Failure Bit
	dev->cfg.DTMEN = 1; //!< Enable Discharge Timer Monitor
	//!< Digital Redundancy Path Selection//ps-0,1
	dev->cfg.PSBITS[0]= 0;
	dev->cfg.PSBITS[1]= 0;
	ltc6813_driver_t *ltc = &dev->ltc;
	LTC6813_init(ltc,
				 hspi_a,
				 hspi_b,
				 cs_port_a,
				 cs_port_b,
				 cs_pin_a,
				 cs_pin_b,
				 NSEGS,
				 dev->arr
				);
	wakeup_sleep(ltc);
	LTC6813_init_cfg(ltc); // to set all zeros
	LTC6813_init_cfgb(ltc);

	for(int i = 0; i < NSEGS; i++)
	{
	    LTC6813_set_cfgr(ltc,
	    				 i,
						 dev->cfg.REFON,
						 dev->cfg.ADCOPT,
						 dev->cfg.GPIOBITS_A,
						 dev->cfg.DCCBITS_A,
						 dev->cfg.DCTOBITS,
						 dev->cfg.UV,
						 dev->cfg.OV);
	    LTC6813_set_cfgrb(ltc,
	    		  	  	  i,
						  dev->cfg.FDRF,
						  dev->cfg.DTMEN,
						  dev->cfg.PSBITS,
						  dev->cfg.GPIOBITS_B,
						  dev->cfg.DCCBITS_B);
	}
	LTC6813_wrcfg(ltc); // write config a
	LTC6813_wrcfgb(ltc); // write config b
	LTC6813_reset_crc_count(ltc);
	LTC6813_init_reg_limits(ltc);
}

int accumulator_read(accumulator_t *dev)
{
	ltc6813_driver_t *ltc = &dev->ltc;
	int ret = 0;
	int8_t error = 0;
	uint32_t conv = 0;

	wakeup_sleep(ltc);
	LTC6813_adcv(ltc, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
	conv = LTC6813_pollAdc(ltc);
    wakeup_sleep(ltc);
    do{
    	error = LTC6813_rdcv(ltc, REG_ALL); // Set to read back all cell voltage registers
    	//check_error(error);
    } while(error == -1);


	// Convert to voltage, min max stuff too
	return ret;
}

int convert_cell_reads(accumulator_t *dev)
{
	int seg, row;

	for(seg = 0; seg < NSEGS; seg++){
		for(row = 0; row < 18; row++){
			dev->arr[seg].voltage[row] = ((dev->arr[seg].cells.c_codes[row] / 65535.0) * 21.3) - 0.3;
		}
	}

	return 0;

}
