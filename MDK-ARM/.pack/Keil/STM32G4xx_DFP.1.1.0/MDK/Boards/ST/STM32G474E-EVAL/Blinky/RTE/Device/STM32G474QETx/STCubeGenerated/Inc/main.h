/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
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

void HAL_HRTIM_MspPostInit(HRTIM_HandleTypeDef *hhrtim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SAI_MCLK_A_Pin GPIO_PIN_2
#define SAI_MCLK_A_GPIO_Port GPIOE
#define SAI_SD_B_Pin GPIO_PIN_3
#define SAI_SD_B_GPIO_Port GPIOE
#define SAI_FS_A_Pin GPIO_PIN_4
#define SAI_FS_A_GPIO_Port GPIOE
#define TRACED2_Pin GPIO_PIN_5
#define TRACED2_GPIO_Port GPIOE
#define TRACED3_Pin GPIO_PIN_6
#define TRACED3_GPIO_Port GPIOE
#define TAMPER_KEY_Pin GPIO_PIN_13
#define TAMPER_KEY_GPIO_Port GPIOC
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define FMC_A3_Pin GPIO_PIN_3
#define FMC_A3_GPIO_Port GPIOF
#define FMC_A4_Pin GPIO_PIN_4
#define FMC_A4_GPIO_Port GPIOF
#define FMC_A5_Pin GPIO_PIN_5
#define FMC_A5_GPIO_Port GPIOF
#define FMC_A1_Pin GPIO_PIN_7
#define FMC_A1_GPIO_Port GPIOF
#define uSD_CS_Pin GPIO_PIN_8
#define uSD_CS_GPIO_Port GPIOF
#define SPI2_SCK_Pin GPIO_PIN_9
#define SPI2_SCK_GPIO_Port GPIOF
#define FMC_A0_Pin GPIO_PIN_10
#define FMC_A0_GPIO_Port GPIOF
#define PF0_OSC_IN_Pin GPIO_PIN_0
#define PF0_OSC_IN_GPIO_Port GPIOF
#define PF1_OSC_OUT_Pin GPIO_PIN_1
#define PF1_OSC_OUT_GPIO_Port GPIOF
#define VSENSE_Pin GPIO_PIN_0
#define VSENSE_GPIO_Port GPIOC
#define FMC_A2_Pin GPIO_PIN_2
#define FMC_A2_GPIO_Port GPIOF
#define MFX_IRQ_OUT_Pin GPIO_PIN_0
#define MFX_IRQ_OUT_GPIO_Port GPIOA
#define ADC_OUTN_Pin GPIO_PIN_1
#define ADC_OUTN_GPIO_Port GPIOA
#define DAC_Pin GPIO_PIN_4
#define DAC_GPIO_Port GPIOA
#define MC_DAC_OUT2_Pin GPIO_PIN_5
#define MC_DAC_OUT2_GPIO_Port GPIOA
#define DISCHARGE_Pin GPIO_PIN_2
#define DISCHARGE_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_11
#define LED3_GPIO_Port GPIOF
#define FMC_A6_Pin GPIO_PIN_12
#define FMC_A6_GPIO_Port GPIOF
#define FMC_A7_Pin GPIO_PIN_13
#define FMC_A7_GPIO_Port GPIOF
#define FMC_A8_Pin GPIO_PIN_14
#define FMC_A8_GPIO_Port GPIOF
#define FMC_A9_Pin GPIO_PIN_15
#define FMC_A9_GPIO_Port GPIOF
#define FMC_D4_Pin GPIO_PIN_7
#define FMC_D4_GPIO_Port GPIOE
#define FMC_D5_Pin GPIO_PIN_8
#define FMC_D5_GPIO_Port GPIOE
#define FMC_D6_Pin GPIO_PIN_9
#define FMC_D6_GPIO_Port GPIOE
#define FMC_D7_Pin GPIO_PIN_10
#define FMC_D7_GPIO_Port GPIOE
#define FMC_D8_Pin GPIO_PIN_11
#define FMC_D8_GPIO_Port GPIOE
#define FMC_D9_Pin GPIO_PIN_12
#define FMC_D9_GPIO_Port GPIOE
#define FMC_D10_Pin GPIO_PIN_13
#define FMC_D10_GPIO_Port GPIOE
#define FMC_D11_Pin GPIO_PIN_14
#define FMC_D11_GPIO_Port GPIOE
#define FMC_D12_Pin GPIO_PIN_15
#define FMC_D12_GPIO_Port GPIOE
#define OPAMP4_VINM_Pin GPIO_PIN_10
#define OPAMP4_VINM_GPIO_Port GPIOB
#define OPAMP4_COMP6_INP_Pin GPIO_PIN_11
#define OPAMP4_COMP6_INP_GPIO_Port GPIOB
#define OPAMP4_VOUT_Pin GPIO_PIN_12
#define OPAMP4_VOUT_GPIO_Port GPIOB
#define FDCAN2_TX_PD_CC1_Pin GPIO_PIN_13
#define FDCAN2_TX_PD_CC1_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define FMC_D13_Pin GPIO_PIN_8
#define FMC_D13_GPIO_Port GPIOD
#define FMC_D14_Pin GPIO_PIN_9
#define FMC_D14_GPIO_Port GPIOD
#define FMC_D15_Pin GPIO_PIN_10
#define FMC_D15_GPIO_Port GPIOD
#define FMC_A16_Pin GPIO_PIN_11
#define FMC_A16_GPIO_Port GPIOD
#define FMC_A17_Pin GPIO_PIN_12
#define FMC_A17_GPIO_Port GPIOD
#define FMC_A18_Pin GPIO_PIN_13
#define FMC_A18_GPIO_Port GPIOD
#define FMC_D0_Pin GPIO_PIN_14
#define FMC_D0_GPIO_Port GPIOD
#define FMC_D1_Pin GPIO_PIN_15
#define FMC_D1_GPIO_Port GPIOD
#define CMOP6_OUT_Pin GPIO_PIN_6
#define CMOP6_OUT_GPIO_Port GPIOC
#define MFX_WAKEUP_Pin GPIO_PIN_7
#define MFX_WAKEUP_GPIO_Port GPIOC
#define FMC_A10_Pin GPIO_PIN_0
#define FMC_A10_GPIO_Port GPIOG
#define FMC_A11_Pin GPIO_PIN_1
#define FMC_A11_GPIO_Port GPIOG
#define FMC_A12_Pin GPIO_PIN_2
#define FMC_A12_GPIO_Port GPIOG
#define FMC_A13_Pin GPIO_PIN_3
#define FMC_A13_GPIO_Port GPIOG
#define FMC_A14_Pin GPIO_PIN_4
#define FMC_A14_GPIO_Port GPIOG
#define BK_Drive_Pin GPIO_PIN_8
#define BK_Drive_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_9
#define LCD_CS_GPIO_Port GPIOC
#define SAI_SCK_A_Pin GPIO_PIN_8
#define SAI_SCK_A_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define JTMS_SWDIO_Pin GPIO_PIN_13
#define JTMS_SWDIO_GPIO_Port GPIOA
#define Audio_INT_Pin GPIO_PIN_6
#define Audio_INT_GPIO_Port GPIOF
#define JTCK_SWCLK_Pin GPIO_PIN_14
#define JTCK_SWCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SmartCard_IO_Pin GPIO_PIN_10
#define SmartCard_IO_GPIO_Port GPIOC
#define SOURCE_EN_Pin GPIO_PIN_11
#define SOURCE_EN_GPIO_Port GPIOC
#define SmartCard_CLK_Pin GPIO_PIN_12
#define SmartCard_CLK_GPIO_Port GPIOC
#define FMC_A15_Pin GPIO_PIN_5
#define FMC_A15_GPIO_Port GPIOG
#define I2C3_SMBA_Pin GPIO_PIN_6
#define I2C3_SMBA_GPIO_Port GPIOG
#define I2C3_SCL_Pin GPIO_PIN_7
#define I2C3_SCL_GPIO_Port GPIOG
#define I2C3_SDA_Pin GPIO_PIN_8
#define I2C3_SDA_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOG
#define FMC_D2_Pin GPIO_PIN_0
#define FMC_D2_GPIO_Port GPIOD
#define FMC_D3_Pin GPIO_PIN_1
#define FMC_D3_GPIO_Port GPIOD
#define RS485_DIR_Pin GPIO_PIN_2
#define RS485_DIR_GPIO_Port GPIOD
#define FMC_NOE_Pin GPIO_PIN_4
#define FMC_NOE_GPIO_Port GPIOD
#define FMC_NWE_Pin GPIO_PIN_5
#define FMC_NWE_GPIO_Port GPIOD
#define SAI_SD_A_Pin GPIO_PIN_6
#define SAI_SD_A_GPIO_Port GPIOD
#define FMC_NE1_Pin GPIO_PIN_7
#define FMC_NE1_GPIO_Port GPIOD
#define JTDO_SWO_Pin GPIO_PIN_3
#define JTDO_SWO_GPIO_Port GPIOB
#define JTRST_PD_CC2_Pin GPIO_PIN_4
#define JTRST_PD_CC2_GPIO_Port GPIOB
#define FDCAN2_RX_Pin GPIO_PIN_5
#define FDCAN2_RX_GPIO_Port GPIOB
#define BOOT0_FDCAN1_RX_Pin GPIO_PIN_8
#define BOOT0_FDCAN1_RX_GPIO_Port GPIOB
#define FDCAN1_TX_Pin GPIO_PIN_9
#define FDCAN1_TX_GPIO_Port GPIOB
#define FMC_NBL0_Pin GPIO_PIN_0
#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_NBL1_Pin GPIO_PIN_1
#define FMC_NBL1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
extern void app_main (void *arg);

extern uint64_t app_main_stk[];
extern const osThreadAttr_t app_main_attr;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
