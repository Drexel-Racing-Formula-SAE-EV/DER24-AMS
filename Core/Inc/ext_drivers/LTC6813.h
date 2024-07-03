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

/*! @file
    @ingroup LTC6813-1
    Header for LTC6813-1 Multicell Battery Monitor
*/

#ifndef LTC6813_H_
#define LTC6813_H_


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "ext_drivers/LTC681x.h"
#include "stm32f4xx_hal.h"

#define CELL 1
#define AUX 2
#define STAT 3

#define ICOM_BLANK 0x00
#define ICOM_STOP  0x01
#define ICOM_START 0x06
#define ICOM_NT    0x07

#define FCOM_ACK       0x00
#define FCOM_NACK      0x08
#define FCOM_NACK_STOP 0x09

typedef ltc681x_driver_t ltc6813_driver_t;

/*!
 Helper function to initialize register limits
 @return void
 */
void LTC6813_init_reg_limits(ltc6813_driver_t *dev);

/*!
 Write the LTC6813 configuration register A
 @return void
 */
void LTC6813_wrcfg(ltc6813_driver_t *dev);

/*!
 Write the LTC6813 configuration register B
 @return void
 */
void LTC6813_wrcfgb(ltc6813_driver_t *dev);

/*!
 Reads configuration register A of a LTC6813 daisy chain
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC6813_rdcfg(ltc6813_driver_t *dev);

/*!
 Reads configuration register B of a LTC6813 daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
 */
int8_t LTC6813_rdcfgb(ltc6813_driver_t *dev);

/*!
 Starts cell voltage conversion
 @return void
 */
void LTC6813_adcv(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 );

/*!
 Start a GPIO and Vref2 Conversion
 @return void
 */
void LTC6813_adax(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
                  );

/*!
 Start a Status ADC Conversion
 @return void
 */
void LTC6813_adstat(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t CHST //!< Sets which Stat channels are converted
					);

/*!
 Starts cell voltage  and GPIO 1 & 2 conversion
 @return void
 */
void LTC6813_adcvax(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*! Starts cell voltage and Sum of cells conversion
 @return void
 */
void LTC6813_adcvsc(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*!
 Reads and parses the LTC6813 cell voltage registers.
 @return uint8_t, pec_error PEC Status.
 0: No PEC error detected
 -1: PEC error detected, retry read
 */
uint8_t LTC6813_rdcv(ltc6813_driver_t *dev,
		             uint8_t reg //!< Controls which cell voltage register is read back.
                    );

/*! Reads and parses the LTC6813 auxiliary registers.
 @return  int8_t, pec_error PEC Status
   0: No PEC error detected
  -1: PEC error detected, retry read
  */
int8_t LTC6813_rdaux(ltc6813_driver_t *dev,
		             uint8_t reg //!< Controls which GPIO voltage register is read back
                    );

/*!
 Reads and parses the LTC6813 stat registers.
 @return  int8_t, pec_error PEC Status
  0: No PEC error detected
  -1: PEC error detected, retry read
  */
int8_t LTC6813_rdstat(ltc6813_driver_t *dev,
		              uint8_t reg //!< Determines which Stat  register is read back.
                     );

/*!
  Sends the poll ADC command
  @returns 1 byte read back after a pladc command. If the byte is not 0xFF ADC conversion has completed
  */
uint8_t LTC6813_pladc(ltc6813_driver_t *dev);

/*!
  This function will block operation until the ADC has finished it's conversion
  @returns uint32_t, counter The approximate time it took for the ADC function to complete.
  */
uint32_t LTC6813_pollAdc(ltc6813_driver_t *dev);

/*!
 Clears the LTC6813 cell voltage registers
 @return void
 */
void LTC6813_clrcell(ltc6813_driver_t *dev);

/*!
 Clears the LTC6813 Auxiliary registers
 @return void
 */
void LTC6813_clraux(ltc6813_driver_t *dev);

/*!
 Clears the LTC6813 Stat registers
 @return void
 */
void LTC6813_clrstat(ltc6813_driver_t *dev);

/*!
 Starts the Mux Decoder diagnostic self test
 Running this command will start the Mux Decoder Diagnostic Self Test
 This test takes roughly 1mS to complete. The MUXFAIL bit will be updated,
 the bit will be set to 1 for a failure and 0 if the test has been passed.
 @return void
 */
void LTC6813_diagn(ltc6813_driver_t *dev);

/*!
 Starts cell voltage self test conversion
 @return void
 */
void LTC6813_cvst(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Self Test Mode
				 );

/*!
 Start an Auxiliary Register Self Test Conversion
 @return void
 */
void LTC6813_axst(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Sets if self test 1 or 2 is run
				 );

/*!
 Start a Status Register Self Test Conversion
 @return void
 */
void LTC6813_statst(ltc6813_driver_t *dev,
		            uint8_t MD, //!< ADC Conversion Mode
					uint8_t ST //!< Sets if self test 1 or 2 is run
					);

/*!
 Starts cell voltage overlap conversion
 @return void
 */
void LTC6813_adol(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t DCP //!< Discharge permitted during conversion
				 );

/*!
 Start an GPIO Redundancy test
 @return void
 */
void LTC6813_adaxd(ltc6813_driver_t *dev,
		           uint8_t MD, //!< ADC Conversion Mode
				   uint8_t CHG //!< Sets which GPIO channels are converted
				   );

/*!
 Start a Status register redundancy test Conversion
 @return void
  */
void LTC6813_adstatd(ltc6813_driver_t *dev,
		             uint8_t MD, //!< ADC Mode
					 uint8_t CHST //!< Sets which Status channels are converted
					);

/*!
 Helper function that runs the ADC Self Tests
 @return int16_t, error Number of errors detected.
 */
int16_t LTC6813_run_cell_adc_st(ltc6813_driver_t *dev,
		                        uint8_t adc_reg, //!< Type of register
                                uint8_t md, //!< ADC Mode
								uint8_t adcopt //!< The adcopt bit in the configuration register
								);

/*!
 Helper Function that runs the ADC Overlap test
 @return uint16_t, error
  0: Pass
 -1: False, Error detected
 */
uint16_t LTC6813_run_adc_overlap(ltc6813_driver_t *dev);

/*!
 Helper function that runs the ADC Digital Redundancy commands and checks output for errors
 @return int16_t, error Number of errors detected.
 */
int16_t LTC6813_run_adc_redundancy_st(ltc6813_driver_t *dev,
		                              uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg //!< Type of register
                                     );

/*!
 Start an open wire Conversion
 @return void
 */
void LTC6813_adow(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Conversion Mode
				  uint8_t PUP,//!< Pull up/Pull down current
				  uint8_t CH, //!< Sets which Cell channels are converted
				  uint8_t DCP //!< Discharge permitted during conversion
				 );

/*!
 Start GPIOs open wire ADC conversion
 @return void
 */
void LTC6813_axow(ltc6813_driver_t *dev,
		          uint8_t MD, //!< ADC Mode
				  uint8_t PUP //!< Pull up/Pull down current
				 );

/*!
 Helper function that runs the data sheet algorithm for open wire for single cell detection
 @return void
 */
void LTC6813_run_openwire_single(ltc6813_driver_t *dev);

/*!
 Helper function that runs open wire for multiple cell and two consecutive cells detection
 @return void
 */
void LTC6813_run_openwire_multi(ltc6813_driver_t *dev);

/*!
 Runs open wire for GPIOs
 @return void
 */
void LTC6813_run_gpio_openwire(ltc6813_driver_t *dev);

/*!
 Helper Function to Set DCC bits in the CFGR Registers
 @return void
 */
void LTC6813_set_discharge(ltc6813_driver_t *dev,
		                   int Cell //!< The cell to be discharged
		                  );

/*!
 Helper Function to Set DCC bits in the CFGR Registers
 @return void
 */
void LTC6813_set_discharge_per_segment(ltc6813_driver_t *dev,
								       uint8_t Cell,
									   uint8_t segment
									  );

/*!
 Helper Function to clear DCC bits in the CFGR Registers
 @return void
 */
void LTC6813_clear_discharge(ltc6813_driver_t *dev);

/*!
 Write the LTC6813 PWM register
 @return void
 */
void LTC6813_wrpwm(ltc6813_driver_t *dev,
                   uint8_t pwmReg //!<  PWM  Register A or B
                  );

/*!
 Reads pwm registers of a LTC6813 daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC
  */
int8_t LTC6813_rdpwm(ltc6813_driver_t *dev,
                     uint8_t pwmReg //!< PWM  Register A or B
                    );

/*!
 Write the LTC6813 Sctrl register
 @return void
 */
void LTC6813_wrsctrl(ltc6813_driver_t *dev,
                     uint8_t sctrl_reg //! SCTRL  Register A or B
                    );

/*!
 Reads sctrl registers of a LTC6813 daisy chain
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
   -1: Data read back has incorrect PEC
   */
int8_t LTC6813_rdsctrl(ltc6813_driver_t *dev,
                       uint8_t sctrl_reg //!< SCTRL Register A or B
                      );

/*!
 Start Sctrl data communication
 This command will start the sctrl pulse communication over the spins
 @return void
 */
void LTC6813_stsctrl(ltc6813_driver_t *dev);

/*!
 Clears the LTC6813 Sctrl registers
 @return void
 */
void LTC6813_clrsctrl(ltc6813_driver_t *dev);

/*!
 Write the 6813 PWM/Sctrl Register B
 @return void
 */
void LTC6813_wrpsb(ltc6813_driver_t *dev);

/*!
 Reading pwm/s control register B
 @return uint8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC
  */
uint8_t LTC6813_rdpsb(ltc6813_driver_t *dev);

/*!
 Write the LTC6813 COMM register
 @return void
 */
void LTC6813_wrcomm(ltc6813_driver_t *dev);

/*!
 Reads comm registers of a LTC6813 daisy chain
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC
  */
int8_t LTC6813_rdcomm(ltc6813_driver_t *dev);

/*!
 Issues a stcomm command and clocks data out of the COMM register
 @return void
 */
void LTC6813_stcomm(ltc6813_driver_t *dev,
		            uint8_t len //!< Length of data to be transmitted
				   );

/*!
 Mutes the LTC6813 discharge transistors
 @return void
 */
void LTC6813_mute(ltc6813_driver_t *dev);

/*!
 Clears the LTC6813 Mute Discharge
 @return void
 */
void LTC6813_unmute(ltc6813_driver_t *dev);

/*!
 Helper Function that counts overall PEC errors and register/IC PEC errors
 @return void
 */
void LTC6813_check_pec(ltc6813_driver_t *dev,
                       uint8_t reg //!<  Type of register
					  );

/*!
 Helper Function that resets the PEC error counters
 @return void
 */
void LTC6813_reset_crc_count(ltc6813_driver_t *dev);

/*!
 Helper Function to initialize the CFGR data structures
 @return void
 */
void LTC6813_init_cfg(ltc6813_driver_t *dev);

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void
 */
void LTC6813_set_cfgr(ltc6813_driver_t *dev,
					  uint8_t current_ic,
                      uint8_t refon, //!< The REFON bit
                      uint8_t adcopt, //!< The ADCOPT bit
                      uint8_t gpio[5], //!< The GPIO bits
                      uint8_t dcc[12], //!< The DCC bits
					  uint8_t dcto[4], //!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t ov //!< The OV value
					  );

/*!
 Helper function to turn the REFON bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgr_refon(ltc6813_driver_t *dev,
		                    uint8_t current_ic,
                            uint8_t refon //!< The REFON bit
							);

/*!
 Helper function to turn the ADCOPT bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgr_adcopt(ltc6813_driver_t *dev,
		                     uint8_t current_ic,
                             uint8_t adcopt //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW
 @return void
 */
void LTC6813_set_cfgr_gpio(ltc6813_driver_t *dev,
		                   uint8_t current_ic,
                           uint8_t gpio[] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW
 @return void
 */
void LTC6813_set_cfgr_dis(ltc6813_driver_t *dev,
                          uint8_t current_ic,
                          uint8_t dcc[] //!< The DCC bits
						  );

/*!
 Helper function to set UV field in CFGRA register
 @return void
 */
void LTC6813_set_cfgr_uv(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint16_t uv //!< The UV value
						 );

/*!
 Helper function to set DCTO  field in CFGRA register
 @return void
 */
void LTC6813_set_cfgr_dcto(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint8_t dcto[4] //!< The Dcto bits
						 );

/*!
 Helper function to set OV field in CFGRA register
 @return void
 */
void LTC6813_set_cfgr_ov(ltc6813_driver_t *dev,
                         uint8_t current_ic,
                         uint16_t ov //!< The OV value
						 );

/*!
 Helper Function to initialize the CFGR B data structures
 @return void
 */
void LTC6813_init_cfgb(ltc6813_driver_t *dev);

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void
 */
void LTC6813_set_cfgrb(ltc6813_driver_t *dev,
					   uint8_t current_ic,
					  uint8_t fdrf, //!< The FDRF bit
                      uint8_t dtmen, //!< The DTMEN bit
                      uint8_t ps[2], //!< Path selection bits
					  uint8_t gpiobits[4], //!< The GPIO bits
					  uint8_t dccbits[7] //!< The DCC bits
					  );

/*!
 Helper function to turn the FDRF bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_fdrf(ltc6813_driver_t *dev,
							uint8_t current_ic,
							uint8_t fdrf //!< The FDRF bit
						   );

/*!
 Helper function to turn the DTMEN bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_dtmen(ltc6813_driver_t *dev,
			                 uint8_t current_ic,
							 uint8_t dtmen //!< The DTMEN bit
							);

/*!
 Helper function to turn the Path Select bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_ps(ltc6813_driver_t *dev,
		                  uint8_t current_ic,
                          uint8_t ps[] //!< Path selection bits
					     );

/*!
 Helper function to turn the GPIO bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_gpio_b(ltc6813_driver_t *dev,
		                      uint8_t current_ic,
							  uint8_t gpiobits[] //!< The GPIO bits
							 );

/*!
 Helper function to turn the DCC bit HIGH or LOW
 @return void
 */
void LTC6813_set_cfgrb_dcc_b(ltc6813_driver_t *dev,
		                     uint8_t current_ic,
							 uint8_t dccbits[] //!< The DCC bits
							);

int LTC6813_init(ltc6813_driver_t *dev,
		         SPI_HandleTypeDef *hspi_a,
				 SPI_HandleTypeDef *hspi_b,
				 GPIO_TypeDef *cs_port_a,
				 GPIO_TypeDef *cs_port_b,
				 uint16_t cs_pin_a,
				 uint16_t cs_pin_b,
				 uint8_t num_ics,
				 cell_asic *ic_arr,
				 TIM_HandleTypeDef *htim
				);

#endif
