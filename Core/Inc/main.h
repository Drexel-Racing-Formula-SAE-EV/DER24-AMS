/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define STRINGA_CS_Pin GPIO_PIN_0
#define STRINGA_CS_GPIO_Port GPIOC
#define STRINGB_CS_Pin GPIO_PIN_1
#define STRINGB_CS_GPIO_Port GPIOC
#define IMD_PWM_MCU_Pin GPIO_PIN_0
#define IMD_PWM_MCU_GPIO_Port GPIOA
#define C_SENSE_H_MCU_Pin GPIO_PIN_1
#define C_SENSE_H_MCU_GPIO_Port GPIOA
#define C_SENSE_L_MCU_Pin GPIO_PIN_4
#define C_SENSE_L_MCU_GPIO_Port GPIOC
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define BMS_SAFETY_OUT_Pin GPIO_PIN_11
#define BMS_SAFETY_OUT_GPIO_Port GPIOD
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SCLKB_Pin GPIO_PIN_10
#define SCLKB_GPIO_Port GPIOC
#define MISOB_Pin GPIO_PIN_11
#define MISOB_GPIO_Port GPIOC
#define MOSIB_Pin GPIO_PIN_12
#define MOSIB_GPIO_Port GPIOC
#define CAN_MCU_N_Pin GPIO_PIN_0
#define CAN_MCU_N_GPIO_Port GPIOD
#define CAN_MCU_P_Pin GPIO_PIN_1
#define CAN_MCU_P_GPIO_Port GPIOD
#define IMD_STATUS_MCU_Pin GPIO_PIN_6
#define IMD_STATUS_MCU_GPIO_Port GPIOD
#define AIR_CONTROL_MCU_Pin GPIO_PIN_7
#define AIR_CONTROL_MCU_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
