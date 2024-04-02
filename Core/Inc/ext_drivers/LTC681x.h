/*! General BMS Library
*******************************************************************************
*   @file     LTC681x.h
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

#ifndef LTC681x_H_
#define LTC681x_H_

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"

/*! Cell Voltage data structure. */
typedef struct
{
  volatile uint16_t c_codes[18]; //!< Cell Voltage Codes
  uint8_t pec_match[6]; //!< If a PEC error was detected during most recent read cmd
} cv;

/*! AUX Reg Voltage Data structure */
typedef struct
{
  uint16_t a_codes[9]; //!< Aux Voltage Codes
  uint8_t pec_match[4]; //!< If a PEC error was detected during most recent read cmd
} ax;

/*! Status Reg data structure. */
typedef struct
{
  uint16_t stat_codes[4]; //!< Status codes.
  uint8_t flags[3]; //!< Byte array that contains the uv/ov flag data
  uint8_t mux_fail[1]; //!< Mux self test status flag
  uint8_t thsd[1]; //!< Thermal shutdown status
  uint8_t pec_match[2]; //!< If a PEC error was detected during most recent read cmd
} st;

/*! IC register structure. */
typedef struct
{
  uint8_t tx_data[6];  //!< Stores data to be transmitted
  uint8_t rx_data[8];  //!< Stores received data
  uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
} ic_register;

/*! PEC error counter structure. */
typedef struct
{
  uint16_t pec_count; //!< Overall PEC error count
  uint16_t cfgr_pec;  //!< Configuration register data PEC error count
  uint16_t cell_pec[6]; //!< Cell voltage register data PEC error count
  uint16_t aux_pec[4];  //!< Aux register data PEC error count
  uint16_t stat_pec[2]; //!< Status register data PEC error count
} pec_counter;

/*! Register configuration structure */
typedef struct
{
  uint8_t cell_channels; //!< Number of Cell channels
  uint8_t stat_channels; //!< Number of Stat channels
  uint8_t aux_channels;  //!< Number of Aux channels
  uint8_t num_cv_reg;    //!< Number of Cell voltage register
  uint8_t num_gpio_reg;  //!< Number of Aux register
  uint8_t num_stat_reg;  //!< Number of  Status register
} register_cfg;

/*! Cell variable structure */
typedef struct
{
  ic_register config;
  ic_register configb;
  cv cells;
  ax aux;
  st stat;
  ic_register com;
  ic_register pwm;
  ic_register pwmb;
  ic_register sctrl;
  ic_register sctrlb;
  uint8_t sid[6];
  uint8_t isospi_reverse;
  pec_counter crc_count;
  register_cfg ic_reg;
  long system_open_wire;
} cell_asic;

typedef enum
{
	LTC681X_STR_A,
	LTC681X_STR_B
} ltc681x_str_t;

#define IC_LTC6813

#define LTC_6813B_CS_RESET CSE6_RESET//CSC1_RESET //CSB12_RESET
#define LTC_6813B_CS_SET CSE6_SET//CSC1_SET //CSB12_SET

#define LTC_6813_CS_RESET CSC1_RESET //CSB12_RESET
#define LTC_6813_CS_SET CSC1_SET //CSB12_SET

#define MD_422HZ_1KHZ 0
#define MD_27KHZ_14KHZ 1
#define MD_7KHZ_3KHZ 2
#define MD_26HZ_2KHZ 3

#define ADC_OPT_ENABLED 1
#define ADC_OPT_DISABLED 0

#define CELL_CH_ALL 0
#define CELL_CH_1and7 1
#define CELL_CH_2and8 2
#define CELL_CH_3and9 3
#define CELL_CH_4and10 4
#define CELL_CH_5and11 5
#define CELL_CH_6and12 6

#define SELFTEST_1 1
#define SELFTEST_2 2

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6

#define STAT_CH_ALL 0
#define STAT_CH_SOC 1
#define STAT_CH_ITEMP 2
#define STAT_CH_VREGA 3
#define STAT_CH_VREGD 4

#define REG_ALL 0
#define REG_1 1
#define REG_2 2
#define REG_3 3
#define REG_4 4
#define REG_5 5
#define REG_6 6

#define DCP_DISABLED 0
#define DCP_ENABLED 1

#define PULL_UP_CURRENT 1
#define PULL_DOWN_CURRENT 0

#define NUM_RX_BYT 8
#define CELL 1
#define AUX 2
#define STAT 3
#define CFGRR 0
#define CFGRB 4
//#define CS_PIN 10

typedef struct
{
	ltc681x_str_t string;
	SPI_HandleTypeDef *hspi[2];
	GPIO_TypeDef *cs_port[2];
	uint16_t cs_pin[2];
	uint8_t num_ics;
	cell_asic *ic_arr;
} ltc681x_driver_t;

/*!
 Wake isoSPI up from IDlE state and enters the READY state
 @return void
 */
void wakeup_idle(ltc681x_driver_t *dev);//!< Number of ICs in the daisy chain

/*!
 Wake the LTC681x from the sleep state
 @return void
 */
void wakeup_sleep(ltc681x_driver_t *dev); //!< Number of ICs in the daisy chain

/*!
 Sends a command to the BMS IC. This code will calculate the PEC code for the transmitted command
 @return void
 */
void cmd_68(ltc681x_driver_t *dev, uint8_t tx_cmd[2]); //!< 2 byte array containing the BMS command to be sent

/*!
 Writes an array of data to the daisy chain
 @return void
 */
void write_68(ltc681x_driver_t *dev,
              uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
              uint8_t data[] //!< Array containing the data to be written to the BMS ICs
             );

/*!
 Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t read_68(ltc681x_driver_t *dev,
               uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
               uint8_t *rx_data); //!< Array that the read back data will be stored in.

/*!
 Calculates  and returns the CRC15
 @returns The calculated pec15 as an unsigned int
  */
uint16_t pec15_calc(uint8_t len, //!< The length of the data array being passed to the function
                    uint8_t *data //!< The array of data that the PEC will be generated from
                   );

/*!
 Write the LTC681x CFGRA register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void LTC681x_wrcfg(ltc681x_driver_t *dev);

/*!
 Write the LTC681x CFGRB register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack.
 The configuration is written in descending order so the last device's configuration is written first.
 @return void
 */
void LTC681x_wrcfgb(ltc681x_driver_t *dev);

/*!
 Reads the LTC681x CFGRA register
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdcfg(ltc681x_driver_t *dev);

/*!
 Reads the LTC681x CFGRB register
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdcfgb(ltc681x_driver_t *dev);

/*!
 Starts cell voltage conversion
 Starts ADC conversions of the LTC681x Cpin inputs.
 The type of ADC conversion executed can be changed by setting the following parameters:
 @return void
 */
void LTC681x_adcv(ltc681x_driver_t *dev, // device driver
				  uint8_t MD, //!< ADC conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 );

/*!
 Start a GPIO and Vref2 Conversion
 @return void
 */
void LTC681x_adax(ltc681x_driver_t *dev, // device driver
				  uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
				 );

/*!
 Start a Status ADC Conversion
 @return void
 */
void LTC681x_adstat(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //!< ADC Conversion Mode
					uint8_t CHST //!< Sets which Stat channels are converted
					);

/*!
 Starts cell voltage  and GPIO 1&2 conversion
 @return void
 */
void LTC681x_adcvax(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*!
 Starts cell voltage and SOC conversion
 @return void
 */
void LTC681x_adcvsc(ltc681x_driver_t *dev, // device driver
					uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*!
 Reads and parses the LTC681x cell voltage registers.
 The function is used to read the cell codes of the LTC681x.
 This function will send the requested read commands parse the data and store the cell voltages in the cell_asic structure.
 @return uint8_t, PEC Status.
  0: No PEC error detected
 -1: PEC error detected, retry read
 */
uint8_t LTC681x_rdcv(ltc681x_driver_t *dev,
					 uint8_t reg //!< Controls which cell voltage register is read back.
                    );

/*!
 Reads and parses the LTC681x auxiliary registers.
 The function is used to read the  parsed GPIO codes of the LTC681x.
 This function will send the requested read commands parse the data and store the gpio voltages in the cell_asic structure.
 @return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read
  */
int8_t LTC681x_rdaux(ltc681x_driver_t *dev,
					 uint8_t reg //!< Determines which GPIO voltage register is read back.
                    );

/*!
 Reads and parses the LTC681x stat registers.
 The function is used to read the  parsed status codes of the LTC681x.
 This function will send the requested read commands parse the data and store the status voltages in the cell_asic structure
 @return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read
 */
int8_t LTC681x_rdstat(ltc681x_driver_t *dev,
					  uint8_t reg //!< Determines which Stat  register is read back.
                     );

/*!
 Reads the raw cell voltage register data
 @return void
 */
void LTC681x_rdcv_reg(ltc681x_driver_t *dev, // device driver
					  uint8_t reg, //!< Determines which cell voltage register is read back
                      uint8_t *data //!< An array of the unparsed cell codes
                     );

/*!
 Read the raw data from the LTC681x auxiliary register
 The function reads a single GPIO voltage register and stores the read data in the *data point as a byte array.
 This function is rarely used outside of the LTC681x_rdaux() command.
 @return void
 */
void LTC681x_rdaux_reg(ltc681x_driver_t *dev, // device driver
					   uint8_t reg, //!< Determines which GPIO voltage register is read back
                       uint8_t *data //!< Array of the unparsed auxiliary codes
                      );

/*!
 Read the raw data from the LTC681x stat register
 The function reads a single Status register and stores the read data in the *data point as a byte array.
 This function is rarely used outside of the LTC681x_rdstat() command.
 @return void
 */
void LTC681x_rdstat_reg(ltc681x_driver_t *dev, // device driver
						uint8_t reg, //!< Determines which stat register is read back
                        uint8_t *data //!< Array of the unparsed stat codes
                       );

/*!
 Helper function that parses voltage measurement registers
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t parse_cells(uint8_t current_ic, //!< Current IC
                   uint8_t cell_reg, //!< Type of register
                   uint8_t cell_data[], //!< Unparsed data
                   uint16_t *cell_codes, //!< Parsed data
                   uint8_t *ic_pec //!< PEC error
				  );

/*!
 Sends the poll ADC command
 @returns uint8_t adc_state 1 byte read back after a pladc command. If the byte is not 0xFF ADC conversion has completed
 */
uint8_t LTC681x_pladc(ltc681x_driver_t *dev);

/*!
  This function will block operation until the ADC has finished it's conversion
  @returns uint32_t counter The approximate time it took for the ADC function to complete.
  */
uint32_t LTC681x_pollAdc(ltc681x_driver_t *dev);

/*!
 Clears the LTC681x Cell voltage registers
 The command clears the cell voltage registers and initializes all values to 1.
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void
 */
void LTC681x_clrcell(ltc681x_driver_t *dev);

/*!
 Clears the LTC681x Auxiliary registers
 The command clears the Auxiliary registers and initializes all values to 1.
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void
 */
void LTC681x_clraux(ltc681x_driver_t *dev);

/*!
 Clears the LTC681x Stat registers
 The command clears the Stat registers and initializes all values to 1.
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void
 */
void LTC681x_clrstat(ltc681x_driver_t *dev);

/*!
 Starts the Mux Decoder diagnostic self test
 Running this command will start the Mux Decoder Diagnostic Self Test
 This test takes roughly 1ms to complete. The MUXFAIL bit will be updated,
 the bit will be set to 1 for a failure and 0 if the test has been passed.
 @return void
 */
void LTC681x_diagn(ltc681x_driver_t *dev);

/*!
 Starts cell voltage self test conversion
 @return void
 */
void LTC681x_cvst(ltc681x_driver_t *dev,
				  uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!<  Sets if self test 1 or 2 is run
				 );

/*!
 Start an Auxiliary Register Self Test Conversion
 @return void
 */
void LTC681x_axst(ltc681x_driver_t *dev,
				  uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Sets if self test 1 or 2 is run
				 );

/*!
 Start a Status Register Self Test Conversion
 @return void
 */
void LTC681x_statst(ltc681x_driver_t *dev,
					uint8_t MD, //!< ADC Conversion Mode
					uint8_t ST //!< Sets if self test 1 or 2 is run
				   );

/*!
 Starts cell voltage overlap conversion
 @return void
 */
void LTC681x_adol(ltc681x_driver_t *dev,
				  uint8_t MD, //!< ADC Conversion Mode
				  uint8_t DCP //!< Discharge permitted during conversion
				 );

/*!
 Start an GPIO Redundancy test
 @return void
 */
void LTC681x_adaxd(ltc681x_driver_t *dev,
				   uint8_t MD, //!< ADC Conversion Mode
				   uint8_t CHG //!< Sets which GPIO channels are converted
				  );

/*!
 Start a Status register redundancy test Conversion
 @return void
 */
void LTC681x_adstatd(ltc681x_driver_t *dev,
		             uint8_t MD, //!< ADC Mode
					 uint8_t CHST //!< Sets which Status channels are converted
					);

/*!
 Helper function that runs the ADC Self Tests
 @return int16_t, error   Number of errors detected.
 */
int16_t LTC681x_run_cell_adc_st(ltc681x_driver_t *dev,
							    uint8_t adc_reg, //!< Type of register
								uint8_t md, //!< ADC Mode
								uint8_t adcopt //!< ADCOPT bit in the configuration register
							   );

/*!
 Self Test Helper Function
 @return uint16_t test_pattern returns the register data pattern for a given ADC MD and Self test
 */
uint16_t LTC681x_st_lookup(uint8_t MD, //!< ADC Mode
						   uint8_t ST, //!< Self Test
						   uint8_t adcopt //!< ADCOPT bit in the configuration register
						  );

/*!
 Helper Function that runs the ADC Overlap test
 @return uint16_t, error
  0: Pass
 -1: False, Error detected
 */
uint16_t LTC681x_run_adc_overlap(ltc681x_driver_t *dev);

/*!
 Helper function that runs the ADC Digital Redundancy commands and checks output for errors
 @return int16_t, error   Number of errors detected.
 */
int16_t LTC681x_run_adc_redundancy_st(ltc681x_driver_t *dev,
									  uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg //!< Type of register
									 );

/*!
 Start an open wire Conversion
 @return void
 */
void LTC681x_adow(ltc681x_driver_t *dev,
				  uint8_t MD, //!< ADC Conversion Mode
				  uint8_t PUP, //!< Pull up/Pull down current
				  uint8_t CH, //!< Channels
				  uint8_t DCP//!< Discharge Permit
				);

/*!
 Start GPIOs open wire ADC conversion
 @return void
 */
void LTC681x_axow(ltc681x_driver_t *dev,
				  uint8_t MD, //!< ADC Mode
				  uint8_t PUP //!<Pull up/Pull down current
				 );

/*!
 Helper function that runs the data sheet algorithm for open wire for single cell detection
 @return void
 */
void LTC681x_run_openwire_single(ltc681x_driver_t *dev);

/*!
 Helper function that runs open wire for multiple cell and two consecutive cells detection
 @return void
 */
 void LTC681x_run_openwire_multi(ltc681x_driver_t *dev);

/*!
 Runs open wire for GPIOs
 @return void
 */
void LTC681x_run_gpio_openwire(ltc681x_driver_t *dev);

/*!
 Helper Function to clear DCC bits in the CFGR Registers
 @return void
 */
void LTC681x_clear_discharge(ltc681x_driver_t *dev);

/*!
 Write the LTC681x PWM register
 This command will write the pwm registers of the LTC681x connected in a daisy chain stack.
 The pwm is written in descending order so the last device's pwm is written first.
 @return void
 */
void LTC681x_wrpwm(ltc681x_driver_t *dev,
                   uint8_t pwmReg  //!< The PWM Register to be written
                  );

/*!
  Reads pwm registers of a LTC681x daisy chain
  @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdpwm(ltc681x_driver_t *dev,
                     uint8_t pwmReg //!< The PWM Register to be written A or B
                    );

/*!
 Write the LTC681x Sctrl register
 @return void
 */
void LTC681x_wrsctrl(ltc681x_driver_t *dev,
                     uint8_t sctrl_reg //!< The Sctrl Register to be written A or B
                    );

/*!
 Reads sctrl registers of a LTC681x daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdsctrl(ltc681x_driver_t *dev,
                       uint8_t sctrl_reg //!< The Sctrl Register to be written A or B
                      );

/*!
 Start Sctrl data communication
 This command will start the sctrl pulse communication over the spins
 @return void
 */
void LTC681x_stsctrl(ltc681x_driver_t *dev);

/*!
 Clears the LTC681x SCTRL registers
 The command clears the SCTRL registers and initializes all values to 0.
 The register will read back hexadecimal 0x00 after the command is sent.
 @return void
 */
void LTC681x_clrsctrl(ltc681x_driver_t *dev);

/*!
 Write the LTC681x COMM register
 This command will write the comm registers of the LTC681x connected in a daisy chain stack.
 The comm is written in descending order so the last device's configuration is written first.
 @return void
 */
void LTC681x_wrcomm(ltc681x_driver_t *dev);

/*!
 Reads comm registers of a LTC681x daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC681x_rdcomm(ltc681x_driver_t *dev);

/*!
 Issues a stcomm command and clocks data out of the COMM register
 @return void
 */
void LTC681x_stcomm(ltc681x_driver_t *dev,
					uint8_t len //!< Length of data to be transmitted
					);

/*!
 Helper Function that counts overall PEC errors and register/IC PEC errors
 @return void
 */
void LTC681x_check_pec(ltc681x_driver_t *dev,
                       uint8_t reg //!< Type of register
					   );

/*!
 Helper Function that resets the PEC error counters
 @return void
 */
void LTC681x_reset_crc_count(ltc681x_driver_t *dev);

/*!
 Helper Function to initialize the CFGR data structures
 @return void
 */
void LTC681x_init_cfg(ltc681x_driver_t *dev);

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void
 */
void LTC681x_set_cfgr(ltc681x_driver_t *dev,
					  uint8_t nIC, //!< Current IC
                      uint8_t refon,  //!< The REFON bit
					  uint8_t adcopt, //!< The ADCOPT bit
					  uint8_t gpio[5],//!< The GPIO bits
					  uint8_t dcc[12],//!< The DCC bits
					  uint8_t dcto[4],//!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t ov  //!< The OV value
					  );

/*!
 Helper function to turn the REFON bit HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_refon(ltc681x_driver_t *dev,
							uint8_t nIC, //!< Current IC
							uint8_t refon //!< The REFON bit
							);

/*!
 Helper function to turn the ADCOPT bit HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_adcopt(ltc681x_driver_t *dev,
							 uint8_t nIC, //!< Current IC
							 uint8_t adcopt //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_gpio(ltc681x_driver_t *dev,
						   uint8_t nIC, //!< Current IC
						   uint8_t gpio[] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW
 @return void
 */
void LTC681x_set_cfgr_dis(ltc681x_driver_t *dev,
						  uint8_t nIC, //!< Current IC
						  uint8_t dcc[] //!< The DCC bits
						  );

/*!
 Helper function to control discharge time value
 @return void
 */
void LTC681x_set_cfgr_dcto(ltc681x_driver_t *dev,
						   uint8_t nIC,  //!< Current IC
						   uint8_t dcto[] //!< The Dcto bits
						   );

/*!
 Helper function to set uv field in CFGRA register
 @return void
 */
void LTC681x_set_cfgr_uv(ltc681x_driver_t *dev,
						 uint8_t nIC, //!< Current IC
                         uint16_t uv //!< The UV value
						 );

/*!
 Helper function to set ov field in CFGRA register
 @return void
 */
void LTC681x_set_cfgr_ov(ltc681x_driver_t *dev,
						 uint8_t nIC, //!< Current IC
                         uint16_t ov //!< The OV value
						 );
/*
Writes an array of bytes out of the SPI port
*/
void spi_write_array(ltc681x_driver_t *dev, // DEVICE DRIVER
					uint8_t len, // Option: Number of bytes to be written on the SPI port
                     uint8_t data[] //Array of bytes to be written on the SPI port
                    );
/*
 Writes and read a set number of bytes using the SPI port.

*/

void spi_write_read(ltc681x_driver_t *dev,
				    uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   );

uint8_t spi_read_byte(ltc681x_driver_t *dev, uint8_t tx_dat);//name conflicts with linduino also needs to take a byte as a parameter

void LTC681x_set_cs(ltc681x_driver_t *dev, int state);

int LTC681x_init(ltc681x_driver_t *dev,
		         SPI_HandleTypeDef *hspi_a,
				 SPI_HandleTypeDef *hspi_b,
				 GPIO_TypeDef *cs_port_a,
				 GPIO_TypeDef *cs_port_b,
				 uint16_t cs_pin_a,
				 uint16_t cs_pin_b,
				 uint8_t num_ics,
				 cell_asic *ic_arr
				);

#endif
