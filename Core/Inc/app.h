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

#define VER_MAJOR 0
#define VER_MINOR 1

#define CLI_FREQ 20

typedef enum
{
	STATE_NULL,
	STATE_START,
	STATE_CHARGE,
	STATE_DISCARGE,
	STATE_ERROR
} state_t;

typedef struct
{
	bool hard_fault;
	bool soft_fault;

	bool fan_fault;
	bool cli_fault;

	bool IMD_fault;
	float IMD_freq; // maybe move enum IMD_code_t
	float IMD_duty;

	state_t state;

	float max_temp;
	float avg_temp;
	float max_voltage;
	float min_voltage;
	float current;

	board_t board;
	accumulator_t accumulator;

	TaskHandle_t fan_task;
	TaskHandle_t cli_task;
} app_data_t;

void app_create();
void set_bms(bool state);

#endif /* INC_APP_H_ */
