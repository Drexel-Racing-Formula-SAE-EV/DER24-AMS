/*
 * error_task.h
 *
 *  Created on: May 30, 2024
 *      Author: Cassius Garcia
 */

#ifndef INC_TASKS_ERROR_TASK_H_
#define INC_TASKS_ERROR_TASK_H_

#include "app.h"
#include "cmsis_os.h"

TaskHandle_t error_task_start(app_data_t *data);

#endif /* INC_TASKS_ERROR_TASK_H_ */
