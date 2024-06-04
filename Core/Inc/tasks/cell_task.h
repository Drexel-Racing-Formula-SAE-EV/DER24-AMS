/*
 * cells.h
 *
 *  Created on: Jun 3, 2024
 *      Author: Cassius Garcia
 */

#ifndef INC_TASKS_CELLS_H_
#define INC_TASKS_CELLS_H_

#include "app.h"
#include "cmsis_os.h"

TaskHandle_t cell_task_start(app_data_t *data);

#endif /* INC_TASKS_CELLS_H_ */
