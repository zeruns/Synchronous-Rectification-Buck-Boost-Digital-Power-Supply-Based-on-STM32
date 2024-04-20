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
#include "stm32g4xx_hal.h"

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
#define Encoder_A_Pin GPIO_PIN_0
#define Encoder_A_GPIO_Port GPIOC
#define Encoder_A_EXTI_IRQn EXTI0_IRQn
#define Encoder_B_Pin GPIO_PIN_1
#define Encoder_B_GPIO_Port GPIOC
#define Encoder_KEY_Pin GPIO_PIN_2
#define Encoder_KEY_GPIO_Port GPIOC
#define ADC_Vin_Pin GPIO_PIN_0
#define ADC_Vin_GPIO_Port GPIOA
#define ADC_Iin_Pin GPIO_PIN_1
#define ADC_Iin_GPIO_Port GPIOA
#define ADC_Vout_Pin GPIO_PIN_2
#define ADC_Vout_GPIO_Port GPIOA
#define ADC_Iout_Pin GPIO_PIN_3
#define ADC_Iout_GPIO_Port GPIOA
#define ADC_TEMP_Pin GPIO_PIN_4
#define ADC_TEMP_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_6
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_4
#define LED_G_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOC
#define PWM1_L_Pin GPIO_PIN_14
#define PWM1_L_GPIO_Port GPIOB
#define PWM1_H_Pin GPIO_PIN_15
#define PWM1_H_GPIO_Port GPIOB
#define PWM2_L_Pin GPIO_PIN_6
#define PWM2_L_GPIO_Port GPIOC
#define PWM2_H_Pin GPIO_PIN_7
#define PWM2_H_GPIO_Port GPIOC
#define FAN_PWM_Pin GPIO_PIN_8
#define FAN_PWM_GPIO_Port GPIOC
#define I2C3_SDA_Pin GPIO_PIN_9
#define I2C3_SDA_GPIO_Port GPIOC
#define I2C3_SCL_Pin GPIO_PIN_8
#define I2C3_SCL_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define SP3_SCK_Pin GPIO_PIN_10
#define SP3_SCK_GPIO_Port GPIOC
#define SP3_MISO_Pin GPIO_PIN_11
#define SP3_MISO_GPIO_Port GPIOC
#define SP3_MOSI_Pin GPIO_PIN_12
#define SP3_MOSI_GPIO_Port GPIOC
#define Flash_CS_Pin GPIO_PIN_2
#define Flash_CS_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
