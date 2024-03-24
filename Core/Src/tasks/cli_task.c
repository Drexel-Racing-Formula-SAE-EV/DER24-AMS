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
void cli_handle_cmd(int argc, char *argv[]);
void cmd_not_found(int argc, char *argv[]);

int help(int argc, char *argv[]);
int get_throttle(int argc, char *argv[]);
int get_brakelight(int argc, char *argv[]);
int get_brake(int argc, char *argv[]);
int get_time(int argc, char *argv[]);
int set_time(int argc, char *argv[]);
int get_faults(int argc, char *argv[]);

char line[256];
app_data_t *data;
cli_device_t *cli;
command_t cmds[] =
{
	{"help", &help, "print help menu"},
	{"fault", &get_faults, "gets the faults of the system"}
};

TaskHandle_t cli_task_start(app_data_t *data)
{
   TaskHandle_t handle;
   xTaskCreate(cli_task_fn, "CLI task", 256, (void *)data, 14, &handle);
   return handle;
}

void cli_task_fn(void *arg)
{
    data = (app_data_t *)arg;
    cli = &data->board.cli;
    uint32_t entry;
    char buf[CLI_LINE_SIZE] = {0};
    char *tokens[MAXTOKS];
    int n;
	
	cli_printline(cli, "~~~~~~~~~~ DER ECU FW V0.1~~~~~~~~~~");
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
			cli_handle_cmd(n, tokens);
			taskEXIT_CRITICAL();
			cli->msg_pending = false;
			cli->msg_proc++;
		}
		osDelayUntil(entry + (1000 / CLI_FREQ));
	}
}

void cli_handle_cmd(int argc, char *argv[])
{
	int i;
	bool cmd_found = false;
	int num_cmds = sizeof(cmds) / sizeof(command_t);

	for(i = 0; i < num_cmds; i++)
	{
		if(!strncmp(cmds[i].name, argv[0], CLI_LINE_SIZE))
		{
			data->board.cli.ret = cmds[i].func(argc, argv);
			cli->msg_valid++;
			cmd_found = true;
			break;
		}
	}
	if(!cmd_found) cmd_not_found(argc, argv);
}

void cmd_not_found(int argc, char *argv[])
{
	snprintf(line, 256, "Command not found: \'%s\'", argv[0]);
	cli_printline(cli, line);
	cli_printline(cli, "Type 'help' for list of commands");
}

int help(int argc, char *argv[])
{
	int num_cmds;
	int i;

	cli_printline(cli, "---------- Help Menu ----------");
	num_cmds = sizeof(cmds) / sizeof(command_t);
	for(i = 0; i < num_cmds; i++)
	{
		snprintf(line, 256, "%s - %s", cmds[i].name, cmds[i].desc);
		cli_printline(cli, line);
	}
	return 0;
}

int get_faults(int argc, char *argv[])
{
	cli_printline(cli, "System faults:");
	snprintf(line, 256, "hard:   %d", data->hard_fault);
	cli_printline(cli, line);
	snprintf(line, 256, "soft:   %d", data->soft_fault);
	cli_printline(cli, line);
	return 0;
}

