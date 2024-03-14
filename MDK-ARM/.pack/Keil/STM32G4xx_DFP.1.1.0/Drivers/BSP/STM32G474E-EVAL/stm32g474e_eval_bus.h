/**
  ******************************************************************************
  * @file    stm32g474e_eval_bus.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32G474E-EVAL LEDs,
  *          push-buttons hardware resources.
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
#ifndef STM32G474E_EVAL_BUS_H
#define STM32G474E_EVAL_BUS_H

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_BUS
  * @{
  */

/** @defgroup STM32G474E-EVAL_BUS_Exported_Constants Exported Constants
  * @{
  */
/* I2C3 BUS definitions */
#define BUS_I2C3_INSTANCE                 I2C3

#ifndef BUS_I2C3_FREQUENCY
   #define BUS_I2C3_FREQUENCY             100000U /* Frequency of I2Cn = 100 kHz*/
#endif

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated from APB1 source clock = 50 MHz */
/* Due to the big MOFSET capacity for adapting the camera level the rising time is very large (>1us) */
/* 0x40912732 takes in account the big rising and aims a clock of 100khz */
#ifndef EVAL_I2Cx_TIMING
#define EVAL_I2Cx_TIMING                      ((uint32_t)0x40912732)
#endif /* EVAL_I2Cx_TIMING */

#define BUS_I2C3_CLK_ENABLE()             __HAL_RCC_I2C3_CLK_ENABLE()
#define BUS_I2C3_CLK_DISABLE()            __HAL_RCC_I2C3_CLK_DISABLE()
#define BUS_I2C3_FORCE_RESET()            __HAL_RCC_I2C3_FORCE_RESET()
#define BUS_I2C3_RELEASE_RESET()          __HAL_RCC_I2C3_RELEASE_RESET()

#define BUS_I2C3_SCL_GPIO_PIN             GPIO_PIN_7
#define BUS_I2C3_SCL_GPIO_PORT            GPIOG
#define BUS_I2C3_SCL_GPIO_AF              GPIO_AF4_I2C3
#define BUS_I2C3_SCL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_I2C3_SDA_GPIO_PIN             GPIO_PIN_8
#define BUS_I2C3_SDA_GPIO_PORT            GPIOG
#define BUS_I2C3_SDA_GPIO_AF              GPIO_AF4_I2C3
#define BUS_I2C3_SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOG_CLK_ENABLE()

#ifndef BUS_I2C3_POLL_TIMEOUT
   #define BUS_I2C3_POLL_TIMEOUT          0x1000U
#endif

/* SPI2 BUS definitions */
#define BUS_SPI2_INSTANCE                 SPI2

#ifndef BUS_SPI2_BAUDRATE
   #define BUS_SPI2_BAUDRATE  12500000    /* baud rate of SPIn = 12.5 Mbps*/
#endif

#ifndef BUS_SPI2_POLL_TIMEOUT
  #define BUS_SPI2_POLL_TIMEOUT            0x1000U
#endif

/* SCK GPIO */
#define BUS_SPI2_SCK_CLK_ENABLE  __HAL_RCC_GPIOF_CLK_ENABLE
#define BUS_SPI2_SCK_GPIO        GPIOF
#define BUS_SPI2_SCK_PIN         GPIO_PIN_9
#define BUS_SPI2_SCK_AF          GPIO_AF5_SPI2

/* MOSI GPIO */
#define BUS_SPI2_MOSI_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define BUS_SPI2_MOSI_GPIO       GPIOB
#define BUS_SPI2_MOSI_PIN        GPIO_PIN_15
#define BUS_SPI2_MOSI_AF         GPIO_AF5_SPI2

/* MISO GPIO */
#define BUS_SPI2_MISO_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define BUS_SPI2_MISO_GPIO       GPIOB
#define BUS_SPI2_MISO_PIN        GPIO_PIN_14
#define BUS_SPI2_MISO_AF         GPIO_AF5_SPI2

/* DIR GPIO */
#define BUS_SPI2_DIR_PIN         IO_PIN_13

#define BUS_SPI2_START_BYTE         0x70U
#define BUS_SPI2_SET_INDEX          0x00U
#define BUS_SPI2_READ_STATUS        0x01U
#define BUS_SPI2_WRITE_REG          0x02U
#define BUS_SPI2_READ_REG           0x03U

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
}BSP_SPI2_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_BUS_Private_Types Private Types
  * @{
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspInitCb;
  pSPI_CallbackTypeDef  pMspDeInitCb;
}BSP_SPI_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */


#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspInitCb;
  pI2C_CallbackTypeDef  pMspDeInitCb;
}BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_BUS_Exported_Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c3;
extern SPI_HandleTypeDef hbus_spi2;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_BUS_Exported_Functions Exported Functions
  * @{
  */
int32_t BSP_I2C3_Init(void);
int32_t BSP_I2C3_DeInit(void);
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C3_RegisterDefaultMspCallbacks (void);
int32_t BSP_I2C3_RegisterMspCallbacks (BSP_I2C3_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *phi2c, uint32_t timing);

int32_t BSP_SPI2_Init(void);
int32_t BSP_SPI2_DeInit(void);
int32_t BSP_SPI2_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI2_Recv(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI2_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI2_RegisterDefaultMspCallbacks (void);
int32_t BSP_SPI2_RegisterMspCallbacks (BSP_SPI2_Cb_t *Callbacks);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

HAL_StatusTypeDef MX_SPI2_Init(SPI_HandleTypeDef* phspi, uint32_t BaudratePrescaler);
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

#endif /* STM32G474E_EVAL_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
