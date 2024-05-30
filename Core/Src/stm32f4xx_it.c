/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "app.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if 0
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

/* USER CODE BEGIN EV */
#endif
// TODO: cut these out if needed
extern TIM_HandleTypeDef htim2;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	extern app_data_t app;
	UART_HandleTypeDef *huart2 = app.board.cli.huart;
#if 0
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
#endif
  HAL_UART_IRQHandler(huart2);
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
	extern app_data_t app;
	TIM_HandleTypeDef *htim5 = app.board.imd.htim;
#if 0
  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */
#endif
  HAL_TIM_IRQHandler(htim5);
  /* USER CODE END TIM5_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	extern app_data_t app;
	cli_device_t *cli = &app.board.cli;
	char endl[] = "\r\n";
	HAL_StatusTypeDef ret = 0;

	if(cli->huart->Instance == huart->Instance)
	{
		if(cli->c == '\r')
		{
			ret = HAL_UART_Transmit_IT(cli->huart, (uint8_t *)endl, strlen(endl));
			cli->line[cli->index] = '\0';
			cli->index = 0;
			if(strlen(cli->line) > 0)
			{
				cli->msg_pending = true;
				cli->msg_count++;
			}
		}
		else if(cli->c == '\n')
		{
			// ignore \r
		}
		else if(cli->c == 127)
		{
			uint8_t del = 127;
			if(cli->index != 0)
			{
				cli->index--;
				cli->line[cli->index] = ' ';
				ret = HAL_UART_Transmit_IT(cli->huart, &cli->c, 1);
				ret = HAL_UART_Transmit_IT(cli->huart, (uint8_t *)&cli->line[cli->index], 1);
				ret = HAL_UART_Transmit_IT(cli->huart, &del, 1);
			}
		}
		else if(cli->c >= 32 && cli->c <= 126)
		{
			if(cli->index != CLI_LINESZ - 1)
			{
				cli->line[cli->index++] = cli->c;
				ret = HAL_UART_Transmit_IT(cli->huart, &cli->c, 1);
			}
		}
		ret = HAL_UART_Receive_IT(cli->huart, &cli->c, 1);
		app.cli_fault = (ret != HAL_OK);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	extern app_data_t app;
	imd_t *imd = &app.board.imd;
    if(htim->Instance == imd->htim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) imd_read(imd);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	//BaseType_t task = 0;
	extern app_data_t app;
	canbus_device_t *canbus = &app.board.canbus;
	canbus_packet_t *rx_packet = &canbus->rx_packet;
	CAN_RxHeaderTypeDef rx_header;
	charger_t *ccs = &app.board.charger;
	uint16_t voltage;
	uint16_t current;
	uint8_t flags;

	for (uint8_t i = 0; i < 8; i++) rx_packet->data[i] = 0x00;
	HAL_CAN_GetRxMessage(canbus->hcan, CAN_RX_FIFO0, &rx_header, rx_packet->data);
	rx_packet->id = rx_header.StdId;

	if(rx_packet->id == BCA_CANBUS_ID)
	{
		voltage = ((uint16_t)rx_packet->data[0] << 8) | (rx_packet->data[1] & 0xFF);
		current = ((uint16_t)rx_packet->data[2] << 8) | (rx_packet->data[3] & 0xFF);
		flags = rx_packet->data[4];

		ccs->read_voltage = (float)voltage / 10.0;
		ccs->read_current = (float)current / 10.0;
		ccs->hardware_fail =      flags & (0x1 << 0);
		ccs->overtemp_fail =      flags & (0x1 << 1);
		ccs->input_volt_fail =    flags & (0x1 << 2);
		ccs->voltage_sense_fail = flags & (0x1 << 3);
		ccs->communication_fail = flags & (0x1 << 4);
		ccs->flags = flags;
		ccs->rx_count++;
	}
}
/* USER CODE END 1 */
