/**
  ******************************************************************************
  * @file    stm32g474e_eval_errno.h
  * @author  MCD Application Team
  * @brief   Error Code.
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
#ifndef STM32G474E_EVAL_ERRNO_H
#define STM32G474E_EVAL_ERRNO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Common Error codes */
#define BSP_ERROR_NONE                         0
#define BSP_ERROR_NO_INIT                     -1
#define BSP_ERROR_WRONG_PARAM                 -2
#define BSP_ERROR_BUSY                        -3
#define BSP_ERROR_PERIPH_FAILURE              -4
#define BSP_ERROR_COMPONENT_FAILURE           -5
#define BSP_ERROR_UNKNOWN_FAILURE             -6
#define BSP_ERROR_UNKNOWN_COMPONENT           -7
#define BSP_ERROR_BUS_FAILURE                 -8
#define BSP_ERROR_CLOCK_FAILURE               -9
#define BSP_ERROR_MSP_FAILURE                 -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED       -11

/* BSP QSPI error codes */
#define BSP_ERROR_QSPI_ASSIGN_FAILURE         -20
#define BSP_ERROR_QSPI_SETUP_FAILURE          -21
#define BSP_ERROR_QSPI_MMP_LOCK_FAILURE       -22
#define BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE     -23

/* BSP BUS error codes */
#define BSP_ERROR_BUS_TRANSACTION_FAILURE    -100
#define BSP_ERROR_BUS_ARBITRATION_LOSS       -101
#define BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE    -102
#define BSP_ERROR_BUS_PROTOCOL_FAILURE       -103

#define BSP_ERROR_BUS_MODE_FAULT             -104
#define BSP_ERROR_BUS_FRAME_ERROR            -105
#define BSP_ERROR_BUS_CRC_ERROR              -106
#define BSP_ERROR_BUS_DMA_FAILURE            -107

#endif /* STM32G474E_EVAL_ERRNO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
