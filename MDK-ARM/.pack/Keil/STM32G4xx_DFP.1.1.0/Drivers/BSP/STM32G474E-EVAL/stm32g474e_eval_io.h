/**
  ******************************************************************************
  * @file    stm32g474e_eval_io.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32g474e_eval_io.c driver.
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
#ifndef STM32G474E_EVAL_IO_H
#define STM32G474E_EVAL_IO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"
/* Include common IO driver */
#include "../Components/Common/io.h"
#include "../Components/Common/idd.h"
/* Include IO component driver */
#include "../Components/mfxstm32l152/mfxstm32l152.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_IO
  * @{
  */

/** @defgroup STM32G474E-EVAL_IO_Exported_Types Exported Types
  * @{
  */
typedef struct
{
  uint32_t                    IsInitialized;     /*   IO_IsInitialized    */
  uint32_t                    Functions;         /*   Selected functions  */
}IOEXPANDER_Ctx_t;

typedef struct
{
  uint32_t Pin;       /*!< Specifies the IO pins to be configured */
  uint32_t Mode;      /*!< Specifies the operating mode for the selected pins */
  uint32_t Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins */
}BSP_IO_Init_t;

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_IO_Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief IOExpander modes
  */
#ifndef USE_BSP_IO_CLASS
   #define USE_BSP_IO_CLASS       1U
#endif

#define IOEXPANDER_IO_MODE        1U
#define IOEXPANDER_IDD_MODE       2U
#define IOEXPANDER_TS_MODE        4U /* Not used */

/**
  * @brief IOExpander instances number
  */
#define IOEXPANDER_INSTANCES_NBR  1U

/**
  * @brief IOExpander pins control
  */
#define IO_PIN_RESET              0U
#define IO_PIN_SET                1U

#define IO_PIN_IT_RESET           0U
#define IO_PIN_IT_SET             1U

/**
  * @brief IOExpander IOs definition
  */
#define IO_PIN_0        MFXSTM32L152_GPIO_PIN_0
#define IO_PIN_1        MFXSTM32L152_GPIO_PIN_1
#define IO_PIN_2        MFXSTM32L152_GPIO_PIN_2
#define IO_PIN_3        MFXSTM32L152_GPIO_PIN_3
#define IO_PIN_4        MFXSTM32L152_GPIO_PIN_4
#define IO_PIN_5        MFXSTM32L152_GPIO_PIN_5
#define IO_PIN_6        MFXSTM32L152_GPIO_PIN_6
#define IO_PIN_7        MFXSTM32L152_GPIO_PIN_7
#define IO_PIN_8        MFXSTM32L152_GPIO_PIN_8
#define IO_PIN_9        MFXSTM32L152_GPIO_PIN_9
#define IO_PIN_10       MFXSTM32L152_GPIO_PIN_10
#define IO_PIN_11       MFXSTM32L152_GPIO_PIN_11
#define IO_PIN_12       MFXSTM32L152_GPIO_PIN_12
#define IO_PIN_13       MFXSTM32L152_GPIO_PIN_13
#define IO_PIN_14       MFXSTM32L152_GPIO_PIN_14
#define IO_PIN_15       MFXSTM32L152_GPIO_PIN_15
#define IO_PIN_16       MFXSTM32L152_GPIO_PIN_16
#define IO_PIN_17       MFXSTM32L152_GPIO_PIN_17
#define IO_PIN_18       MFXSTM32L152_GPIO_PIN_18
#define IO_PIN_19       MFXSTM32L152_GPIO_PIN_19
#define IO_PIN_20       MFXSTM32L152_GPIO_PIN_20
#define IO_PIN_21       MFXSTM32L152_GPIO_PIN_21
#define IO_PIN_22       MFXSTM32L152_GPIO_PIN_22
#define IO_PIN_23       MFXSTM32L152_GPIO_PIN_23
#define IO_PIN_ALL      MFXSTM32L152_GPIO_PINS_ALL
#define AGPIO_PIN_0     MFXSTM32L152_AGPIO_PIN_0
#define AGPIO_PIN_1     MFXSTM32L152_AGPIO_PIN_1

/**
  * @brief IOExpander I2C device address
  */
#define IO_I2C_ADDRESS            0x84U
#define IO_I2C_ADDRESS_2          0x86U

/**
  * @brief IOExpander IOs pull define
  */
#define IO_NOPULL                 MFXSTM32L152_GPIO_NOPULL
#define IO_PULLUP                 MFXSTM32L152_GPIO_PULLUP
#define IO_PULLDOWN               MFXSTM32L152_GPIO_PULLDOWN

/**
  * @brief IOExpander IOs mode define
  */
#define IO_MODE_OFF               MFXSTM32L152_GPIO_MODE_OFF
#define IO_MODE_ANALOG            MFXSTM32L152_GPIO_MODE_ANALOG
#define IO_MODE_INPUT             MFXSTM32L152_GPIO_MODE_INPUT
#define IO_MODE_OUTPUT_OD         MFXSTM32L152_GPIO_MODE_OUTPUT_OD
#define IO_MODE_OUTPUT_PP         MFXSTM32L152_GPIO_MODE_OUTPUT_PP
#define IO_MODE_IT_RISING_EDGE    MFXSTM32L152_GPIO_MODE_IT_RISING_EDGE
#define IO_MODE_IT_FALLING_EDGE   MFXSTM32L152_GPIO_MODE_IT_FALLING_EDGE
#define IO_MODE_IT_LOW_LEVEL      MFXSTM32L152_GPIO_MODE_IT_LOW_LEVEL
#define IO_MODE_IT_HIGH_LEVEL     MFXSTM32L152_GPIO_MODE_IT_HIGH_LEVEL

/**
  * @brief IOExpander_IRQOUT pin
  */
#define IOEXPANDER_IRQOUT_GPIO_PIN               GPIO_PIN_0
#define IOEXPANDER_IRQOUT_GPIO_PORT              GPIOA
#define IOEXPANDER_IRQOUT_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define IOEXPANDER_IRQOUT_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE()
#define IOEXPANDER_IRQOUT_EXTI_IRQn              EXTI0_IRQn
#define IOEXPANDER_IRQOUT_EXTI_IRQnHandler       EXTI0_IRQHandler
#define IOEXPANDER_IRQOUT_EXTI_LINE              EXTI_LINE_0

#define IOEXPANDER_WAKEUP_GPIO_PORT              GPIOC
#define IOEXPANDER_WAKEUP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define IOEXPANDER_WAKEUP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOC_CLK_DISABLE()
#define IOEXPANDER_WAKEUP_PIN                    GPIO_PIN_7
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_IO_Exported_Variables
  * @{
  */
extern IOEXPANDER_Ctx_t                IOECtx[IOEXPANDER_INSTANCES_NBR];
extern MFXSTM32L152_Object_t           IOECompObj[IOEXPANDER_INSTANCES_NBR];
extern IDD_Drv_t                       *IddDrv[IOEXPANDER_INSTANCES_NBR];
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_IO_Exported_Functions Exported Functions
  * @{
  */
int32_t BSP_IO_Init(uint32_t Instance, BSP_IO_Init_t *Init);
int32_t BSP_IO_DeInit(uint32_t Instance);

int32_t BSP_IO_GetIT(uint32_t Instance, uint32_t Pins);
int32_t BSP_IO_ClearIT(uint32_t Instance, uint32_t Pins);

int32_t BSP_IO_WritePin(uint32_t Instance, uint32_t Pin, uint32_t PinState);
int32_t BSP_IO_ReadPin(uint32_t Instance, uint32_t Pin);
int32_t BSP_IO_TogglePin(uint32_t Instance, uint32_t Pin);

int32_t BSP_IOEXPANDER_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_IOEXPANDER_DeInit(uint32_t Instance);
void    BSP_IOEXPANDER_ITConfig(void);
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
#endif /* STM32G474E_EVAL_IO_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
