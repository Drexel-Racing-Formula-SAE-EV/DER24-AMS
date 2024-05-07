/*! LTC6813: Multicell Battery Monitors
*
*@verbatim
*The LTC6813 is multi-cell battery stack monitor that measures up to 18 series
*connected battery cells with a total measurement error of less than 2.2mV.
*The cell measurement range of 0V to 5V makes the LTC6813 suitable for most
*battery chemistries. All 18 cell voltages can be captured in 290uS, and lower
*data acquisition rates can be selected for high noise reduction.
*Using the LTC6813-1, multiple devices are connected in a daisy-chain with one
*host processor connection for all devices, permitting simultaneous cell monitoring
*of long, high voltage battery strings.
*@endverbatim
*
* https://www.analog.com/en/products/ltc6813-1.html
* https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc2350a-b.html
*
*********************************************************************************
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

//! @ingroup BMS
//! @{
//! @defgroup LTC6813-1 LTC6813-1: Multicell Battery Monitor
//! @}

/*! @file
    @ingroup LTC6813-1
    Library for LTC6813-1 Multicell Battery Monitor
*/

#include "stdint.h"
#include "ext_drivers/LTC6813.h"



/* Helper function to initialize register limits. */
void LTC6813_init_reg_limits(ltc6813_driver_t *dev)
{
	cell_asic *ic = dev->ic_arr;
    for(uint8_t cic=0; cic<dev->num_ics; cic++)
    {
        ic[cic].ic_reg.cell_channels=18;
        ic[cic].ic_reg.stat_channels=4;
        ic[cic].ic_reg.aux_channels=9;
        ic[cic].ic_reg.num_cv_reg=6;
        ic[cic].ic_reg.num_gpio_reg=4;
        ic[cic].ic_reg.num_stat_reg=2;
    }
}

 /*
This command will write the configuration registers of the LTC6813-1s
connected in a daisy chain stack. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6813_wrcfg(ltc6813_driver_t *dev)
{
	LTC681x_wrcfg(dev);
}

/*
This command will write the configuration b registers of the LTC6813-1s
connected in a daisy chain stack. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6813_wrcfgb(ltc6813_driver_t *dev)
{
    LTC681x_wrcfgb(dev);
}

/* Reads configuration registers of a LTC6813 daisy chain */
int8_t LTC6813_rdcfg(ltc6813_driver_t *dev)
{
	int8_t pec_error = 0;
	pec_error = LTC681x_rdcfg(dev);
	return(pec_error);
}

/* Reads configuration b registers of a LTC6813 daisy chain */
int8_t LTC6813_rdcfgb(ltc6813_driver_t *dev)
{
    int8_t pec_error = 0;
    pec_error = LTC681x_rdcfgb(dev);
    return(pec_error);
}

/* Starts cell voltage conversion */
void LTC6813_adcv(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 )
{
    LTC681x_adcv(dev, MD, DCP, CH);
}

/* Start a GPIO and Vref2 Conversion */
void LTC6813_adax(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
                  )
{
	LTC681x_adax(dev, MD, CHG);
}

/* Start a Status ADC Conversion */
void LTC6813_adstat(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t CHST //!< Sets which Stat channels are converted
					)
{
	LTC681x_adstat(dev, MD, CHST);
}

/* Starts cell voltage and GPIO 1&2 conversion */
void LTC6813_adcvax(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					)
{
    LTC681x_adcvax(dev, MD, DCP);
}

/* Starts cell voltage and SOC conversion */
void LTC6813_adcvsc(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					)
{
    LTC681x_adcvsc(dev, MD, DCP);
}

/*  Reads and parses the LTC6813 cell voltage registers */
uint8_t LTC6813_rdcv(ltc6813_driver_t *dev,
		             uint8_t reg //!< Controls which cell voltage register is read back.
                    )
{
	int8_t pec_error = 0;
	pec_error = LTC681x_rdcv(dev, reg);
	return(pec_error);
}

/*
The function is used
to read the  parsed GPIO codes of the LTC6813. This function will send the requested
read commands parse the data and store the gpio voltages in aux_codes variable
*/
int8_t LTC6813_rdaux(ltc6813_driver_t *dev,
		             uint8_t reg //!< Controls which GPIO voltage register is read back
                    )
{
	int8_t pec_error = 0;
	LTC681x_rdaux(dev, reg);
	return (pec_error);
}

/*
Reads and parses the LTC6813 stat registers.
The function is used
to read the  parsed stat codes of the LTC6813. This function will send the requested
read commands parse the data and store the stat voltages in stat_codes variable
*/
int8_t LTC6813_rdstat(ltc6813_driver_t *dev,
		              uint8_t reg //!< Determines which Stat  register is read back.
                     )
{
    int8_t pec_error = 0;
    pec_error = LTC681x_rdstat(dev, reg);
    return (pec_error);
}

/* Sends the poll ADC command */
uint8_t LTC6813_pladc(ltc6813_driver_t *dev)
{
	return(LTC681x_pladc(dev));
}

/* This function will block operation until the ADC has finished it's conversion */
uint32_t LTC6813_pollAdc(ltc6813_driver_t *dev)
{
	return(LTC681x_pollAdc(dev));
}

/*
The command clears the cell voltage registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6813_clrcell(ltc6813_driver_t *dev)
{
	LTC681x_clrcell(dev);
}

/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6813_clraux(ltc6813_driver_t *dev)
{
	LTC681x_clraux(dev);
}

/*
The command clears the Stat registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6813_clrstat(ltc6813_driver_t *dev)
{
	LTC681x_clrstat(dev);
}

/* Starts the Mux Decoder diagnostic self test */
void LTC6813_diagn(ltc6813_driver_t *dev)
{
     LTC681x_diagn(dev);
}

/* Starts cell voltage self test conversion */
void LTC6813_cvst(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Self Test Mode
				 )
{
    LTC681x_cvst(dev, MD, ST);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC6813_axst(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Sets if self test 1 or 2 is run
				 )
{
	LTC681x_axst(dev, MD, ST);
}

/* Start a Status Register Self Test Conversion */
void LTC6813_statst(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t ST //!< Sets if self test 1 or 2 is run
					)
{
    LTC681x_statst(dev, MD, ST);
}

/* Starts cell voltage overlap conversion */
void LTC6813_adol(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t DCP //!< Discharge permitted during conversion
				 )
{
	LTC681x_adol(dev, MD, DCP);
}

/* Start an GPIO Redundancy test */
void LTC6813_adaxd(ltc6813_driver_t *dev,
		           uint8_t MD, //!< ADC Conversion Mode
				   uint8_t CHG //!< Sets which GPIO channels are converted
				   )
{
	LTC681x_adaxd(dev, MD, CHG);
}

/* Start a Status register redundancy test Conversion */
void LTC6813_adstatd(ltc6813_driver_t *dev,
		             uint8_t MD, //!< ADC Mode
					 uint8_t CHST //!< Sets which Status channels are converted
					)
{
	LTC681x_adstatd(dev, MD, CHST);
}

/* Runs the Digital Filter Self Test */
int16_t LTC6813_run_cell_adc_st(ltc6813_driver_t *dev,
		                        uint8_t adc_reg, //!< Type of register
                                uint8_t md, //!< ADC Mode
								uint8_t adcopt //!< The adcopt bit in the configuration register
								)
{
	int16_t error = 0;
	error = LTC681x_run_cell_adc_st(dev, adc_reg, md, adcopt);
	return(error);
}

/*  Runs the ADC overlap test for the IC */
uint16_t LTC6813_run_adc_overlap(ltc6813_driver_t *dev)
{
	uint16_t error = 0;
	int32_t measure_delta =0;
	int16_t failure_pos_limit = 20;
	int16_t failure_neg_limit = -20;
	uint32_t conv_time=0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;
	wakeup_idle(dev);
	LTC681x_adol(dev, MD_7KHZ_3KHZ,DCP_DISABLED);
	conv_time = LTC681x_pollAdc(dev);

	wakeup_idle(dev);
	error = LTC681x_rdcv(dev, 0);
	for (int cic = 0; cic<total_ic; cic++)
	{
		measure_delta = (int32_t)ic[cic].cells.c_codes[6]-(int32_t)ic[cic].cells.c_codes[7];
		if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		{
		  error = error | (1<<(cic-1));
		}
		measure_delta = (int32_t)ic[cic].cells.c_codes[12]-(int32_t)ic[cic].cells.c_codes[13];
		if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		{
		  error = error | (1<<(cic-1));
		}
	}
	return(error);
}

/* Runs the redundancy self test */
int16_t LTC6813_run_adc_redundancy_st(ltc6813_driver_t *dev,
		                              uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg //!< Type of register
                                     )
{
	int16_t error = 0;
	LTC681x_run_adc_redundancy_st(dev, adc_mode, adc_reg);
	return(error);
}

/* Start an open wire Conversion */
void LTC6813_adow(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t PUP,//!< Pull up/Pull down current
				  uint8_t CH, //!< Sets which Cell channels are converted
				  uint8_t DCP //!< Discharge permitted during conversion
				 )
{
    LTC681x_adow(dev, MD, PUP, CH, DCP);
}

/* Start GPIOs open wire ADC conversion */
void LTC6813_axow(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Mode
				  uint8_t PUP //!< Pull up/Pull down current
				 )
{
   LTC681x_axow(dev, MD, PUP);
}

/* Runs the data sheet algorithm for open wire for single cell detection */
void LTC6813_run_openwire_single(ltc6813_driver_t *dev)
{
	LTC681x_run_openwire_single(dev);
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
void LTC6813_run_openwire_multi(ltc6813_driver_t *dev)
{
	LTC681x_run_openwire_multi(dev);
}

/* Runs open wire for GPIOs */
void LTC6813_run_gpio_openwire(ltc6813_driver_t *dev)
{
	LTC681x_run_gpio_openwire(dev);
}

/* Helper function to set discharge bit in CFG register */
void LTC6813_set_discharge(ltc6813_driver_t *dev,
		                   int Cell //!< The cell to be discharged
		                  )
{
	uint8_t total_ic = dev->num_ics;
	for(uint8_t seg = 0; seg < total_ic; seg++) LTC6813_set_discharge_per_segment(dev, Cell, seg);
}

/* Helper function to set discharge bit in CFG register */
void LTC6813_set_discharge_per_segment(ltc6813_driver_t *dev,
								       uint8_t Cell,
									   uint8_t segment
									  )
{
	cell_asic *ic = dev->ic_arr;
	if     (Cell == 0) ic[segment].configb.tx_data[1] |= (0x04);
	else if(Cell <  9) ic[segment].config.tx_data[4]  |= (1 << (Cell - 1));
	else if(Cell < 13) ic[segment].config.tx_data[5]  |= (1 << (Cell - 9));
	else if(Cell < 17) ic[segment].configb.tx_data[0] |= (1 << (Cell - 9));
	else if(Cell < 19) ic[segment].configb.tx_data[1] |= (1 << (Cell - 17));
	else return;
}


/* Clears all of the DCC bits in the configuration registers */
void LTC6813_clear_discharge(ltc6813_driver_t *dev)
{
    LTC681x_clear_discharge(dev);
}

/* Writes the pwm registers of a LTC6813 daisy chain  */
void LTC6813_wrpwm(ltc6813_driver_t *dev,
                   uint8_t pwmReg //!<  PWM  Register A or B
                  )
{
	LTC681x_wrpwm(dev, pwmReg);
}

/* Reads pwm registers of a LTC6813 daisy chain */
int8_t LTC6813_rdpwm(ltc6813_driver_t *dev,
                     uint8_t pwmReg //!< PWM  Register A or B
                    )
{
	int8_t pec_error =0;
	pec_error = LTC681x_rdpwm(dev, pwmReg);
	return(pec_error);
}

/* Writes data in S control register the ltc6813-1  connected in a daisy chain stack */
void LTC6813_wrsctrl(ltc6813_driver_t *dev,
                     uint8_t sctrl_reg //! SCTRL  Register A or B
                    )
{
	LTC681x_wrsctrl(dev, sctrl_reg);
}

/* Reads sctrl registers of a LTC6813 daisy chain */
int8_t LTC6813_rdsctrl(ltc6813_driver_t *dev,
                       uint8_t sctrl_reg//!< SCTRL Register A or B
                      )
{
	return LTC681x_rdsctrl(dev, sctrl_reg);
}

/*
Start Sctrl data communication
This command will start the sctrl pulse communication over the spins
*/
void LTC6813_stsctrl(ltc6813_driver_t *dev)
{
	LTC681x_stsctrl(dev);
}

/*
The command clears the Sctrl registers and initializes
all values to 0. The register will read back hexadecimal 0x00
after the command is sent.
*/
void LTC6813_clrsctrl(ltc6813_driver_t *dev)
{
	LTC681x_clrsctrl(dev);
}

/* Write the 6813 PWM/Sctrl Register B  */
void LTC6813_wrpsb(ltc6813_driver_t *dev)
{
	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t c_ic = 0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;

	cmd[0] = 0x00;
	cmd[1] = 0x1C;
	for(uint8_t current_ic = 0; current_ic<total_ic;current_ic++)
	{
		if(ic->isospi_reverse == 1){c_ic = current_ic;}
		else{c_ic = total_ic - current_ic - 1;}

		write_buffer[0] = ic[c_ic].pwmb.tx_data[0];
		write_buffer[1] = ic[c_ic].pwmb.tx_data[1];
		write_buffer[2]= ic[c_ic].pwmb.tx_data[2];
		write_buffer[3] = ic[c_ic].sctrlb.tx_data[3];
		write_buffer[4] = ic[c_ic].sctrlb.tx_data[4];
		write_buffer[5]= ic[c_ic].sctrlb.tx_data[5];
	}
	write_68(dev, cmd, write_buffer);
}

/* Reading the 6813 PWM/Sctrl Register B */
uint8_t LTC6813_rdpsb(ltc6813_driver_t *dev)
{
    uint8_t cmd[4];
    uint8_t read_buffer[256];
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t calc_pec;
    uint8_t c_ic = 0;
	uint8_t total_ic = dev->num_ics;
	cell_asic *ic = dev->ic_arr;
	cmd[0] = 0x00;
	cmd[1] = 0x1E;
    pec_error = read_68(dev, cmd, read_buffer);

    for(uint8_t current_ic =0; current_ic<total_ic; current_ic++)
    {
        if(ic->isospi_reverse == 1){c_ic = current_ic;}
        else{c_ic = total_ic - current_ic - 1;}
        for(int byte=0; byte<3;byte++)
        {
            ic[c_ic].pwmb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
        }

		for(int byte=3; byte<6;byte++)
        {
            ic[c_ic].sctrlb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
        }

		for(int byte=6; byte<8;byte++)
        {
			ic[c_ic].pwmb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
            ic[c_ic].sctrlb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
        }

        calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
        data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
        if(calc_pec != data_pec )
        {
            ic[c_ic].pwmb.rx_pec_match = 1;
			ic[c_ic].sctrlb.rx_pec_match = 1;

        }
        else
		{
			ic[c_ic].pwmb.rx_pec_match = 0;
			ic[c_ic].sctrlb.rx_pec_match = 0;

		}
    }
    return(pec_error);
}

/* Writes the COMM registers of a LTC6813 daisy chain */
void LTC6813_wrcomm(ltc6813_driver_t *dev)
{
	LTC681x_wrcomm(dev);
}

/* Reads COMM registers of a LTC6813 daisy chain */
int8_t LTC6813_rdcomm(ltc6813_driver_t *dev)
{
	int8_t pec_error = 0;
	LTC681x_rdcomm(dev);
	return(pec_error);
}

/* Shifts data in COMM register out over LTC6813 SPI/I2C port */
void LTC6813_stcomm(ltc6813_driver_t *dev,
		            uint8_t len //!< Length of data to be transmitted
				   )
{
    LTC681x_stcomm(dev, len);
}

/* Mutes the LTC6813 discharge transistors */
void LTC6813_mute(ltc6813_driver_t *dev)
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x28;
	cmd_68(dev, cmd);
}

/* Clears the LTC6813 Mute Discharge */
void LTC6813_unmute(ltc6813_driver_t *dev)
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x29;
	cmd_68(dev, cmd);
}

/* Helper function that increments PEC counters */
void LTC6813_check_pec(ltc6813_driver_t *dev,
                       uint8_t reg //!<  Type of register
					  )
{
	LTC681x_check_pec(dev, reg);
}

/* Helper Function to reset PEC counters */
void LTC6813_reset_crc_count(ltc6813_driver_t *dev)
{
	LTC681x_reset_crc_count(dev);
}

/* Helper function to initialize CFG variables */
void LTC6813_init_cfg(ltc6813_driver_t *dev)
{
   LTC681x_init_cfg(dev);
}

/* Helper function to set CFGR variable */
void LTC6813_set_cfgr(ltc6813_driver_t *dev,
		              uint8_t current_ic,
                      uint8_t refon, //!< The REFON bit
                      uint8_t adcopt, //!< The ADCOPT bit
                      uint8_t gpio[5], //!< The GPIO bits
                      uint8_t dcc[12], //!< The DCC bits
					  uint8_t dcto[4], //!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t  ov //!< The OV value
					  )
{
    LTC681x_set_cfgr_refon(dev, current_ic, refon);
    LTC681x_set_cfgr_adcopt(dev, current_ic, adcopt);
    LTC681x_set_cfgr_gpio(dev, current_ic, gpio);
    LTC681x_set_cfgr_dis(dev, current_ic, dcc);
	LTC681x_set_cfgr_dcto(dev, current_ic, dcto);
	LTC681x_set_cfgr_uv(dev, current_ic, uv);
    LTC681x_set_cfgr_ov(dev, current_ic, ov);
}

/* Helper function to set the REFON bit */
void LTC6813_set_cfgr_refon(ltc6813_driver_t *dev,
		                    uint8_t current_ic,
                            uint8_t refon //!< The REFON bit
							)
{
	LTC681x_set_cfgr_refon(dev, current_ic, refon);
}

/* Helper function to set the adcopt bit */
void LTC6813_set_cfgr_adcopt(ltc6813_driver_t *dev,
		                     uint8_t current_ic,
                             uint8_t adcopt //!< The ADCOPT bit
							 )
{
	LTC681x_set_cfgr_adcopt(dev, current_ic, adcopt);
}

/* Helper function to set GPIO bits */
void LTC6813_set_cfgr_gpio(ltc6813_driver_t *dev,
		                   uint8_t current_ic,
                           uint8_t gpio[] //!< The GPIO bits
						   )
{
	LTC681x_set_cfgr_gpio(dev, current_ic, gpio);
}

/* Helper function to control discharge */
void LTC6813_set_cfgr_dis(ltc6813_driver_t *dev,
                          uint8_t current_ic,
                          uint8_t dcc[] //!< The DCC bits
						  )
{
	LTC681x_set_cfgr_dis(dev, current_ic, dcc);
}

/* Helper Function to set uv value in CFG register */
void LTC6813_set_cfgr_uv(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint16_t uv //!< The UV value
						 )
{
    LTC681x_set_cfgr_uv(dev, current_ic, uv);
}

/* Helper Function to set dcto value in CFG register */
void LTC6813_set_cfgr_dcto(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint8_t dcto[4] //!< The Dcto bits
						 )
{
    LTC681x_set_cfgr_dcto(dev, current_ic, dcto);
}

/* Helper function to set OV value in CFG register */
void LTC6813_set_cfgr_ov(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint16_t ov //!< The OV value
						 )
{
    LTC681x_set_cfgr_ov(dev, current_ic, ov);
}

/* Helper Function to initialize the CFGRB data structures */
void LTC6813_init_cfgb(ltc6813_driver_t *dev)
{
	for (uint8_t current_ic = 0; current_ic<dev->num_ics;current_ic++)
    {
		for(int j =0; j<6;j++)
        {
            dev->ic_arr[current_ic].configb.tx_data[j] = 0;
        }
    }
}

/* Helper Function to set the configuration register B */
void LTC6813_set_cfgrb(ltc6813_driver_t *dev,
		              uint8_t current_ic,
					  uint8_t fdrf, //!< The FDRF bit
                      uint8_t dtmen, //!< The DTMEN bit
                      uint8_t ps[2], //!< Path selection bits
					  uint8_t gpiobits[4], //!< The GPIO bits
					  uint8_t dccbits[7] //!< The DCC bits
					  )
{
    LTC6813_set_cfgrb_fdrf(dev, current_ic, fdrf);
    LTC6813_set_cfgrb_dtmen(dev, current_ic, dtmen);
    LTC6813_set_cfgrb_ps(dev, current_ic, ps);
    LTC6813_set_cfgrb_gpio_b(dev, current_ic, gpiobits);
	LTC6813_set_cfgrb_dcc_b(dev, current_ic, dccbits);
}

/* Helper function to set the FDRF bit */
void LTC6813_set_cfgrb_fdrf(ltc6813_driver_t *dev,
							uint8_t current_ic,
							uint8_t fdrf //!< The FDRF bit
						   )
{
	cell_asic *ic = dev->ic_arr;
	if(fdrf) ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]|0x40;
	else ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]&0xBF;
}

/* Helper function to set the DTMEN bit */
void LTC6813_set_cfgrb_dtmen(ltc6813_driver_t *dev,
			                 uint8_t current_ic,
							 uint8_t dtmen //!< The DTMEN bit
							)
{
	cell_asic *ic = dev->ic_arr;
	if(dtmen) ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]|0x08;
	else ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]&0xF7;
}

/* Helper function to set the PATH SELECT bit */
void LTC6813_set_cfgrb_ps(ltc6813_driver_t *dev,
		                  uint8_t current_ic,
                          uint8_t ps[] //!< Path selection bits
					     )
{
	cell_asic *ic = dev->ic_arr;
	for(int i =0;i<2;i++)
	{
	  if(ps[i])ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]|(0x01<<(i+4));
	  else ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]&(~(0x01<<(i+4)));
	}
}

/*  Helper function to set the gpio bits in configb b register  */
void LTC6813_set_cfgrb_gpio_b(ltc6813_driver_t *dev,
		                      uint8_t current_ic,
							  uint8_t gpiobits[] //!< The GPIO bits
							 )
{
	cell_asic *ic = dev->ic_arr;
	for(int i =0;i<4;i++)
	{
	  if(gpiobits[i])ic[current_ic].configb.tx_data[0] = ic[current_ic].configb.tx_data[0]|(0x01<<i);
	  else ic[current_ic].configb.tx_data[0] = ic[current_ic].configb.tx_data[0]&(~(0x01<<i));
	}
}

/*  Helper function to set the dcc bits in configb b register */
void LTC6813_set_cfgrb_dcc_b(ltc6813_driver_t *dev,
		                     uint8_t current_ic,
							 uint8_t dccbits[] //!< The DCC bits
							)
{
	cell_asic *ic = dev->ic_arr;
	for(int i =0;i<7;i++)
	{
		if(i==0)
		{
			if(dccbits[i])ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]|0x04;
			else ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]&0xFB;
		}
		if(i>0 && i<5)
		{
			if(dccbits[i])ic[current_ic].configb.tx_data[0] = ic[current_ic].configb.tx_data[0]|(0x01<<(i+3));
			else ic[current_ic].configb.tx_data[0] = ic[current_ic].configb.tx_data[0]&(~(0x01<<(i+3)));
		}
		if(i>4 && i<7)
		{
			if(dccbits[i])ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]|(0x01<<(i-5));
			else ic[current_ic].configb.tx_data[1] = ic[current_ic].configb.tx_data[1]&(~(0x01<<(i-5)));
		}
	}
}

int LTC6813_init(ltc6813_driver_t *dev,
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
	// TODO: determie if specific ltc6813 config is needed
	return LTC681x_init(dev, hspi_a, hspi_b, cs_port_a, cs_port_b, cs_pin_a, cs_pin_b, num_ics, ic_arr);
}
