/**
* @file cli_task.c
* @author Cole Bardin (cab572@drexel.edu)
* @brief
* @version 0.1
* @date 2023-10-24
*
* @copyright Copyright (c) 2023
*
*/

#include "tasks/cli_task.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "ext_drivers/cli.h"

/**
* @brief Actual CLI task function
*
* @param arg App_data struct pointer converted to void pointer
*/
void cli_task_fn(void *arg);
int cli_handle_cmd(int argc, char *argv[]);
int cmd_not_found(int argc, char *argv[]);

int help(int argc, char *argv[]);
int id(int argc, char *argv[]);
int get_faults(int argc, char *argv[]);
int get_stat(int argc, char *argv[]);
int get_fans(int argc, char *argv[]);
int get_current(int argc, char *argv[]);
int set_state(int argc, char *argv[]);

char outline[CLI_LINESZ];
app_data_t *data;
cli_device_t *cli;
command_t cmds[] =
{
	{"help", &help, "print help menu"},
	{"id", &id, "identifies system"},
	{"fault", &get_faults, "gets the faults of the system"},
	{"stat", &get_stat, "prints out min and max stats from accumulator"},
	{"fans", &get_fans, "prints out the status of the fans"},
	{"current", &get_current, "prints reading from current sensor"},
	{"state", &set_state, "changes the state of the AMS"}
};
char *state_str[] =
{
		"NULL",
		"start",
		"charge",
		"discharge",
		"balance",
		"error"
};

TaskHandle_t cli_task_start(app_data_t *data)
{
   TaskHandle_t handle;
   xTaskCreate(cli_task_fn, "CLI task", 512, (void *)data, CLI_PRIO, &handle);
   return handle;
}

void cli_task_fn(void *arg)
{
    data = (app_data_t *)arg;
    cli = &data->board.cli;
    uint32_t entry;
    char buf[CLI_LINESZ] = {0};
    char *tokens[MAXTOKS];
    int n;
    int ret = 0;
	
    cli_printline(cli, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    cli_printline(cli, "Type 'help' for list of commands");

	for(;;)
	{
		entry = osKernelGetTickCount();
		if(cli->msg_pending == true)
		{
			taskENTER_CRITICAL();
			memcpy(buf, cli->line, strlen(cli->line) + 1);
			memset(cli->line, 0, strlen(cli->line) + 1);
			n = tokenize(buf, tokens, MAXTOKS, " \t");
			ret = cli_handle_cmd(n, tokens);
			taskEXIT_CRITICAL();
			data->cli_fault = ret;
			cli->msg_pending = false;
			cli->msg_proc++;
		}
		osDelayUntil(entry + (1000 / CLI_FREQ));
	}
}

int cli_handle_cmd(int argc, char *argv[])
{
	cli_device_t *cli = &data->board.cli;
	int i;
	int ret = 0;
	bool cmd_found = false;
	int num_cmds = sizeof(cmds) / sizeof(command_t);

	for(i = 0; i < num_cmds; i++)
	{
		if(!strncmp(cmds[i].name, argv[0], CLI_LINESZ))
		{
			ret = cmds[i].func(argc, argv);
			cli->msg_valid++;
			cmd_found = true;
			break;
		}
	}
	if(!cmd_found) return cmd_not_found(argc, argv);
	cli->ret = ret;
	return ret;
}

int cmd_not_found(int argc, char *argv[])
{
	int ret = 0;
	snprintf(outline, CLI_LINESZ, "Command not found: \'%s\'", argv[0]);
	ret |= cli_printline(cli, outline);
	ret |= cli_printline(cli, "Type 'help' for list of commands");
	return ret;
}

int help(int argc, char *argv[])
{
	int num_cmds;
	int i;
	int ret = 0;

	ret |= cli_printline(cli, "---------- Help Menu ----------");
	num_cmds = sizeof(cmds) / sizeof(command_t);
	for(i = 0; i < num_cmds; i++)
	{
		snprintf(outline, CLI_LINESZ, "%s - %s", cmds[i].name, cmds[i].desc);
		ret |= cli_printline(cli, outline);
	}
	return ret;
}

int id(int argc, char *argv[])
{
    snprintf(outline, CLI_LINESZ, "DER AMS FW V%d.%d.%d", VER_MAJOR, VER_MINOR, VER_BUG);
	cli_printline(cli, outline);
	return 0;
}

int get_faults(int argc, char *argv[])
{
	int ret = 0;
	ret |= cli_printline(cli, "System faults:");
	snprintf(outline, CLI_LINESZ, "hard:   %d", data->hard_fault);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "soft:   %d", data->soft_fault);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "  cli:    %d", data->cli_fault);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "  fan:    %d", data->fan_fault);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "  canbus: %d", data->canbus_fault);
	ret |= cli_printline(cli, outline);
	return ret;
}

int get_stat(int argc, char *argv[])
{
	int ret = 0;
	snprintf(outline, CLI_LINESZ, "AMS State: %s", state_str[data->state]);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "AIR Status: %s", data->air_state ? "Closed" : "Open");
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "BMS OK: %d", data->bms_state);
	ret |= cli_printline(cli, outline);
	if(data->state == STATE_CHARGE)
	{
		ret |= cli_printline(cli, "Charging Stats:");
		ret |= cli_printline(cli, "Target / Actual");
		snprintf(outline, CLI_LINESZ, " %5.1f / %5.1f Volts", data->board.charger.target_voltage, data->board.charger.read_voltage);
		ret |= cli_printline(cli, outline);
		snprintf(outline, CLI_LINESZ, " %5.1f / %5.1f Amps", data->board.charger.target_current, data->board.charger.read_current);
		ret |= cli_printline(cli, outline);
		snprintf(outline, CLI_LINESZ, "Flag value: %d", data->board.charger.flags);
		ret |= cli_printline(cli, outline);
	}
	else if(data->state == STATE_BALANCE)
	{
		snprintf(outline, CLI_LINESZ, "Balancing %d cell(s), threshold = %d mV", data->acc.balance_cnt, (int)(BALANCE_THRESH * 1000.0));
		ret |= cli_printline(cli, outline);
	}
	snprintf(outline, CLI_LINESZ, "total voltage: %f", data->total_voltage);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "max cell voltage: %f", data->max_voltage);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "min cell voltage: %f", data->min_voltage);
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "max segment temp: %f", data->max_temp);
	ret |= cli_printline(cli, outline);
	return ret;
}

int get_fans(int argc, char *argv[])
{
	int ret = 0;
	snprintf(outline, CLI_LINESZ, "fan status: %s", data->fan_state ? "ON" : "OFF");
	ret |= cli_printline(cli, outline);
	snprintf(outline, CLI_LINESZ, "max segment temp: %f", data->max_temp);
	ret |= cli_printline(cli, outline);
	return ret;
}

int get_current(int argc, char *argv[])
{
	int ret = 0;
	snprintf(outline, CLI_LINESZ, "Current Value: %.3f amps", data->current);
	ret |= cli_printline(cli, outline);
	return ret;
}

int set_state(int argc, char *argv[])
{
	int ret = 0;

	if(argc == 1)
	{
		snprintf(outline, CLI_LINESZ, "AMS State: %s", state_str[data->state]);
		ret |= cli_printline(cli, outline);
	}
	else if(argc == 2)
	{
		if(!strcmp(argv[1], "charge")) data->state = STATE_CHARGE;
		else if(!strcmp(argv[1], "discharge")) data->state = STATE_DISCHARGE;
		else if(!strcmp(argv[1], "balance")) data->state = STATE_BALANCE;
		else
		{
			snprintf(outline, CLI_LINESZ, "ERROR: unrecognized state: %s", argv[1]);
			cli_printline(cli, outline);
			cli_printline(cli, "Usage: state [charge|discharge|balance]");
			return 1;
		}
		snprintf(outline, CLI_LINESZ, "AMS State: %s", state_str[data->state]);
		ret |= cli_printline(cli, outline);

		if(data->state == STATE_CHARGE) HAL_CAN_ActivateNotification(data->board.canbus.hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
		else HAL_CAN_DeactivateNotification(data->board.canbus.hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

		if(data->state != STATE_BALANCE) data->acc.stop_balance = true;
	}
	else
	{
		cli_printline(cli, "ERROR: too many arguments. use no arguments to query state, or 1 argument to set state");
		cli_printline(cli, "Usage: state [charge|discharge|balance]");
		return 1;
	}
	return 0;
}
