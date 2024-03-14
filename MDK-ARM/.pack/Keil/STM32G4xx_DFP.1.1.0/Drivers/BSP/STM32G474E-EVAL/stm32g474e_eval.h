/**
  ******************************************************************************
  * @file    stm32g474e_eval.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32G474E-EVAL:
  *          LEDs
  *          Push-buttons
  *          Joystick
  *          POTentiometer
  *          COM ports
  *          Hardware resources.
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
#ifndef STM32G474E_EVAL_H
#define STM32G474E_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"
#if (USE_BSP_IO_CLASS > 0)
#include "stm32g474e_eval_io.h"
#endif

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_Common
  * @{
  */

/** @defgroup STM32G474E-EVAL_Exported_Types Exported Types
  * @{
  */

/**
  * @brief Define for STM32G474E-EVAL board
  */

/**
 * @brief STM32G474E-EVAL BSP Driver version number V2.0.0
 */
#define	STM32G474E_EVAL_BSP_VERSION_MAIN   (uint32_t)(0x01) /*!< [31:24] main version */
#define	STM32G474E_EVAL_BSP_VERSION_SUB1   (uint32_t)(0x00) /*!< [23:16] sub1 version */
#define	STM32G474E_EVAL_BSP_VERSION_SUB2   (uint32_t)(0x00) /*!< [15:8]  sub2 version */
#define	STM32G474E_EVAL_BSP_VERSION_RC     (uint32_t)(0x00) /*!< [7:0]  release candidate */
#define	STM32G474E_EVAL_BSP_VERSION        ((STM32G474E_EVAL_BSP_VERSION_MAIN << 24)\
                                            |(STM32G474E_EVAL_BSP_VERSION_SUB1 << 16)\
                                            |(STM32G474E_EVAL_BSP_VERSION_SUB2 << 8 )\
                                            |(STM32G474E_EVAL_BSP_VERSION_RC))

#if !defined (USE_STM32G4XX_EVAL)
 #define USE_STM32G4XX_EVAL
#endif
#ifndef USE_BSP_COM_FEATURE
   #define USE_BSP_COM_FEATURE                  0U
#endif

#ifndef USE_COM_LOG
  #define USE_COM_LOG                           0U
#endif

#ifndef USE_BSP_POT_FEATURE
  #define USE_BSP_POT_FEATURE                   0U
#endif

#ifndef USE_BSP_POT_COMP_FEATURE
  #define USE_BSP_POT_COMP_FEATURE              0U
#endif

#ifndef USE_BSP_JOY_FEATURE
   #define USE_BSP_JOY_FEATURE                  1U
#endif

#ifndef BSP_BUTTON_USER_IT_PRIORITY
  #define BSP_BUTTON_USER_IT_PRIORITY          15U
#endif

typedef enum
{
  LED1 = 0U,
  LED_GREEN = LED1,
  LED2 = 1U,
  LED_ORANGE = LED2,
  LED3 = 2U,
  LED_RED = LED3,
  LED4 = 3U,
  LED_BLUE = LED4,
  LED_BUCK = 4U,
}Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0U,
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
}ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn,
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

typedef struct
{
  uint32_t             BaudRate;
  uint32_t             WordLength;
  COM_StopBitsTypeDef  StopBits;
  COM_ParityTypeDef    Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
}COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
}BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif /* (USE_BSP_COM_FEATURE > 0) */

typedef enum
{
  POT1 = 0U,
  POTn,
}POT_TypeDef;

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1) || (USE_HAL_COMP_REGISTER_CALLBACKS == 1) || (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
typedef struct
{
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
  pOPAMP_CallbackTypeDef  pMspOPAMPInitCb;
  pOPAMP_CallbackTypeDef  pMspOPAMPDeInitCb;
#endif /* (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1) */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
  pADC_CallbackTypeDef    pMspADCInitCb;
  pADC_CallbackTypeDef    pMspADCDeInitCb;
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 1) */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
#if (USE_BSP_POT_COMP_FEATURE > 0)
  pCOMP_CallbackTypeDef   pMspCOMPInitCb;
  pCOMP_CallbackTypeDef   pMspCOMPDeInitCb;
#endif /* (USE_BSP_POT_COMP_FEATURE > 0)*/
#endif /* (USE_HAL_COMP_REGISTER_CALLBACKS == 1) */
}BSP_POT_Cb_t;
#endif /* USE_HAL_XXX_REGISTER_CALLBACKS */

#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
typedef enum
{
  JOY_NONE  = 0x00,
  JOY_SEL   = 0x01,
  JOY_DOWN  = 0x02,
  JOY_LEFT  = 0x04,
  JOY_RIGHT = 0x08,
  JOY_UP    = 0x10,
  JOY_ALL   = 0x1F,
} JOYPin_TypeDef;

typedef enum
{
  JOY_MODE_GPIO = 0U,
  JOY_MODE_EXTI = 1U
}JOYMode_TypeDef;

typedef enum
{
  JOY1 = 0U,
  JOYn,
}JOY_TypeDef;
#endif/* USE_BSP_JOY_FEATURE */
#endif/* USE_BSP_IO_CLASS */

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_Exported_Constants Exported Constants
  * @{
  */
#if defined(_GUI_INTERFACE)
const uint8_t HWBoardVersionName[]  = "STM32G474E-EVAL";
const uint8_t PDTypeName[]          = "MB1397B";
#endif /* _GUI_INTERFACE */

/** @defgroup STM32G474E-EVAL_LED LED Constants
  * @{
  */
#define LEDn                            5U

#define LED1_GPIO_PORT                   GPIOG
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOG_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOG_CLK_DISABLE()
#define LED1_PIN                         GPIO_PIN_9
#define LED2_PIN                         IO_PIN_6
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()
#define LED3_PIN                         GPIO_PIN_11
#define LED4_PIN                         IO_PIN_7

#define LED_BUCK_GPIO_PORT              GPIOC
#define LED_BUCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_BUCK_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED_BUCK_PIN                    GPIO_PIN_8
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_BUTTON BUTTON Constants
  * @{
  */
/* Button state */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
#define BUTTONn                            1U

/**
 * @brief Key Push-button
 */
#define USER_BUTTON_PIN                       GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                 GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn                 EXTI15_10_IRQn
#define USER_BUTTON_EXTI_LINE                 EXTI_LINE_13
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_COM COM Constants
  * @{
  */
#if (USE_BSP_COM_FEATURE > 0)

/**
 * @brief Definition for COM port1, connected to USART1
 */
#define COM1_UART                     USART1
#define COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define COM1_TX_PIN                   GPIO_PIN_9
#define COM1_TX_GPIO_PORT             GPIOA
#define COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_TX_AF                    GPIO_AF7_USART1

#define COM1_RX_PIN                   GPIO_PIN_10
#define COM1_RX_GPIO_PORT             GPIOA
#define COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_RX_AF                    GPIO_AF7_USART1
#define COM_POLL_TIMEOUT              1000

#define MX_UART_InitTypeDef           COM_InitTypeDef

#endif /* (USE_BSP_COM_FEATURE > 0) */
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_POT POT Constants
  * @{
  */
#if (USE_BSP_POT_FEATURE > 0)

/**
 * @brief Definition for Potentiometer, connected to OpAmp4/ADC5 channel 5 or COMP6
 */
#if (USE_BSP_POT_COMP_FEATURE > 0)
/* Definition for COMP6 Channel Pin */
#define POT1_OUT_GPIO_PIN            GPIO_PIN_6
#define POT1_OUT_GPIO_PORT           GPIOC
#endif /* USE_BSP_POT_COMP_FEATURE > 0)*/
/* Definition for OpAmp4/ADCx Channel Pin */

#define POT1_CLK_ENABLE()            do { __HAL_RCC_ADC345_CLK_ENABLE();\
                                          __HAL_RCC_SYSCFG_CLK_ENABLE();\
                                     } while(0)

#define POT1_CLK_DISABLE()           do { __HAL_RCC_ADC345_CLK_DISABLE();\
                                     } while(0)

#define POT1_FORCE_RESET()           __HAL_RCC_ADC345_FORCE_RESET()
#define POT1_RELEASE_RESET()         __HAL_RCC_ADC345_RELEASE_RESET()
#define POT1_GPIO_CLK_ENABLE()       do { __HAL_RCC_GPIOB_CLK_ENABLE();\
                                          __HAL_RCC_GPIOC_CLK_ENABLE();\
                                     }while(0)

#define POT1_VINP_GPIO_PIN           GPIO_PIN_11
#define POT1_VINP_GPIO_PORT          GPIOB
#define POT1_VOUT_GPIO_PIN           GPIO_PIN_12
#define POT1_VOUT_GPIO_PORT          GPIOB
#define POT1_INM_GPIO_PIN            GPIO_PIN_10
#define POT1_INM_GPIO_PORT           GPIOB

/* Definition for ADCx's Channel */
#define POT1_ADC_CHANNEL             ADC_CHANNEL_VOPAMP4
#define POT1_SAMPLING_TIME           ADC_SAMPLETIME_47CYCLES_5
#define POT1_PRESCALER               ADC_CLOCKPRESCALER_PCLK_DIV4
#define POT_ADC_POLL_TIMEOUT         1000U
#define POT_CONVERT2PERC(x)          (((x) * 100UL)/0xFFFUL)

#endif /* USE_BSP_POT_FEATURE > 0 */
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_JOYSTICK JOYSTICK Constants
  * @{
  */
#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
#define JOY1_EXTI_LINE                  EXTI_LINE_0

#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */
/**
  * @}
  */
  
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_Exported_Variables
  * @{
  */
extern EXTI_HandleTypeDef hpb_exti[BUTTONn];
#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
extern EXTI_HandleTypeDef hjoy_exti[JOYn];
#endif /* USE_BSP_IO_CLASS > 0)    */
#endif /* USE_BSP_JOY_FEATURE > 0) */

#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[COMn];
#endif /* #if (USE_BSP_COM_FEATURE > 0) */

#if (USE_BSP_POT_FEATURE > 0)
extern OPAMP_HandleTypeDef hpot_opamp[POTn];
extern ADC_HandleTypeDef hpot_adc[POTn];
#if (USE_BSP_POT_COMP_FEATURE > 0)
extern COMP_HandleTypeDef hpot_comp[POTn];
#endif /* (USE_BSP_POT_COMP_FEATURE > 0)*/
#endif  /* (USE_BSP_POT_FEATURE > 0) */
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_Exported_Functions Exported Functions
  * @{
  */
int32_t  BSP_GetVersion(void);
#if defined(_GUI_INTERFACE)
const uint8_t* BSP_GetHWBoardVersionName(void);
const uint8_t* BSP_GetPDTypeName(void);
#endif /* _GUI_INTERFACE */
int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState (Led_TypeDef Led);
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);
void     BSP_PB_IRQHandler(Button_TypeDef Button);
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if( USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort (COM_TypeDef COM);
#endif /* USE_COM_LOG */
HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef *huart, COM_InitTypeDef *COM_Init);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#endif /* USE_BSP_COM_FEATURE */

#if (USE_BSP_POT_FEATURE > 0)
int32_t BSP_POT_Init(POT_TypeDef POT);
int32_t BSP_POT_DeInit(POT_TypeDef POT);
int32_t BSP_POT_GetLevel(POT_TypeDef POT);
#if (USE_BSP_POT_COMP_FEATURE > 0)
HAL_StatusTypeDef MX_COMP6_Init(COMP_HandleTypeDef *comp);
HAL_StatusTypeDef MX_COMP6_DeInit(COMP_HandleTypeDef *comp);
#endif  /* (USE_BSP_POT_COMP_FEATURE > 0) */
HAL_StatusTypeDef MX_ADC5_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef MX_ADC5_DeInit(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef MX_OPAMP4_Init(OPAMP_HandleTypeDef *opamp);
HAL_StatusTypeDef MX_OPAMP4_DeInit(OPAMP_HandleTypeDef *opamp);
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1) || (USE_HAL_COMP_REGISTER_CALLBACKS == 1) || (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
int32_t BSP_POT_RegisterDefaultMspCallbacks(POT_TypeDef POT);
int32_t BSP_POT_RegisterMspCallbacks(POT_TypeDef POT, BSP_POT_Cb_t *Callback);
#endif /* USE_HAL_XXX_REGISTER_CALLBACKS */
#endif  /* (USE_BSP_POT_FEATURE > 0) */

#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
int32_t  BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_GetState(JOY_TypeDef JOY);
void     BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
void     BSP_JOY_IRQHandler(void);
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */
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
#endif /* STM32G474E_EVAL_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
