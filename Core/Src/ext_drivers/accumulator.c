/*
 * accumulator.c
 *
 *  Created on: Feb 1, 2024
 *      Author: cole
 */

#include "ext_drivers/accumulator.h"
#include <math.h>

void accumulator_init(accumulator_t *dev,
				      SPI_HandleTypeDef *hspi_a,
					  SPI_HandleTypeDef *hspi_b,
					  GPIO_TypeDef *cs_port_a,
					  GPIO_TypeDef *cs_port_b,
					  uint16_t cs_pin_a,
					  uint16_t cs_pin_b
					 )
{
	dev->total_volt = 0;

	dev->cfg.OV_THRESHOLD = 0;

	dev->cfg.REFON = 1; //!< Reference Powered Up Bit
	dev->cfg.ADCOPT = 0; //!< ADC Mode option bit
	//!< GPIO Pin Control // Gpio 1,2,3,4,5
	for(int i = 0; i < 5; i++) dev->cfg.GPIOBITS_A[i] = 1;
	//!< GPIO Pin Control // Gpio 6,7,8,9
	for(int i = 0; i < 4; i++) dev->cfg.GPIOBITS_B[i] = 1;
	dev->cfg.UV=dev->cfg.UV_THRESHOLD; //!< Under voltage Comparison Voltage
	dev->cfg.OV=dev->cfg.OV_THRESHOLD; //!< Over voltage Comparison Voltage
	//!< Discharge cell switch //Dcc 1,2,3,4,5,6,7,8,9,10,11,12
	for(int i = 0; i < 12; i++) dev->cfg.DCCBITS_A[i] = 0;
	//!< Discharge cell switch //Dcc 0,13,14,15
	for(int i = 0; i < 7; i++) dev->cfg.DCCBITS_B[i] = 0;
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

int accumulator_read_volt(accumulator_t *dev)
{
	ltc6813_driver_t *ltc = &dev->ltc;
	int ret = 0;
	int error = 0;
	uint32_t conv = 0;

	wakeup_sleep(ltc);
	// call MUTE
	LTC6813_adcv(ltc, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
	// call UNMUTE
	conv = LTC6813_pollAdc(ltc);
    wakeup_sleep(ltc);
    do{
    	error = LTC6813_rdcv(ltc, REG_ALL);
    } while(error == -1);
    ret |= accumulator_convert_volt(dev);
    return ret;
}

int accumulator_read_temp(accumulator_t *dev)
{
	ltc6813_driver_t *ltc = &dev->ltc;
	int error = 0;
	uint32_t conv = 0;

	for(int channel = 0; channel < 8; channel++)
	{
		accumulator_set_temp_ch(dev, channel);
		wakeup_sleep(ltc);
		LTC6813_adax(ltc, MD_7KHZ_3KHZ, AUX_CH_ALL);
		conv = LTC6813_pollAdc(ltc);
		wakeup_sleep(ltc);
		error = LTC6813_rdaux(ltc, REG_ALL); // Set to read back all aux registers
		error |= accumulator_convert_temp(dev, channel);
	}
	error |= accumulator_stat_temp(dev);
	return error;
}

int accumulator_convert_volt(accumulator_t *dev)
{
	int seg, row;
	float total = 0;
	float max = -0.3, min = 21;
	float seg_total, seg_min, seg_max;
	float volt;

	for(seg = 0; seg < NSEGS; seg++)
	{
		seg_total = 0;
		seg_min = 21;
		seg_max = -0.3;
		for(row = 0; row < NCELLS; row++)
		{
			volt = (float)dev->arr[seg].cells.c_codes[row] * 0.0001;
			dev->arr[seg].voltage[row] = volt;
			total += volt;
			seg_total += volt;
			if(volt > seg_max) seg_max = volt;
			if(volt < seg_min) seg_min = volt;
			if(volt > max) max = volt;
			if(volt < min) min = volt;
		}
		dev->arr[seg].min_volt = seg_min;
		dev->arr[seg].max_volt = seg_max;
		dev->arr[seg].total_volt = seg_total;
	}
	dev->min_volt = min;
	dev->max_volt = max;
	dev->total_volt = total;

	return 0;
}

int accumulator_convert_temp(accumulator_t *dev, int channel)
{
	int seg;
	float temp[2] = {0};
	float volt[2] = {0};
	float ratio[2] = {0};

	for(seg = 0; seg < NSEGS; seg++)
	{
		volt[0] = (float)dev->arr[seg].aux.a_codes[0] * 0.0001;
		volt[1] = (float)dev->arr[seg].aux.a_codes[1] * 0.0001;
		ratio[0] = (5.0 / volt[0]) - 1;
		ratio[1] = (5.0 / volt[1]) - 1;
		temp[0] = NXFT15XV103FEAB050_convert(ratio[0]);
		temp[1] = NXFT15XV103FEAB050_convert(ratio[1]);
		//temp[0] = volt[0];
		//temp[1] = volt[1];
		dev->arr[seg].temp[channel]     = temp[0];
		dev->arr[seg].temp[channel + 8] = temp[1];
	}
	return 0;
}

int accumulator_stat_temp(accumulator_t *dev)
{
	int seg, row;
	float max = 0;
	float seg_max;
	float temp;

	for(seg = 0; seg < NSEGS; seg++)
	{
		seg_max = 0;
		for(row = 0; row < NTEMPS; row++)
		{
			temp = dev->ltc.ic_arr[seg].temp[row];
			if(temp > seg_max) seg_max = temp;
			if(temp > max) max = temp;
		}
		dev->ltc.ic_arr[seg].max_temp = seg_max;
	}
	dev->max_temp = max;
	return 0;
}

int accumulator_set_temp_ch(accumulator_t *dev, uint8_t channel)
{
	int error = 0;
	error |= accumulator_set_mux_ch(dev, channel, MUX_ADDR7_00);
	error |= accumulator_set_mux_ch(dev, channel, MUX_ADDR7_01);
    return error;
}

int accumulator_set_mux_ch(accumulator_t *dev, uint8_t channel, uint8_t addr7)
{
	int error = 0;
	uint8_t data[3] = {0};
	uint8_t icom[3] = {0};
	uint8_t fcom[3] = {0};
	uint8_t com[6] = {0};

	if(channel > 7) return 1;

	icom[0] = ICOM_START;
	data[0] = addr7 << 1;
	fcom[0] = FCOM_NACK;

	icom[1] = ICOM_BLANK;
	data[1] = 1 << channel;
	fcom[1] = FCOM_NACK_STOP;

	icom[2] = ICOM_NT;
	data[2] = 0xFF;
	fcom[2] = FCOM_NACK_STOP;

	// COMM0: ICOM0[3:0]  D0[7:4]
	// COMM1: D0[3:0]     FCOM0[3:0]
	// COMM2: ICOM1[3:0]  D1[7:4]
	// COMM3: D1[3:0]     FCOM1[3:0]
	// COMM4: ICOM2[3:0]  D2[7:4]
	// COMM5: D2[3:0]     FCOM2[3:0]
	for(uint8_t byte = 0; byte < 3; byte++)
	{
		com[byte * 2]     = (icom[byte] << 4) | (data[byte] >> 4);
		com[byte * 2 + 1] = (data[byte] << 4) | (fcom[byte] & 0xF);
	}

    for (uint8_t current_ic = 0; current_ic < dev->ltc.num_ics; current_ic++)
    {
    	for(uint8_t byte = 0; byte < 8; byte++)
    	{
    		dev->ltc.ic_arr[current_ic].com.tx_data[byte]= com[byte];
    	}
    }
    wakeup_sleep(&dev->ltc);
    LTC6813_wrcomm(&dev->ltc);
    wakeup_idle(&dev->ltc);
    LTC6813_stcomm(&dev->ltc, 3);
    return error;
}

float NXFT15XV103FEAB050_convert(float ratio)
{
	// TODO: Verify
	double a = 104.517;
	double b = 0.221876;
	return a * pow(b, ratio);
}
