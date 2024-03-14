/**
  ******************************************************************************
  * @file    stm32g474e_eval_smartcard.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32G474E-EVAL SmartCard,
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef STM32G474E_EVAL_SMARTCARD_H
#define STM32G474E_EVAL_SMARTCARD_H

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_SMARTCARD
  * @{
  */

/** @defgroup STM32G474E-EVAL_SMARTCARD_Exported_Constants SMARTCARD Exported Constants
  * @{
  */

/**
  * @brief SMARTCARD constant
  */
#define T0_PROTOCOL         0x00  /* T0 protocol */
#define DIRECT              0x3B  /* Direct bit convention */
#define INDIRECT            0x3F  /* Indirect bit convention */
#define SETUP_LENGTH        20
#define HIST_LENGTH         20
#define LC_MAX              256
#define ICCID_LENGTH        10

/* SMARTCARD ADPU Command: Operation Code -------------------------------------------*/
#define SMARTCARD_CLA_GSM11       0xA0

/*------------------------ Data Area Management Commands ---------------------*/
#define SMARTCARD_SELECT_FILE     0xA4
#define SMARTCARD_GET_RESPONSE    0xC0
#define SMARTCARD_STATUS          0xF2
#define SMARTCARD_UPDATE_BINARY   0xD6
#define SMARTCARD_READ_BINARY     0xB0
#define SMARTCARD_WRITE_BINARY    0xD0
#define SMARTCARD_UPDATE_RECORD   0xDC
#define SMARTCARD_READ_RECORD     0xB2

/*-------------------------- Administrative Commands -------------------------*/
#define SMARTCARD_CREATE_FILE     0xE0

/*-------------------------- Safety Management Commands ----------------------*/
#define SMARTCARD_VERIFY          0x20
#define SMARTCARD_CHANGE          0x24
#define SMARTCARD_DISABLE         0x26
#define SMARTCARD_ENABLE          0x28
#define SMARTCARD_UNBLOCK         0x2C
#define SMARTCARD_EXTERNAL_AUTH   0x82
#define SMARTCARD_GET_CHALLENGE   0x84

/*-------------------------- Answer to reset Commands ------------------------*/
#define SMARTCARD_GET_A2R         0x00

/* SMARTCARD STATUS: Status Code ----------------------------------------------------*/
#define SMARTCARD_EF_SELECTED     0x9F
#define SMARTCARD_DF_SELECTED     0x9F
#define SMARTCARD_OP_TERMINATED   0x9000
/* Smartcard Voltage */
#define SMARTCARD_VOLTAGE_5V      0
#define SMARTCARD_VOLTAGE_3V      1

/* Smartcard communication method */
#define SMARTCARD_POLLING         0
#define SMARTCARD_INTERRUPT       1
#define SMARTCARD_DMA             2

/* USART Clock set at most to 4 MHz (PCLK1 (150 MHz) / 38) => prescaler set to 19 */
#define SMARTCARD_PRESCALER       19

/* SMARTCARD Timeout */
#define SMARTCARD_TIMEOUT                ((uint32_t)1000)

#define SMARTCARD_IO                       SMARTCARD_IO_PIN
#define SMARTCARD_IO_GPIO_PORT             GPIOC
#define SMARTCARD_IO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define SMARTCARD_IO_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define SMARTCARD_IO_PIN                   GPIO_PIN_10

#define SMARTCARD_CLK                      SMARTCARD_CLK_PIN
#define SMARTCARD_CLK_GPIO_PORT             GPIOC
#define SMARTCARD_CLK_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define SMARTCARD_CLK_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOC_CLK_DISABLE()
#define SMARTCARD_CLK_PIN                  GPIO_PIN_12

#if defined(USE_IOEXPANDER)
#define SMARTCARD_1V8                      IO_PIN_8
#define SMARTCARD_3_5V                     IO_PIN_9
#define SMARTCARD_OFF                      IO_PIN_10
#define SMARTCARD_CMDVCC                   IO_PIN_11
#define SMARTCARD_RST                      IO_PIN_12
#endif /* USE_IOEXPANDER */

/**
  * @}
  */
/** @defgroup STM32G474E-EVAL_SMARTCARD_Exported_Types SMARTCARD Exported Types
  * @{
  */

/* SMARTCARD Exported types ------------------------------------------------------------*/
typedef enum
{
  SMARTCARD_POWER_ON = 0x00,
  SMARTCARD_RESET_LOW = 0x01,
  SMARTCARD_RESET_HIGH = 0x02,
  SMARTCARD_ACTIVE = 0x03,
  SMARTCARD_ACTIVE_ON_T0 = 0x04,
  SMARTCARD_POWER_OFF = 0x05
} SMARTCARD_State;

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
  uint8_t TS;               /* Bit Convention */
  uint8_t T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
  uint8_t T[SETUP_LENGTH];  /* Setup array */
  uint8_t H[HIST_LENGTH];   /* Historical array */
  uint8_t Tlength;          /* Setup array dimension */
  uint8_t Hlength;          /* Historical array dimension */
} SMARTCARD_ATR;

/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} SMARTCARD_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct
{
  uint8_t LC;           /* Data field length */
  uint8_t Data[LC_MAX];  /* Command parameters */
  uint8_t LE;           /* Expected length of data to be returned */
} SMARTCARD_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
  SMARTCARD_Header Header;
  SMARTCARD_Body Body;
} SMARTCARD_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
  uint8_t Data[LC_MAX]; /* Data returned from the card */
  uint8_t SW1;          /* Command Processing status */
  uint8_t SW2;          /* Command Processing qualification */
} SMARTCARD_ADPU_Response;

/* DMA request mapping descriptor */
typedef struct {
  DMA_Channel_TypeDef *    DMA_channel_rx;
  uint32_t                 DMA_request_rx;
  DMA_Channel_TypeDef *    DMA_channel_tx;
  uint32_t                 DMA_request_tx;
} USART_DMA_t;

typedef struct {
  IRQn_Type  IRQn_USART;
  IRQn_Type  IRQn_DMA_rx;
  IRQn_Type  IRQn_DMA_tx;
 } USART_IRQ_t;

typedef enum {
  USART_CLOCK_SOURCE_PCLK = 0,
  USART_CLOCK_SOURCE_SYSCLK,
  USART_CLOCK_SOURCE_HSI,
  USART_CLOCK_SOURCE_LSE
} USART_Clock_Source_t;

/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_SMARTCARD_Exported_Variables STM32G474E EVAL SMARTCARD Exported Variables
  * @{
  */
/**
  * @}
  */


/** @defgroup STM32G474E-EVAL_SMARTCARD_Exported_Functions SMARTCARD Exported Functions
  * @{
  */

int32_t BSP_SMARTCARD_Init(void);
int32_t BSP_SMARTCARD_DeInit(void);
int32_t BSP_SMARTCARD_IsDetected(void);
int32_t BSP_SMARTCARD_Config(void);

int32_t BSP_SMARTCARD_Recv(uint8_t *pData, uint16_t Size);
int32_t BSP_SMARTCARD_Send(uint8_t *pData, uint16_t Size);
int32_t BSP_SMARTCARD_Abort(void);

/* Link functions for SmartCard peripheral */
void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsmartcard);
void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsmartcard);

/* SMARTCARD_PowerCmd is active low */
#define SMARTCARD_Power_DISABLE()  SMARTCARD_PowerCmd(SMARTCARD_DISABLE)
#define SMARTCARD_Power_ENABLE()   SMARTCARD_PowerCmd(SMARTCARD_ENABLE)

#define SMARTCARD_Reset_ON()       SMARTCARD_Reset(IO_PIN_SET)
#define SMARTCARD_Reset_OFF()      SMARTCARD_Reset(IO_PIN_RESET)

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

#endif /* STM32G474E_EVAL_SMARTCARD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
