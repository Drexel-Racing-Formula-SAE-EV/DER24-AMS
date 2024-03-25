/**
 * @file canbus_task.c
 * @author Cole Bardin (cab572@drexel.edu)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tasks/canbus_task.h"
#include "ext_drivers/canbus.h"

/**
 * @brief CANBus task function
 * 
 * @param arg App_data struct pointer converted to void pointer
 */
void canbus_task_fn(void *arg);

TaskHandle_t canbus_task_start(app_data_t *data) {
    TaskHandle_t handle;
    xTaskCreate(canbus_task_fn, "CANBus Task", 128, (void *)data, 8, &handle);
    return handle;
}

void canbus_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;

    canbus_device_t *canbus = &data->board.canbus;
    CAN_TxHeaderTypeDef *tx_header = &canbus->tx_header;
    HAL_StatusTypeDef ret;
    uint32_t entry;
    uint16_t packet;
    uint8_t can_data[8] = {0};

    tx_header->StdId = ECU_CANBUS_ID;

    for(;;)
    {
    	entry = osKernelGetTickCount();
    	ret = 0;
    	packet = 0;
    	can_data[0] = TO_LSB16(packet);
    	can_data[1] = TO_MSB16(packet);
    	can_data[2] = TO_LSB16(data->air_state);
    	can_data[3] = TO_MSB16(data->air_state);
    	can_data[4] = TO_LSB16(data->imd_state);
    	can_data[5] = TO_MSB16(data->imd_state);
    	can_data[6] = TO_LSB16((int)(data->current * 10.0));
    	can_data[7] = TO_MSB16((int)(data->current * 10.0));
    	ret = HAL_CAN_AddTxMessage(canbus->hcan, tx_header, can_data, &canbus->tx_mailbox);
    	data->canbus_fault = ret;

    	// TODO: write out all the other packets!
        osDelayUntil(entry + (1000 / CAN_FREQ));

    }
}

