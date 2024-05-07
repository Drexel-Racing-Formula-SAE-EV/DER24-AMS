/*
 * ltc6813_task.h
 *
 *  Created on: Apr 10, 2024
 *      Author: Cole Bardin
 */

#ifndef __LTC_TASK_H_
#define __LTC_TASK_H_

#include "app.h"
#include "cmsis_os.h"

TaskHandle_t ltc_task_start(app_data_t *data);

#endif /* __LTC_TASK_H_ */
