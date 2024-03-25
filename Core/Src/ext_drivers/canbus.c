/**
 * @file canbus.c
 * @author Cole Bardin (cab572@drexel.edu)
 * @brief 
 * @version 0.1
 * @date 2023-04-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ext_drivers/canbus.h"

void canbus_device_init(canbus_device_t *dev, CAN_HandleTypeDef *hcan)
{
    dev->hcan = hcan;

    dev->tx_header.IDE = CAN_ID_STD;
    dev->tx_header.StdId = 0x00;
    dev->tx_header.ExtId = 0x00;
    dev->tx_header.RTR = CAN_RTR_DATA;
    dev->tx_header.DLC = DATALEN;
    dev->tx_header.TransmitGlobalTime = DISABLE;
}
