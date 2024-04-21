/**
  ******************************************************************************
  * @file    mt25ql512abb_conf_template.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MT25QL512ABB QSPI memory.
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
#ifndef MT25QL512ABB_CONF_H
#define MT25QL512ABB_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"

/** @addtogroup BSP
  * @{
  */
#define CONF_QSPI_ODS             MT25QL512ABB_EVCR_ODS_30   /* MT25QL512ABB Output Driver Strenght         */


#ifdef __cplusplus
}
#endif

#endif /* MT25QL512ABB_CONF_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/