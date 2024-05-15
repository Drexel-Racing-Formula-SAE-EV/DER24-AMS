/*! General BMS Library
***************************************************************************//**
*   @file     LTC681x.cpp
*   @author BMS (bms.support@analog.com)

********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/*! @file
    Library for LTC681x Multi-cell Battery Monitor
*/

#include <stdint.h>
#include "ext_drivers/LTC681x.h"

uint8_t blank_data[100] = {0xFF};

/* Wake isoSPI up from IDLE state and enters the READY state */
void wakeup_idle(ltc681x_driver_t *dev) //Number of ICs in the system
{
	for(int i = 0; i < dev->num_ics; i++)
	{
		spi_read_byte(dev, 0xFF, 1);
	}
}

/* Generic wakeup command to wake the LTC681x from sleep state */
void wakeup_sleep(ltc681x_driver_t *dev)
{
	for(int i = 0; i < dev->num_ics; i++)
	{
		// TODO: check delays. original was 300uS and 10uS
		LTC681x_set_cs(dev, 0);
		HAL_Delay(1);
		LTC681x_set_cs(dev, 1);
		HAL_Delay(1);
	}
}

/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(ltc681x_driver_t *dev, uint8_t tx_cmd[2]) //The command to be transmitted
{
	uint8_t cmd[4];
	uint16_t cmd_pec;
	//uint8_t md_bits;

	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	spi_write_array(dev, 4, cmd, 1);
}

/*
Generic function to write 68xx commands and write payload data.
Function calculates PEC for tx_cmd data and the data to be transmitted.
 */
void write_68(ltc681x_driver_t *dev, // device driver
			  uint8_t tx_cmd[2], //The command to be transmitted
			  uint8_t data[] // Payload Data
			  )
{
	const uint8_t BYTES_IN_REG = 6;
	const uint8_t CMD_LEN = 4+(8 * dev->num_ics);
	uint8_t *cmd;
	uint16_t data_pec;
	uint16_t cmd_pec;
	uint8_t cmd_index;

	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cmd_index = 4;
	for (uint8_t current_ic = dev->num_ics; current_ic > 0; current_ic--)               // Executes for each LTC681x, this loops starts with the last IC on the stack.
    {	                                                                            //The first configuration written is received by the last IC in the daisy chain
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
			cmd_index = cmd_index + 1;
		}

		data_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &data[(current_ic-1)*6]);    // Calculating the PEC for each ICs configuration register data
		cmd[cmd_index] = (uint8_t)(data_pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)data_pec;
		cmd_index = cmd_index + 2;
	}

	spi_write_array(dev, CMD_LEN, cmd, 1);

	free(cmd);
}

/* Generic function to write 68xx commands and read data. Function calculated PEC for tx_cmd data */
int8_t read_68( ltc681x_driver_t *dev, // device driver
				uint8_t tx_cmd[2], // The command to be transmitted
				uint8_t *rx_data // Data to be read
				)
{
	const uint8_t BYTES_IN_REG = 8;
	uint8_t cmd[4];
	uint8_t data[256];
	int8_t pec_error = 0;
	uint16_t cmd_pec;
	uint16_t data_pec;
	uint16_t received_pec;

	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);


	spi_write_read(dev, cmd, 4, data, (BYTES_IN_REG * dev->num_ics), 1);         //Transmits the command and reads the configuration data of all ICs on the daisy chain into rx_data[] array

	for (uint8_t current_ic = 0; current_ic < dev->num_ics; current_ic++) //Executes for each LTC681x in the daisy chain and packs the data
	{																//into the rx_data array as well as check the received data for any bit errors
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			rx_data[(current_ic*8)+current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
		}

		received_pec = (rx_data[(current_ic*8)+6]<<8) + rx_data[(current_ic*8)+7];
		data_pec = pec15_calc(6, &rx_data[current_ic*8]);

		if (received_pec != data_pec)
		{
		  pec_error = -1;
		}
	}

	return(pec_error);
}


const uint16_t crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  // precomputed CRC15 Table
                                0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                                0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                                0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                                0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                                0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                                0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                                0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                                0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                                0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                                0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                                0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                                0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                                0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                                0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                                0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                                0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                                0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                                0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                                0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                                0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                                0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                                0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                               };

/* Calculates  and returns the CRC15 */
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
                    uint8_t *data //Array of data that will be used to calculate  a PEC
                   )
{
	uint16_t remainder,addr;
	remainder = 16;//initialize the PEC

	for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
			remainder = (remainder<<8)^crc15Table[addr];
	}

	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

/* Write the LTC681x CFGRA */
void LTC681x_wrcfg(ltc681x_driver_t *dev)
{
	uint8_t cmd[2] = {0x00 , 0x01} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	for (uint8_t current_ic = 0; current_ic < dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = dev->ic_arr[c_ic].config.tx_data[data];
			write_count++;
		}
	}
	write_68(dev, cmd, write_buffer);
}

/* Write the LTC681x CFGRB */
void LTC681x_wrcfgb(ltc681x_driver_t *dev)
{
	uint8_t cmd[2] = {0x00 , 0x24} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	for (uint8_t current_ic = 0; current_ic < dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = dev->ic_arr[c_ic].configb.tx_data[data];
			write_count++;
		}
	}
	write_68(dev, cmd, write_buffer);
}

/* Read the LTC681x CFGA */
int8_t LTC681x_rdcfg(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x00 , 0x02};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;

	pec_error = read_68(dev, cmd, read_buffer);

	for (uint8_t current_ic = 0; current_ic<dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (int byte=0; byte<8; byte++)
		{
			dev->ic_arr[c_ic].config.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}

		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			dev->ic_arr[c_ic].config.rx_pec_match = 1;
		}
		else dev->ic_arr[c_ic].config.rx_pec_match = 0;
	}
	LTC681x_check_pec(dev, CFGRR);

	return(pec_error);
}

/* Reads the LTC681x CFGB */
int8_t LTC681x_rdcfgb(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x00 , 0x26};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;

	pec_error = read_68(dev, cmd, read_buffer);

	for (uint8_t current_ic = 0; current_ic<dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (int byte=0; byte<8; byte++)
		{
			dev->ic_arr[c_ic].configb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}

		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			dev->ic_arr[c_ic].configb.rx_pec_match = 1;
		}
		else dev->ic_arr[c_ic].configb.rx_pec_match = 0;
	}
	LTC681x_check_pec(dev, CFGRB);

	return(pec_error);
}

/* Starts ADC conversion for cell voltage */
void LTC681x_adcv(ltc681x_driver_t *dev, // device driver
				  uint8_t MD, //ADC Mode
				  uint8_t DCP, //Discharge Permit
				  uint8_t CH //Cell Channels to be measured
                 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;

	cmd_68(dev, cmd);
}

/* Start ADC Conversion for GPIO and Vref2  */
void LTC681x_adax(ltc681x_driver_t *dev, // device driver
				  uint8_t MD, //ADC Mode
				  uint8_t CHG //GPIO Channels to be measured
				 )
{
	uint8_t cmd[4];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x60 + CHG ;

	cmd_68(dev, cmd);
}

/* Start ADC Conversion for Status  */
void LTC681x_adstat(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //ADC Mode
				    uint8_t CHST //Stat Channels to be measured
				    )
{
	uint8_t cmd[4];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x68 + CHST ;

	cmd_68(dev, cmd);
}

/* Starts cell voltage and SOC conversion */
void LTC681x_adcvsc(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits | 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits | 0x60 | (DCP<<4) | 0x07;

	cmd_68(dev, cmd);
}

/* Starts cell voltage and GPIO 1&2 conversion */
void LTC681x_adcvax(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
				   )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits | 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits | (((DCP&0x01)<<4) + 0x6F);

	cmd_68(dev, cmd);
}

/*
Reads and parses the LTC681x cell voltage registers.
The function is used to read the parsed Cell voltages codes of the LTC681x.
This function will send the requested read commands parse the data
and store the cell voltages in c_codes variable.
*/
uint8_t LTC681x_rdcv(ltc681x_driver_t *dev,
					 uint8_t reg //!< Controls which cell voltage register is read back.
                    )
{
	int8_t pec_error = 0;
	uint8_t *cell_data;
	uint8_t c_ic = 0;
	cell_data = (uint8_t *) malloc((NUM_RX_BYT*dev->num_ics)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t cell_reg = 1; cell_reg<dev->ic_arr[0].ic_reg.num_cv_reg+1; cell_reg++) //Executes once for each of the LTC681x cell voltage registers
		{
			LTC681x_rdcv_reg(dev, cell_reg,cell_data );
			for (int current_ic = 0; current_ic<dev->num_ics; current_ic++)
			{
				if (dev->ic_arr->isospi_reverse == 0) c_ic = current_ic;
				else c_ic = dev->num_ics - current_ic - 1;

				pec_error = pec_error + parse_cells(current_ic,cell_reg, cell_data,
													&dev->ic_arr[c_ic].cells.c_codes[0],
													&dev->ic_arr[c_ic].cells.pec_match[0]);
			}
		}
	}
	else
	{
		LTC681x_rdcv_reg(dev, reg, cell_data);

		for (int current_ic = 0; current_ic<dev->num_ics; current_ic++)
		{
			if (dev->ic_arr->isospi_reverse == 0) c_ic = current_ic;
			else c_ic = dev->num_ics - current_ic - 1;
			pec_error = pec_error + parse_cells(current_ic,reg, &cell_data[8*c_ic],
											  &dev->ic_arr[c_ic].cells.c_codes[0],
											  &dev->ic_arr[c_ic].cells.pec_match[0]);
		}
	}
	LTC681x_check_pec(dev, CELL);
	free(cell_data);

	return(pec_error);
}

/*
The function is used to read the  parsed GPIO codes of the LTC681x.
This function will send the requested read commands parse the data
and store the gpio voltages in a_codes variable.
*/
int8_t LTC681x_rdaux(ltc681x_driver_t *dev,
					 uint8_t reg //!< Determines which GPIO voltage register is read back.
                    )
{
	uint8_t *data;
	int8_t pec_error = 0;
	uint8_t c_ic =0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;
	data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t gpio_reg = 1; gpio_reg<ic[0].ic_reg.num_gpio_reg+1; gpio_reg++) //Executes once for each of the LTC681x aux voltage registers
		{
			LTC681x_rdaux_reg(dev, gpio_reg, data);                 //Reads the raw auxiliary register data into the data[] array
			for (int current_ic = 0; current_ic<total_ic; current_ic++)
			{
				if (ic->isospi_reverse == 0)
				{
				  c_ic = current_ic;
				}
				else
				{
				  c_ic = total_ic - current_ic - 1;
				}
				pec_error = parse_cells(current_ic,gpio_reg, data,
										&ic[c_ic].aux.a_codes[0],
										&ic[c_ic].aux.pec_match[0]);
			}
		}
	}
	else
	{
		LTC681x_rdaux_reg(dev, reg, data);

		for (int current_ic = 0; current_ic<total_ic; current_ic++)
		{
			if (ic->isospi_reverse == 0)
			{
			c_ic = current_ic;
			}
			else
			{
			c_ic = total_ic - current_ic - 1;
			}
			pec_error = parse_cells(current_ic,reg, data,
								  &ic[c_ic].aux.a_codes[0],
								  &ic[c_ic].aux.pec_match[0]);
		}
	}
	LTC681x_check_pec(dev, AUX);
	free(data);

	return (pec_error);
}

/*
Reads and parses the LTC681x stat registers.
The function is used to read the  parsed Stat codes of the LTC681x.
This function will send the requested read commands parse the data
and store the gpio voltages in stat_codes variable.
*/
int8_t LTC681x_rdstat(ltc681x_driver_t *dev,
					  uint8_t reg //!< Determines which Stat  register is read back.
                     )

{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t STAT_IN_REG = 3;
	uint8_t *data;
	uint8_t data_counter = 0;
	int8_t pec_error = 0;
	uint16_t parsed_stat;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t c_ic = 0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;

	data = (uint8_t *) malloc((12*total_ic)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t stat_reg = 1; stat_reg< 3; stat_reg++)                      //Executes once for each of the LTC681x stat voltage registers
		{
			data_counter = 0;
			LTC681x_rdstat_reg(dev, stat_reg, data);                            //Reads the raw status register data into the data[] array

			for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++)      // Executes for every LTC681x in the daisy chain
			{																		// current_ic is used as the IC counter
				if (ic->isospi_reverse == 0)
				{
					c_ic = current_ic;
				}
				else
				{
					c_ic = total_ic - current_ic - 1;
				}

				if (stat_reg ==1)
				{
					for (uint8_t current_stat = 0; current_stat< STAT_IN_REG; current_stat++) // This loop parses the read back data into Status registers,
					{																		 // it loops once for each of the 3 stat codes in the register
						parsed_stat = data[data_counter] + (data[data_counter+1]<<8);       //Each stat codes is received as two bytes and is combined to create the parsed status code
						ic[c_ic].stat.stat_codes[current_stat] = parsed_stat;
						data_counter=data_counter+2;                                       //Because stat codes are two bytes the data counter
					}
				}
				else if (stat_reg == 2)
				{
					parsed_stat = data[data_counter] + (data[data_counter+1]<<8);          //Each stat is received as two bytes and is combined to create the parsed status code
					data_counter = data_counter +2;
					ic[c_ic].stat.stat_codes[3] = parsed_stat;
					ic[c_ic].stat.flags[0] = data[data_counter++];
					ic[c_ic].stat.flags[1] = data[data_counter++];
					ic[c_ic].stat.flags[2] = data[data_counter++];
					ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
					ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
				}

				received_pec = (data[data_counter]<<8)+ data[data_counter+1];        //The received PEC for the current_ic is transmitted as the 7th and 8th
																					//after the 6 status data bytes
				data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);

				if (received_pec != data_pec)
				{
					pec_error = -1;                         //The pec_error variable is simply set negative if any PEC errors
					ic[c_ic].stat.pec_match[stat_reg-1]=1;  //are detected in the received serial data

				}
				else
				{
					ic[c_ic].stat.pec_match[stat_reg-1]=0;
				}

				data_counter=data_counter+2;    //Because the transmitted PEC code is 2 bytes long the data_counter
											//must be incremented by 2 bytes to point to the next ICs status data
			}
		}
	}
	else
	{
		LTC681x_rdstat_reg(dev, reg, data);
		for (int current_ic = 0 ; current_ic < total_ic; current_ic++)            // Executes for every LTC681x in the daisy chain
		{																		  // current_ic is used as an IC counter
			if (ic->isospi_reverse == 0)
			{
			c_ic = current_ic;
			}
			else
			{
			c_ic = total_ic - current_ic - 1;
			}
			if (reg ==1)
			{
				for (uint8_t current_stat = 0; current_stat< STAT_IN_REG; current_stat++) // This loop parses the read back data into Status voltages, it
				{																		  // loops once for each of the 3 stat codes in the register

					parsed_stat = data[data_counter] + (data[data_counter+1]<<8);           //Each stat codes is received as two bytes and is combined to
																							// create the parsed stat code

					ic[c_ic].stat.stat_codes[current_stat] = parsed_stat;
					data_counter=data_counter+2;                     //Because stat codes are two bytes the data counter
																	//must increment by two for each parsed stat code
				}
			}
			else if (reg == 2)
			{
				parsed_stat = data[data_counter] + ((data[data_counter + 1]) <<8); //Each stat codes is received as two bytes and is combined to
				data_counter += 2;
				ic[c_ic].stat.stat_codes[3] = parsed_stat;
				ic[c_ic].stat.flags[0] = data[data_counter++];
				ic[c_ic].stat.flags[1] = data[data_counter++];
				ic[c_ic].stat.flags[2] = data[data_counter++];
				ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
				ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
			}

			received_pec = (data[data_counter]<<8)+ data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																		  //after the 6 status data bytes
			data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
			if (received_pec != data_pec)
			{
				pec_error = -1;                  //The pec_error variable is simply set negative if any PEC errors
				ic[c_ic].stat.pec_match[reg-1]=1;
			}

			data_counter=data_counter+2;
		}
	}
	LTC681x_check_pec(dev, STAT);

	free(data);

	return (pec_error);
}

/* Writes the command and reads the raw cell voltage register data */
void LTC681x_rdcv_reg(ltc681x_driver_t *dev, // device driver
					  uint8_t reg, //Determines which cell voltage register is read back
                      uint8_t *data //An array of the unparsed cell codes
                     )
{
	const uint8_t REG_LEN = 8; //Number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t total_ic = dev->num_ics;

	if (reg == 1)     //1: RDCVA
	{
		cmd[1] = 0x04;
		cmd[0] = 0x00;
	}
	else if (reg == 2) //2: RDCVB
	{
		cmd[1] = 0x06;
		cmd[0] = 0x00;
	}
	else if (reg == 3) //3: RDCVC
	{
		cmd[1] = 0x08;
		cmd[0] = 0x00;
	}
	else if (reg == 4) //4: RDCVD
	{
		cmd[1] = 0x0A;
		cmd[0] = 0x00;
	}
	else if (reg == 5) //4: RDCVE
	{
		cmd[1] = 0x09;
		cmd[0] = 0x00;
	}
	else if (reg == 6) //4: RDCVF
	{
		cmd[1] = 0x0B;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	spi_write_read(dev, cmd,4,data,(REG_LEN*total_ic), 1);

}

/*
The function reads a single GPIO voltage register and stores the read data
in the *data point as a byte array. This function is rarely used outside of
the LTC681x_rdaux() command.
*/
void LTC681x_rdaux_reg(ltc681x_driver_t *dev, // device driver
					   uint8_t reg, //Determines which GPIO voltage register is read back
                       uint8_t *data //Array of the unparsed auxiliary codes
                      )
{
	const uint8_t REG_LEN = 8; // Number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t total_ic = dev->num_ics;

	if (reg == 1)     //Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}
	else if (reg == 2)  //Read back auxiliary group B
	{
		cmd[1] = 0x0E;
		cmd[0] = 0x00;
	}
	else if (reg == 3)  //Read back auxiliary group C
	{
		cmd[1] = 0x0D;
		cmd[0] = 0x00;
	}
	else if (reg == 4)  //Read back auxiliary group D
	{
		cmd[1] = 0x0F;
		cmd[0] = 0x00;
	}
	else          //Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	spi_write_read(dev, cmd,4,data,(REG_LEN*total_ic), 1);

}

/*
The function reads a single stat  register and stores the read data
in the *data point as a byte array. This function is rarely used outside of
the LTC681x_rdstat() command.
*/
void LTC681x_rdstat_reg(ltc681x_driver_t *dev, // device driver
					    uint8_t reg, //Determines which stat register is read back
                        uint8_t *data //Array of the unparsed stat codes
                       )
{
	const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t total_ic = dev->num_ics;

	if (reg == 1)     //Read back status group A
	{
		cmd[1] = 0x10;
		cmd[0] = 0x00;
	}
	else if (reg == 2)  //Read back status group B
	{
		cmd[1] = 0x12;
		cmd[0] = 0x00;
	}

	else          //Read back status group A
	{
		cmd[1] = 0x10;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	spi_write_read(dev, cmd,4,data,(REG_LEN*total_ic), 1);
}

/* Helper function that parses voltage measurement registers */
int8_t parse_cells(uint8_t current_ic, // Current IC
				   uint8_t cell_reg,  // Type of register
				   uint8_t cell_data[], // Unparsed data
				   uint16_t *cell_codes, // Parsed data
				   uint8_t *ic_pec // PEC error
				  )
{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t CELL_IN_REG = 3;
	int8_t pec_error = 0;
	uint16_t parsed_cell;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t data_counter = current_ic*NUM_RX_BYT; //data counter

	for (uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++) // This loop parses the read back data into the register codes, it
	{																		// loops once for each of the 3 codes in the register

		parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each code is received as two bytes and is combined to
																				   // create the parsed code
		cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;

		data_counter = data_counter + 2;                       //Because the codes are two bytes, the data counter
															  //must increment by two for each parsed code
	}
	received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																			   //after the 6 cell voltage data bytes
	data_pec = pec15_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);

	if (received_pec != data_pec)
	{
		pec_error = 1;                             //The pec_error variable is simply set negative if any PEC errors
		ic_pec[cell_reg-1]=1;
	}
	else
	{
		ic_pec[cell_reg-1]=0;
	}
	data_counter=data_counter+2;

	return(pec_error);
}

/* Sends the poll ADC command */
uint8_t LTC681x_pladc(ltc681x_driver_t *dev)
{
	uint8_t cmd[4];
	uint8_t adc_state = 0xFF;
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x14;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	LTC681x_set_cs(dev, 0);
	spi_write_array(dev, 4, cmd, 0);
	adc_state = spi_read_byte(dev, 0xFF, 0);
	LTC681x_set_cs(dev, 1);

	return(adc_state);
}

/* This function will block operation until the ADC has finished it's conversion */
uint32_t LTC681x_pollAdc(ltc681x_driver_t *dev)
{
	uint32_t counter = 0;
	uint8_t finished = 0;
	uint8_t current_time = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x14;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	spi_write_array(dev, 4, cmd, 1);
	while ((counter<200000)&&(finished == 0))
	{
		current_time = spi_read_byte(dev, 0xff, 1);
		if (current_time > 0) finished = 1;
		else counter = counter + 10;
	}

	return(counter);
}

/*
The command clears the cell voltage registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clrcell(ltc681x_driver_t *dev) // device driver)
{
	uint8_t cmd[2]= {0x07 , 0x11};
	cmd_68(dev, cmd);
}

/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clraux(ltc681x_driver_t *dev) // device driver)
{
	uint8_t cmd[2]= {0x07 , 0x12};
	cmd_68(dev, cmd);
}

/*
The command clears the Stat registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.

*/
void LTC681x_clrstat(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x07 , 0x13};
	cmd_68(dev, cmd);
}

/* Starts the Mux Decoder diagnostic self test */
void LTC681x_diagn(ltc681x_driver_t *dev)
{
	uint8_t cmd[2] = {0x07 , 0x15};
	cmd_68(dev, cmd);
}

/* Starts cell voltage self test conversion */
void LTC681x_cvst(ltc681x_driver_t *dev,
				  uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
                 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST)<<5) +0x07;

	cmd_68(dev,cmd);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC681x_axst(ltc681x_driver_t *dev,
				  uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST&0x03)<<5) +0x07;

	cmd_68(dev,cmd);
}

/* Start a Status Register Self Test Conversion */
void LTC681x_statst(ltc681x_driver_t *dev,
					uint8_t MD, //ADC Mode
				    uint8_t ST //Self Test
				   )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST&0x03)<<5) +0x0F;

	cmd_68(dev, cmd);
}

/* Starts cell voltage overlap conversion */
void LTC681x_adol(ltc681x_driver_t *dev,
				  uint8_t MD, //ADC Mode
                  uint8_t DCP //Discharge Permit
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + (DCP<<4) +0x01;

	cmd_68(dev, cmd);
}

/* Start an GPIO Redundancy test */
void LTC681x_adaxd(ltc681x_driver_t *dev,
				   uint8_t MD, //ADC Mode
				   uint8_t CHG //GPIO Channels to be measured
				  )
{
	uint8_t cmd[4];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + CHG ;

	cmd_68(dev, cmd);
}

/* Start a Status register redundancy test Conversion */
void LTC681x_adstatd(ltc681x_driver_t *dev,
					 uint8_t MD, //ADC Mode
					 uint8_t CHST //Stat Channels to be measured
					)
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x08 + CHST ;

	cmd_68(dev,cmd);
}

/* Runs the Digital Filter Self Test */
int16_t LTC681x_run_cell_adc_st(ltc681x_driver_t *dev,
							    uint8_t adc_reg, //!< Type of register
								uint8_t md, //!< ADC Mode
								uint8_t adcopt //!< ADCOPT bit in the configuration register
							   )
{
	int16_t error = 0;
	uint16_t expected_result = 0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;

	for (int self_test = 1; self_test<3; self_test++)
	{
		expected_result = LTC681x_st_lookup(md,self_test,adcopt);
		wakeup_idle(dev);

		switch (adc_reg)
		{
			case CELL:
			  wakeup_idle(dev);
			  LTC681x_clrcell(dev);
			  LTC681x_cvst(dev, md,self_test);
			  LTC681x_pollAdc(dev);

			  wakeup_idle(dev);
			  error = LTC681x_rdcv(dev, 0);
			  for (int cic = 0; cic < total_ic; cic++)
				{
				  for (int channel=0; channel< ic[cic].ic_reg.cell_channels; channel++)
				  {

					if (ic[cic].cells.c_codes[channel] != expected_result)
					{
					  error = error+1;
					}
				  }
				}
			break;
		  case AUX:
			  error = 0;
			  wakeup_idle(dev);
			  LTC681x_clraux(dev);
			  LTC681x_axst(dev, md, self_test);
			  LTC681x_pollAdc(dev);

			  wakeup_idle(dev);
			  LTC681x_rdaux(dev, 0);
			  for (int cic = 0; cic < total_ic; cic++)
				{
				  for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
				  {

					if (ic[cic].aux.a_codes[channel] != expected_result)
					{
					  error = error+1;
					}
				  }
				}
			break;
		  case STAT:
			wakeup_idle(dev);
			LTC681x_clrstat(dev);
			LTC681x_statst(dev, md,self_test);
			LTC681x_pollAdc(dev);

			wakeup_idle(dev);
			error = LTC681x_rdstat(dev, 0);
			for (int cic = 0; cic < total_ic; cic++)
			{
			  for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
			  {
				if (ic[cic].stat.stat_codes[channel] != expected_result)
				{
				  error = error+1;
				}
			  }
			}
			break;

		  default:
			error = -1;
			break;
		}
	}

	return(error);
}

/* Runs the ADC overlap test for the IC */
uint16_t LTC681x_run_adc_overlap(ltc681x_driver_t *dev)
{
	uint16_t error = 0;
	int32_t measure_delta =0;
	int16_t failure_pos_limit = 20;
	int16_t failure_neg_limit = -20;
	//int32_t a, b;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;

	wakeup_idle(dev);
	LTC681x_adol(dev, MD_7KHZ_3KHZ,DCP_DISABLED);
	LTC681x_pollAdc(dev);
	wakeup_idle(dev);
	error = LTC681x_rdcv(dev, 0);

	  for (int cic = 0; cic<total_ic; cic++)
	  {
		  measure_delta = (int32_t)ic[cic].cells.c_codes[6]-(int32_t)ic[cic].cells.c_codes[7];

		 if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		 {
		  error = error | (1<<(cic-1));
		 }
	  }

    return(error);
}

/* Runs the redundancy self test */
int16_t LTC681x_run_adc_redundancy_st(ltc681x_driver_t *dev,
									  uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg //!< Type of register
									 )
{
	int16_t error = 0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;
	for (int self_test = 1; self_test<3; self_test++)
	{
		wakeup_idle(dev);
		switch (adc_reg)
		{
			case AUX:
				LTC681x_clraux(dev);
				LTC681x_adaxd(dev, adc_mode,AUX_CH_ALL);
				LTC681x_pollAdc(dev);

				wakeup_idle(dev);
				error = LTC681x_rdaux(dev, 0);
				for (int cic = 0; cic < total_ic; cic++)
				{
					for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
					{
						if (ic[cic].aux.a_codes[channel] >= 65280) error++;
					}
				}
				break;
			case STAT:
				LTC681x_clrstat(dev);
				LTC681x_adstatd(dev, adc_mode,STAT_CH_ALL);
				LTC681x_pollAdc(dev);
				wakeup_idle(dev);
				error = LTC681x_rdstat(dev, 0);
				for (int cic = 0; cic < total_ic; cic++)
				{
					for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
					{
						if (ic[cic].stat.stat_codes[channel] >= 65280) error++;
					}
				}
				break;

			default:
			error = -1;
			break;
		}
	}
	return(error);
}

/* Looks up the result pattern for digital filter self test */
uint16_t LTC681x_st_lookup(uint8_t MD, //ADC Mode
						   uint8_t ST, //Self Test
						   uint8_t adcopt // ADCOPT bit in the configuration register
						  )
{
	uint16_t test_pattern = 0;

    if (MD == 1)
    {
		if ( adcopt == 0)
		{
			if (ST == 1)
			{
				test_pattern = 0x9565;
			}
			else
			{
				test_pattern = 0x6A9A;
			}
		}
		else
		{
			if (ST == 1)
			{
				test_pattern = 0x9553;
			}
			else
			{
				test_pattern = 0x6AAC;
			}
		}
    }
    else
    {
		if (ST == 1)
		{
		   test_pattern = 0x9555;
		}
		else
		{
		   test_pattern = 0x6AAA;
		}
    }
    return(test_pattern);
}

/* Start an open wire Conversion */
void LTC681x_adow(ltc681x_driver_t *dev,
				  uint8_t MD, //ADC Mode
				  uint8_t PUP,//Pull up/Pull down current
				  uint8_t CH, //Channels
				  uint8_t DCP//Discharge Permit
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x28 + (PUP<<6) + CH+(DCP<<4);

	cmd_68(dev, cmd);
}

/* Start GPIOs open wire ADC conversion */
void LTC681x_axow(ltc681x_driver_t *dev,
				uint8_t MD, //ADC Mode
				  uint8_t PUP //Pull up/Pull down current
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x10+ (PUP<<6) ;//+ CH;

	cmd_68(dev, cmd);
}

/* Runs the data sheet algorithm for open wire for single cell detection */
void LTC681x_run_openwire_single(ltc681x_driver_t *dev)
{
	uint16_t OPENWIRE_THRESHOLD = 4000;
	const uint8_t  N_CHANNELS = dev->ic_arr[0].ic_reg.cell_channels;

	uint16_t pullUp[dev->num_ics][N_CHANNELS];
	uint16_t pullDwn[dev->num_ics][N_CHANNELS];
	int16_t openWire_delta[dev->num_ics][N_CHANNELS];

	int8_t error;
	int8_t i;
	uint32_t conv_time = 0;

	wakeup_sleep(dev);
	LTC681x_clrcell(dev);

	// Pull Ups
	for (i = 0; i < 3; i++)
	{
	  wakeup_idle(dev);
	  LTC681x_adow(dev, MD_26HZ_2KHZ,PULL_UP_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error=LTC681x_rdcv(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		  pullUp[cic][cell] = dev->ic_arr[cic].cells.c_codes[cell];
		}
	}

	// Pull Downs
	for (i = 0; i < 3; i++)
	{
	  wakeup_idle(dev);
	  LTC681x_adow(dev, MD_26HZ_2KHZ,PULL_DOWN_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error=LTC681x_rdcv(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		   pullDwn[cic][cell] = dev->ic_arr[cic].cells.c_codes[cell];
		}
	}

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		dev->ic_arr[cic].system_open_wire = 0xFFFF;

		for (int cell=0; cell<N_CHANNELS; cell++)
		{
			if (pullDwn[cic][cell] < pullUp[cic][cell])
			{
				openWire_delta[cic][cell] = (pullUp[cic][cell] - pullDwn[cic][cell]);
			}
			else
			{
				openWire_delta[cic][cell] = 0;
			}

			if (openWire_delta[cic][cell]>OPENWIRE_THRESHOLD)
			{
				dev->ic_arr[cic].system_open_wire = cell+1;
			}
		}

		if (pullUp[cic][0] == 0)
		{
		  dev->ic_arr[cic].system_open_wire = 0;
		}

		if (pullUp[cic][(N_CHANNELS-1)] == 0)//checking the Pull up value of the top measured channel
		{
		 dev->ic_arr[cic].system_open_wire = N_CHANNELS;
		}
	}
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
void LTC681x_run_openwire_multi(ltc681x_driver_t *dev)
{
	uint16_t OPENWIRE_THRESHOLD = 4000;
	const uint8_t  N_CHANNELS = dev->ic_arr[0].ic_reg.cell_channels;

	uint16_t pullUp[dev->num_ics][N_CHANNELS];
	uint16_t pullDwn[dev->num_ics][N_CHANNELS];
	uint16_t openWire_delta[dev->num_ics][N_CHANNELS];

	int8_t error;
	int8_t opencells[N_CHANNELS];
	int8_t n=0;
	int8_t i,j,k;
	uint32_t conv_time = 0;

	wakeup_sleep(dev);
	LTC681x_clrcell(dev);

	// Pull Ups
	for (i = 0; i < 5; i++)
	{
		wakeup_idle(dev);
		LTC681x_adow(dev, MD_26HZ_2KHZ,PULL_UP_CURRENT,CELL_CH_ALL,DCP_DISABLED);
		conv_time =LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error = LTC681x_rdcv(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		  pullUp[cic][cell] = dev->ic_arr[cic].cells.c_codes[cell];
		}
	}

	// Pull Downs
	for (i = 0; i < 5; i++)
	{
	  wakeup_idle(dev);
	  LTC681x_adow(dev, MD_26HZ_2KHZ,PULL_DOWN_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time = LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error = LTC681x_rdcv(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
		   pullDwn[cic][cell] = dev->ic_arr[cic].cells.c_codes[cell];
		}
	}

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
			if (pullDwn[cic][cell] < pullUp[cic][cell])
				{
					openWire_delta[cic][cell] = (pullUp[cic][cell] - pullDwn[cic][cell]);
				}
				else
				{
					openWire_delta[cic][cell] = 0;
				}
		}
	}

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		n=0;
		for (int cell=0; cell<N_CHANNELS; cell++)
		{

		  if (openWire_delta[cic][cell]>OPENWIRE_THRESHOLD)
			{
				opencells[n] = cell+1;
				n++;
				for (int j = cell; j < N_CHANNELS-3 ; j++)
				{
					if (pullUp[cic][j + 2] == 0)
					{
					opencells[n] = j+2;
					n++;
					}
				}
				if((cell==N_CHANNELS-4) && (pullDwn[cic][N_CHANNELS-3] == 0))
				{
					  opencells[n] = N_CHANNELS-2;
					  n++;
				}
			}
		}
		if (pullDwn[cic][0] == 0)
		{
		  opencells[n] = 0;
		  n++;
		}

		if (pullDwn[cic][N_CHANNELS-1] == 0)
		{
		  opencells[n] = N_CHANNELS;
		  n++;
		}

		if (pullDwn[cic][N_CHANNELS-2] == 0)
		{
		  opencells[n] = N_CHANNELS-1;
		  n++;
		}

	//Removing repetitive elements
		for(i=0;i<n;i++)
		{
			for(j=i+1;j<n;)
			{
				if(opencells[i]==opencells[j])
				{
					for(k=j;k<n;k++)
						opencells[k]=opencells[k+1];

					n--;
				}
				else
					j++;
			}
		}

	// Sorting open cell array
		for(int i=0; i<n; i++)
		{
			for(int j=0; j<n-1; j++)
			{
				if( opencells[j] > opencells[j+1] )
				{
					k = opencells[j];
					opencells[j] = opencells[j+1];
					opencells[j+1] = k;
				}
			}
		}
	}
}

/* Runs open wire for GPIOs */
void LTC681x_run_gpio_openwire(ltc681x_driver_t *dev)
{
	uint16_t OPENWIRE_THRESHOLD = 150;
	const uint8_t  N_CHANNELS = dev->ic_arr[0].ic_reg.aux_channels +1;

	uint16_t aux_val[dev->num_ics][N_CHANNELS];
	uint16_t pDwn[dev->num_ics][N_CHANNELS];
	uint16_t ow_delta[dev->num_ics][N_CHANNELS];

	int8_t error;
	int8_t i;
	uint32_t conv_time=0;

	wakeup_sleep(dev);
	LTC681x_clraux(dev);

	for (i = 0; i < 3; i++)
	{
	   wakeup_idle(dev);
	   LTC681x_adax(dev, MD_7KHZ_3KHZ, AUX_CH_ALL);
	   conv_time= LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error = LTC681x_rdaux(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		for (int channel=0; channel<N_CHANNELS; channel++)
		{
			aux_val[cic][channel]=dev->ic_arr[cic].aux.a_codes[channel];
		}
	}
	LTC681x_clraux(dev);

	// pull downs
	for (i = 0; i < 3; i++)
	{
	   wakeup_idle(dev);
	   LTC681x_axow(dev, MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
	   conv_time =LTC681x_pollAdc(dev);
	}

	wakeup_idle(dev);
	error = LTC681x_rdaux(dev, 0);

	for (int cic=0; cic<dev->num_ics; cic++)
	{
	   for (int channel=0; channel<N_CHANNELS; channel++)
		{
			pDwn[cic][channel]=dev->ic_arr[cic].aux.a_codes[channel] ;
		}
	}

	for (int cic=0; cic<dev->num_ics; cic++)
	{
		dev->ic_arr[cic].system_open_wire = 0xFFFF;

		for (int channel=0; channel<N_CHANNELS; channel++)
		{
			if (pDwn[cic][channel] > aux_val[cic][channel])
			{
				ow_delta[cic][channel] = (pDwn[cic][channel] - aux_val[cic][channel]);
			}
			else
			{
				ow_delta[cic][channel] = 0;
			}

			if(channel<5)
			{
				if (ow_delta[cic][channel] > OPENWIRE_THRESHOLD)
				{
					dev->ic_arr[cic].system_open_wire= channel+1;

				}
			}
			else if(channel>5)
			{
				if (ow_delta[cic][channel] > OPENWIRE_THRESHOLD)
				{
					dev->ic_arr[cic].system_open_wire= channel;

				}
			}
		}
	}
}

/* Clears all of the DCC bits in the configuration registers */
void LTC681x_clear_discharge(ltc681x_driver_t *dev)
{
	for (int i=0; i<dev->num_ics; i++)
	{
	   dev->ic_arr[i].config.tx_data[4] = 0;
	   dev->ic_arr[i].config.tx_data[5] = dev->ic_arr[i].config.tx_data[5]&(0xF0);
	   dev->ic_arr[i].configb.tx_data[0]= dev->ic_arr[i].configb.tx_data[0]&(0x0F);
	   dev->ic_arr[i].configb.tx_data[1]= dev->ic_arr[i].configb.tx_data[1]&(0xF0);
	}
}

/* Writes the pwm register */
void LTC681x_wrpwm(ltc681x_driver_t *dev,
                   uint8_t pwmReg  //!< The PWM Register to be written
                  )
{
	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	if (pwmReg == 0)
	{
		cmd[0] = 0x00;
		cmd[1] = 0x20;
	}
	else
	{
		cmd[0] = 0x00;
		cmd[1] = 0x1C;
	}

	for (uint8_t current_ic = 0; current_ic<dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = dev->ic_arr[c_ic].pwm.tx_data[data];
			write_count++;
		}
	}
	write_68(dev, cmd, write_buffer);
}


/* Reads pwm registers of a LTC681x daisy chain */
int8_t LTC681x_rdpwm(ltc681x_driver_t *dev,
                     uint8_t pwmReg //!< The PWM Register to be written A or B
                    )
{
	//const uint8_t BYTES_IN_REG = 8;
	uint8_t cmd[4];
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;

	if (pwmReg == 0)
	{
		cmd[0] = 0x00;
		cmd[1] = 0x22;
	}
	else
	{
		cmd[0] = 0x00;
		cmd[1] = 0x1E;
	}

	pec_error = read_68(dev, cmd, read_buffer);
	for (uint8_t current_ic =0; current_ic<dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (int byte=0; byte<8; byte++)
		{
			dev->ic_arr[c_ic].pwm.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}

		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec ) dev->ic_arr[c_ic].pwm.rx_pec_match = 1;
		else dev->ic_arr[c_ic].pwm.rx_pec_match = 0;
	}
	return(pec_error);
}

/*  Write the LTC681x Sctrl register */
void LTC681x_wrsctrl(ltc681x_driver_t *dev,
                     uint8_t sctrl_reg //!< The Sctrl Register to be written A or B
                    )
{
	uint8_t cmd[2];
    uint8_t write_buffer[256];
    uint8_t write_count = 0;
    uint8_t c_ic = 0;
    if (sctrl_reg == 0)
    {
      cmd[0] = 0x00;
      cmd[1] = 0x14;
    }
    else
    {
      cmd[0] = 0x00;
      cmd[1] = 0x1C;
    }

    for(uint8_t current_ic = 0; current_ic<dev->num_ics;current_ic++)
    {
        if(dev->ic_arr->isospi_reverse == 0){c_ic = current_ic;}
        else{c_ic = dev->num_ics - current_ic - 1;}


        for(uint8_t data = 0; data<6;data++)
        {
            write_buffer[write_count] = dev->ic_arr[c_ic].sctrl.tx_data[data];
            write_count++;
        }
    }
    write_68(dev, cmd, write_buffer);
}

/*  Reads sctrl registers of a LTC681x daisy chain */
int8_t LTC681x_rdsctrl(ltc681x_driver_t *dev,
                       uint8_t sctrl_reg //!< The Sctrl Register to be written A or B
                      )
{
    uint8_t cmd[4];
    uint8_t read_buffer[256];
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t calc_pec;
    uint8_t c_ic = 0;

    if (sctrl_reg == 0)
    {
      cmd[0] = 0x00;
      cmd[1] = 0x16;
	  }
    else
    {
      cmd[0] = 0x00;
      cmd[1] = 0x1E;
	}

    pec_error = read_68(dev, cmd, read_buffer);

    for(uint8_t current_ic =0; current_ic<dev->num_ics; current_ic++)
    {
        if(dev->ic_arr->isospi_reverse == 0){c_ic = current_ic;}
        else{c_ic = dev->num_ics - current_ic - 1;}


        for(int byte=0; byte<8;byte++)
        {
            dev->ic_arr[c_ic].sctrl.rx_data[byte] = read_buffer[byte+(8*current_ic)];
        }

        calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
        data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
        if(calc_pec != data_pec ) dev->ic_arr[c_ic].sctrl.rx_pec_match = 1;
        else dev->ic_arr[c_ic].sctrl.rx_pec_match = 0;

    }
    return pec_error;
}

/*
Start Sctrl data communication
This command will start the sctrl pulse communication over the spins
*/
void LTC681x_stsctrl(ltc681x_driver_t *dev)
{
	uint8_t cmd[4];
    uint16_t cmd_pec;

    cmd[0] = 0x00;
    cmd[1] = 0x19;
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    spi_write_array(dev, 4, cmd, 1);
}

/*
The command clears the Sctrl registers and initializes
all values to 0. The register will read back hexadecimal 0x00
after the command is sent.
*/
void LTC681x_clrsctrl(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x00 , 0x18};
	cmd_68(dev,cmd);
}

/* Writes the comm register */
void LTC681x_wrcomm(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x07 , 0x21};
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	cell_asic *ic = dev->ic_arr;
	for (uint8_t current_ic = 0; current_ic<dev->num_ics; current_ic++)
	{
		if (ic->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].com.tx_data[data];
			write_count++;
		}
	}
	write_68(dev, cmd, write_buffer);
}

/* Reads COMM registers of a LTC681x daisy chain */
int8_t LTC681x_rdcomm(ltc681x_driver_t *dev)
{
	uint8_t cmd[2]= {0x07 , 0x22};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic=0;

	pec_error = read_68(dev, cmd, read_buffer);

	for (uint8_t current_ic = 0; current_ic<dev->num_ics; current_ic++)
	{
		if (dev->ic_arr->isospi_reverse == 0)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = dev->num_ics - current_ic - 1;
		}

		for (int byte=0; byte<8; byte++)
		{
			dev->ic_arr[c_ic].com.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}

		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec ) dev->ic_arr[c_ic].com.rx_pec_match = 1;
		else dev->ic_arr[c_ic].com.rx_pec_match = 0;
	}

    return(pec_error);
}

/* Shifts data in COMM register out over LTC681x SPI/I2C port */
void LTC681x_stcomm(ltc681x_driver_t *dev,
					uint8_t len //!< Length of data to be transmitted
					)
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x23;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	LTC681x_set_cs(dev, 0);
	spi_write_array(dev, 4, cmd, 0);
	for (int i = 0; i < len * 3; i++) spi_read_byte(dev, 0xFF, 0);
	LTC681x_set_cs(dev, 1);
}

/* Helper function that increments PEC counters */
void LTC681x_check_pec(ltc681x_driver_t *dev,
                       uint8_t reg //!< Type of register
					   )
{
	switch (reg)
	{
		case CFGRR:
			for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
			{
				dev->ic_arr[current_ic].crc_count.pec_count = dev->ic_arr[current_ic].crc_count.pec_count + dev->ic_arr[current_ic].config.rx_pec_match;
				dev->ic_arr[current_ic].crc_count.cfgr_pec = dev->ic_arr[current_ic].crc_count.cfgr_pec + dev->ic_arr[current_ic].config.rx_pec_match;
			}
			break;
		case CFGRB:
			for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
			{
				dev->ic_arr[current_ic].crc_count.pec_count = dev->ic_arr[current_ic].crc_count.pec_count + dev->ic_arr[current_ic].configb.rx_pec_match;
				dev->ic_arr[current_ic].crc_count.cfgr_pec = dev->ic_arr[current_ic].crc_count.cfgr_pec + dev->ic_arr[current_ic].configb.rx_pec_match;
			}
			break;
		case CELL:
			for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
			{
				for (int i = 0; i < dev->ic_arr[0].ic_reg.num_cv_reg; i++)
				{
					dev->ic_arr[current_ic].crc_count.pec_count = dev->ic_arr[current_ic].crc_count.pec_count + dev->ic_arr[current_ic].cells.pec_match[i];
					dev->ic_arr[current_ic].crc_count.cell_pec[i] = dev->ic_arr[current_ic].crc_count.cell_pec[i] + dev->ic_arr[current_ic].cells.pec_match[i];
				}
			}
			break;
		case AUX:
			for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
			{
				for (int i = 0; i < dev->ic_arr[0].ic_reg.num_gpio_reg; i++)
				{
					dev->ic_arr[current_ic].crc_count.pec_count = dev->ic_arr[current_ic].crc_count.pec_count + (dev->ic_arr[current_ic].aux.pec_match[i]);
					dev->ic_arr[current_ic].crc_count.aux_pec[i] = dev->ic_arr[current_ic].crc_count.aux_pec[i] + (dev->ic_arr[current_ic].aux.pec_match[i]);
				}
			}
			break;
		case STAT:
			for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
			{
				for (int i = 0; i < dev->ic_arr[0].ic_reg.num_stat_reg-1; i++)
				{
					dev->ic_arr[current_ic].crc_count.pec_count = dev->ic_arr[current_ic].crc_count.pec_count + dev->ic_arr[current_ic].stat.pec_match[i];
					dev->ic_arr[current_ic].crc_count.stat_pec[i] = dev->ic_arr[current_ic].crc_count.stat_pec[i] + dev->ic_arr[current_ic].stat.pec_match[i];
				}
			}
			break;
		default:
			break;
	}
}

/* Helper Function to reset PEC counters */
void LTC681x_reset_crc_count(ltc681x_driver_t *dev)
{
	for (int current_ic = 0 ; current_ic < dev->num_ics; current_ic++)
	{
		dev->ic_arr[current_ic].crc_count.pec_count = 0;
		dev->ic_arr[current_ic].crc_count.cfgr_pec = 0;
		for (int i=0; i<6; i++)
		{
			dev->ic_arr[current_ic].crc_count.cell_pec[i]=0;

		}
		for (int i=0; i<4; i++)
		{
			dev->ic_arr[current_ic].crc_count.aux_pec[i]=0;
		}
		for (int i=0; i<2; i++)
		{
			dev->ic_arr[current_ic].crc_count.stat_pec[i]=0;
		}
	}
}

/* Helper function to initialize CFG variables */
void LTC681x_init_cfg(ltc681x_driver_t *dev)
{
	for(uint8_t current_ic = 0; current_ic < dev->num_ics ; current_ic++)
	{
		for(int j = 0; j < 6; j++)
		{
			dev->ic_arr[current_ic].config.tx_data[j] = 0;
		}
	}
}

/* Helper function to set CFGR variable */
void LTC681x_set_cfgr(ltc681x_driver_t *dev,
					  uint8_t nIC, //!< Current IC
                      uint8_t refon,  //!< The REFON bit
					  uint8_t adcopt, //!< The ADCOPT bit
					  uint8_t gpio[5],//!< The GPIO bits
					  uint8_t dcc[12],//!< The DCC bits
					  uint8_t dcto[4],//!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t ov  //!< The OV value
					  )
{
	LTC681x_set_cfgr_refon(dev, nIC, refon);
	LTC681x_set_cfgr_adcopt(dev, nIC, adcopt);
	LTC681x_set_cfgr_gpio(dev, nIC, gpio);
	LTC681x_set_cfgr_dis(dev, nIC, dcc);
	LTC681x_set_cfgr_dcto(dev, nIC, dcto);
	LTC681x_set_cfgr_uv(dev, nIC, uv);
	LTC681x_set_cfgr_ov(dev, nIC, ov);
}

/* Helper function to set the REFON bit */
void LTC681x_set_cfgr_refon(ltc681x_driver_t *dev,
							uint8_t nIC, //!< Current IC
							uint8_t refon //!< The REFON bit
							)
{
	if (refon) dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]|0x04;
	else dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]&0xFB;
}

/* Helper function to set the ADCOPT bit */
void LTC681x_set_cfgr_adcopt(ltc681x_driver_t *dev,
							 uint8_t nIC, //!< Current IC
							 uint8_t adcopt //!< The ADCOPT bit
							 )
{
	if (adcopt) dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]|0x01;
	else dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]&0xFE;
}

/* Helper function to set GPIO bits */
void LTC681x_set_cfgr_gpio(ltc681x_driver_t *dev,
						   uint8_t nIC, //!< Current IC
						   uint8_t gpio[] //!< The GPIO bits
						   )
{
	for (int i =0; i<5; i++)
	{
		if (gpio[i])dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]|(0x01<<(i+3));
		else dev->ic_arr[nIC].config.tx_data[0] = dev->ic_arr[nIC].config.tx_data[0]&(~(0x01<<(i+3)));
	}
}

/* Helper function to control discharge */
void LTC681x_set_cfgr_dis(ltc681x_driver_t *dev,
						  uint8_t nIC, //!< Current IC
						  uint8_t dcc[] //!< The DCC bits
						  )
{
	for (int i =0; i<8; i++)
	{
		if (dcc[i])dev->ic_arr[nIC].config.tx_data[4] = dev->ic_arr[nIC].config.tx_data[4]|(0x01<<i);
		else dev->ic_arr[nIC].config.tx_data[4] = dev->ic_arr[nIC].config.tx_data[4]& (~(0x01<<i));
	}
	for (int i =0; i<4; i++)
	{
		if (dcc[i+8])dev->ic_arr[nIC].config.tx_data[5] = dev->ic_arr[nIC].config.tx_data[5]|(0x01<<i);
		else dev->ic_arr[nIC].config.tx_data[5] = dev->ic_arr[nIC].config.tx_data[5]&(~(0x01<<i));
	}
}

/* Helper function to control discharge time value */
void LTC681x_set_cfgr_dcto(ltc681x_driver_t *dev,
						   uint8_t nIC,  //!< Current IC
						   uint8_t dcto[] //!< The Dcto bits
						   )
{
	for(int i =0;i<4;i++)
	{
		if(dcto[i]) dev->ic_arr[nIC].config.tx_data[5] = dev->ic_arr[nIC].config.tx_data[5]|(0x01<<(i+4));
		else dev->ic_arr[nIC].config.tx_data[5] = dev->ic_arr[nIC].config.tx_data[5]&(~(0x01<<(i+4)));
	}
}

/* Helper Function to set UV value in CFG register */
void LTC681x_set_cfgr_uv(ltc681x_driver_t *dev,
						 uint8_t nIC, //!< Current IC
                         uint16_t uv //!< The UV value
						 )
{
	uint16_t tmp = (uv/16)-1;
	dev->ic_arr[nIC].config.tx_data[1] = 0x00FF & tmp;
	dev->ic_arr[nIC].config.tx_data[2] = dev->ic_arr[nIC].config.tx_data[2]&0xF0;
	dev->ic_arr[nIC].config.tx_data[2] = dev->ic_arr[nIC].config.tx_data[2]|((0x0F00 & tmp)>>8);
}

/* Helper function to set OV value in CFG register */
void LTC681x_set_cfgr_ov(ltc681x_driver_t *dev,
						 uint8_t nIC, //!< Current IC
                         uint16_t ov //!< The OV value
						 )
{
	uint16_t tmp = (ov/16);
	dev->ic_arr[nIC].config.tx_data[3] = 0x00FF & (tmp>>4);
	dev->ic_arr[nIC].config.tx_data[2] = dev->ic_arr[nIC].config.tx_data[2]&0x0F;
	dev->ic_arr[nIC].config.tx_data[2] = dev->ic_arr[nIC].config.tx_data[2]|((0x000F & tmp)<<4);
}

/*
Writes an array of bytes out of the SPI port
*/
void spi_write_array(ltc681x_driver_t *dev, // Device pointer
					 uint8_t len, // Option: Number of bytes to be written on the SPI port
                     uint8_t data[], //Array of bytes to be written on the SPI port
					 uint8_t use_cs
					)
{
	if(use_cs) LTC681x_set_cs(dev, 0);
	HAL_SPI_Transmit(dev->hspi[dev->string], data, len, 100);
	if(use_cs) LTC681x_set_cs(dev, 1);
}

/*
 Writes and read a set number of bytes using the SPI port.

*/

void spi_write_read(ltc681x_driver_t *dev,
					uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len, //Option: number of bytes to be read from the SPI port
					uint8_t use_cs
				   )
{
	HAL_StatusTypeDef ret = 0;
	if(use_cs) LTC681x_set_cs(dev, 0);
	ret |= HAL_SPI_Transmit(dev->hspi[dev->string], tx_Data, tx_len, 100);
	ret |= HAL_SPI_TransmitReceive(dev->hspi[dev->string], tx_Data, rx_data, rx_len, 100);
	if(use_cs) LTC681x_set_cs(dev, 1);
}


uint8_t spi_read_byte(ltc681x_driver_t *dev, uint8_t tx_dat, uint8_t use_cs)
{
  uint8_t data;
  uint8_t blank_data[1]={tx_dat};

  if(use_cs) LTC681x_set_cs(dev, 0);
  HAL_SPI_TransmitReceive(dev->hspi[dev->string], blank_data, &data, 1, 100);
  if(use_cs) LTC681x_set_cs(dev, 1);

  return data;
}

void LTC681x_set_cs(ltc681x_driver_t *dev, int state)
{
	HAL_GPIO_WritePin(dev->cs_port[dev->string], dev->cs_pin[dev->string], state);
	return;
}

int LTC681x_init(ltc681x_driver_t *dev,
		         SPI_HandleTypeDef *hspi_a,
				 SPI_HandleTypeDef *hspi_b,
				 GPIO_TypeDef *cs_port_a,
				 GPIO_TypeDef *cs_port_b,
				 uint16_t cs_pin_a,
				 uint16_t cs_pin_b,
				 uint8_t num_ics,
				 cell_asic *ic_arr
				)
{
	int ret = 0;
	dev->string = LTC681X_STR_A;
	dev->hspi[0] = hspi_a;
	dev->hspi[1] = hspi_b;
	dev->cs_port[0] = cs_port_a;
	dev->cs_port[1] = cs_port_b;
	dev->cs_pin[0] = cs_pin_a;
	dev->cs_pin[1] = cs_pin_b;
	dev->num_ics = num_ics;
	dev->ic_arr = ic_arr;
	HAL_GPIO_WritePin(cs_port_a, cs_pin_a, 1);
	HAL_GPIO_WritePin(cs_port_b, cs_pin_b, 1);

	// TODO: delete test
	//uint8_t d = 104;
	//spi_write_array(dev, 1, &d);
	// TODO: determine if other config is needed
	return ret;
}
