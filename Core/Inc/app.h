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

#define LTC_FREQ 10

#define OVERVOLT 4.2
#define UNDERVOLT 2.5

typedef enum
{
	STATE_NULL,
	STATE_START,
	STATE_CHARGE,
	STATE_DISCARGE,
	STATE_BALANCE,
	STATE_ERROR
} state_t;

typedef struct
{
	float total_voltage;
	float max_voltage;
	float min_voltage;
	float max_temp;
	float avg_temp;
	float current;

	bool hardFault;
	bool softFault;

	bool IMD_fault;
	float IMD_freq; // maybe move enum IMD_code_t
	float IMD_duty;

	state_t state;

	board_t board;
	accumulator_t acc;

	TaskHandle_t fan_task;
	TaskHandle_t ltc_task;
} app_data_t;

void app_create();

#endif /* INC_APP_H_ */
