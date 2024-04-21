/**
  ******************************************************************************
  * @file    stm32g4xx_nucleo.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for:
  *          - LEDs and push-button available on STM32G4xx-Nucleo Kit
  *            from STMicroelectronics
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32G4XX_NUCLEO_H
#define STM32G4XX_NUCLEO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_nucleo_errno.h"
#include "stm32g4xx_nucleo_conf.h"

#if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #ifndef __GNUC__
      #include "stdio.h"
    #endif
  #endif
#endif

#if !defined (USE_NUCLEO_32) && !defined (USE_NUCLEO_64)
#error "Board Pin number not defined"
#endif

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32G4XX_NUCLEO STM32G4XX_NUCLEO
  * @{
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */
typedef enum
{
  LED2 = 0U,
  LED_GREEN = LED2,
  LEDn
}Led_TypeDef;

#if defined (USE_NUCLEO_64)
typedef enum
{
  BUTTON_USER = 0U,
  BUTTONn
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
}ButtonMode_TypeDef;
#endif /* USE_NUCLEO_64 */

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn
}COM_TypeDef;

typedef enum
{
  COM_STOPBITS_1     =   UART_STOPBITS_1,
  COM_STOPBITS_2     =   UART_STOPBITS_2,
}COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE     =  UART_PARITY_NONE,
  COM_PARITY_EVEN     =  UART_PARITY_EVEN,
  COM_PARITY_ODD      =  UART_PARITY_ODD,
}COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
}COM_HwFlowCtlTypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B,
}COM_WordLengthTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
}COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  pUART_CallbackTypeDef  pMspInitCb;
  pUART_CallbackTypeDef  pMspDeInitCb;
}BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif /* (USE_BSP_COM_FEATURE > 0) */

/**
  * @}
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */

/**
  * @brief STM32G4XX_NUCLEO BSP Driver version number
  */
#define STM32G4XX_NUCLEO_BSP_VERSION_MAIN   0x01U /*!< [31:24] main version */
#define STM32G4XX_NUCLEO_BSP_VERSION_SUB1   0x00U /*!< [23:16] sub1 version */
#define STM32G4XX_NUCLEO_BSP_VERSION_SUB2   0x00U /*!< [15:8]  sub2 version */
#define STM32G4XX_NUCLEO_BSP_VERSION_RC     0x00U /*!< [7:0]  release candidate */
#define STM32G4XX_NUCLEO_BSP_VERSION        ((STM32G4XX_NUCLEO_BSP_VERSION_MAIN << 24)\
                                            |(STM32G4XX_NUCLEO_BSP_VERSION_SUB1 << 16)\
                                            |(STM32G4XX_NUCLEO_BSP_VERSION_SUB2 << 8 )\
                                            |(STM32G4XX_NUCLEO_BSP_VERSION_RC))

/**
  * @brief Define for STM32G4XX_NUCLEO board
  */
#if !defined (USE_STM32G4XX_NUCLEO)
 #define USE_STM32G4XX_NUCLEO
#endif

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_LED LOW LEVEL LED
  * @{
  */
#if defined (USE_NUCLEO_32)
#define LED2_PIN                              GPIO_PIN_8
#define LED2_GPIO_PORT                        GPIOB
#define LED2_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOB_CLK_DISABLE()

#elif defined (USE_NUCLEO_64)
#define LED2_PIN                              GPIO_PIN_5
#define LED2_GPIO_PORT                        GPIOA
#define LED2_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOA_CLK_DISABLE()
#endif /* USE_NUCLEO_32 */

/**
  * @}
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_BUTTON LOW LEVEL BUTTON
  * @{
  */
#if defined (USE_NUCLEO_64)
/* Button state */
#define BUTTON_RELEASED                       0U
#define BUTTON_PRESSED                        1U

/**
  * @brief User push-button
  */
#define USER_BUTTON_PIN                       GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                 GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn                 EXTI15_10_IRQn
#define USER_BUTTON_EXTI_LINE                 EXTI_LINE_13

#endif /* USE_NUCLEO_64 */
/**
  * @}
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_COM LOW LEVEL COM
  * @{
  */
/**
 * @brief Definition for COM port1, connected to LPUART1
 */
#if (USE_BSP_COM_FEATURE > 0)
#define COM1_UART                             LPUART1
#define COM1_CLK_ENABLE()                     __HAL_RCC_LPUART1_CLK_ENABLE()
#define COM1_CLK_DISABLE()                    __HAL_RCC_LPUART1_CLK_DISABLE()

#define COM1_TX_PIN                           GPIO_PIN_2
#define COM1_TX_GPIO_PORT                     GPIOA
#define COM1_TX_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_TX_AF                            GPIO_AF12_LPUART1

#define COM1_RX_PIN                           GPIO_PIN_3
#define COM1_RX_GPIO_PORT                     GPIOA
#define COM1_RX_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_RX_AF                            GPIO_AF12_LPUART1
#define COM_POLL_TIMEOUT                      1000

#define MX_UART_InitTypeDef COM_InitTypeDef
#endif /* (USE_BSP_COM_FEATURE > 0)*/
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32G4XX_NUCLEO_LOW_LEVEL_Exported_Variables
  * @{
  */
#if defined (USE_NUCLEO_64)
extern EXTI_HandleTypeDef hpb_exti[];
#endif /* USE_NUCLEO_64 */
#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[];
#endif /* (USE_BSP_COM_FEATURE > 0) */
/**
  * @}
  */

/** @defgroup STM32G4XX_NUCLEO_LOW_LEVEL_Exported_Functions LOW LEVEL Exported Functions
  * @{
  */
int32_t  BSP_GetVersion(void);
int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState (Led_TypeDef Led);
#if defined (USE_NUCLEO_64)
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);
void     BSP_PB_IRQHandler(Button_TypeDef Button);
#endif /* USE_NUCLEO_64 */
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort (COM_TypeDef COM);
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t  BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t  BSP_COM_RegisterMspCallbacks(COM_TypeDef COM , BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_LPUART1_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init);
#endif /* (USE_BSP_COM_FEATURE > 0) */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32G4XX_NUCLEO_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
