/*
 * app.h
 *
 *  Created on: Jan 29, 2024
 *      Author: cole
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "board.h"
#include "ext_drivers/accumulator.h"

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_BUG   3

#define ERR_FREQ 10
#define CLI_FREQ 10
#define IMD_FREQ 5
#define CURR_FREQ 5
#define FAN_FREQ 5
#define LTC_FREQ 4
#define CAN_FREQ 2

#define ERR_PRIO  9
#define LTC_PRIO  8
#define CLI_PRIO  7
#define CAN_PRIO  6
#define CURR_PRIO 5
#define FAN_PRIO  4
#define IMD_PRIO  3
#define CELL_PRIO 2

#define ECU_CANBUS_ID 0x69
#define CCS_CANBUS_ID 0x1806E5F4
#define BCA_CANBUS_ID 0x18FF50E5

#define CHARGE_MAX_VOLTAGE 294.0
#define CHARGE_MAX_CURRENT 10.0
#define BALANCE_THRESH 0.025

#define TO_LSB16(x) ((uint16_t)x & 0xff)
#define TO_MSB16(x) ((((uint16_t)x & 0xff00) >> 8) & 0xff)

// TODO: check temp thresholds
#define TEMP_THRESH_C 60.0
#define TEMP_THRESH_H 50.0
#define TEMP_THRESH_L 45.0
#define OVERVOLT 4.2
#define UNDERVOLT 2.5
#define OVERCURR 10.0

typedef enum
{
	STATE_NULL,
	STATE_START,
	STATE_CHARGE,
	STATE_DISCHARGE,
	STATE_BALANCE,
	STATE_ERROR
} state_t;

typedef struct
{
	float total_voltage;
	float max_voltage;
	float min_voltage;
	float max_temp;
	float current;

	bool hard_fault;
	bool soft_fault;

	bool fan_fault;
	bool cli_fault;
	bool canbus_fault;
	bool current_fault;
	bool ltc_fault;
	bool charger_fault;

	bool air_state;
	bool imd_ok;
	imd_status_t imd_status;
    bool fan_state;

    bool bms_state;
    
	state_t state;

	board_t board;
	accumulator_t acc;

	TaskHandle_t fan_task;
	TaskHandle_t cli_task;
	TaskHandle_t canbus_task;
	TaskHandle_t imd_task;
	TaskHandle_t current_task;
	TaskHandle_t ltc_task;
	TaskHandle_t error_task;
} app_data_t;

void app_create();
void set_bms(bool state);

#endif /* INC_APP_H_ */
