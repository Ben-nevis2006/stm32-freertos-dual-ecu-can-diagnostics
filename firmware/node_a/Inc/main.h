/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define FAN_PWM_Pin GPIO_PIN_7
#define FAN_PWM_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define INA260_SCL_Pin GPIO_PIN_8
#define INA260_SCL_GPIO_Port GPIOB
#define INA260_SDA_Pin GPIO_PIN_9
#define INA260_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define H3_FAN_PWM_FREQUENCY_HZ           25000U
#define H3_FAN_PWM_TIMER_COUNTS           2560U
#define H3_FAN_PWM_IDLE_DUTY_PERCENT      50U
#define H3_FAN_PWM_PROFILE_HOLD_MS        10000U
#define H3_FAN_PWM_COMPARE_0_PERCENT      0U
#define H3_FAN_PWM_COMPARE_25_PERCENT     (H3_FAN_PWM_TIMER_COUNTS / 4U)
#define H3_FAN_PWM_COMPARE_50_PERCENT     (H3_FAN_PWM_TIMER_COUNTS / 2U)
#define H3_FAN_PWM_COMPARE_100_PERCENT    H3_FAN_PWM_TIMER_COUNTS
#define H3_TACH_TIMER_FREQUENCY_HZ        100000U
#define H3_TACH_TIMER_TICK_US             10U
#define H3_TACH_INPUT_FILTER              15U
#define H3_TACH_CLOCK_DIVISION            TIM_CLOCKDIVISION_DIV4
#define H3_TACH_CLOCK_DIVISION_VALUE      4U

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
