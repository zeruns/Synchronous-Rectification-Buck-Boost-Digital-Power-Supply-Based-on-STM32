/**
  ******************************************************************************
  * @file    stm32g474e_eval_env_sensor.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          stm32g474e_eval_env_sensor.c firmware driver.
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
#ifndef STM32G474E_EVAL_ENV_SENSOR_H
#define STM32G474E_EVAL_ENV_SENSOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"

#include "../Components/stts751/stts751.h"
#include "../Components/Common/env_sensor.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_ENV_SENSOR
  * @{
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Types Exported Types
  * @{
  */

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Constants Exported Constants
  * @{
  */
#ifndef USE_ENV_SENSOR_STTS751
#define USE_ENV_SENSOR_STTS751          1U
#endif

/* Temperature Sensor hardware I2C address : 1001000 */
#define STTS751_I2C_ADDRESS             0x90

#define ENV_TEMPERATURE      1U

#define STTS751_ID           STTS751_MANUFACTURER

#define ENV_FUNCTIONS_NBR    1U
#define ENV_INSTANCES_NBR    (USE_ENV_SENSOR_STTS751)
#define ENV_INSTANCE         0U

#if (ENV_INSTANCES_NBR == 0)
#error "No environmental sensor instance has been selected"
#endif
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_ENV_SENSOR_Exported_Variables
  * @{
  */
extern void *EnvCompObj;
extern ENV_SENSOR_Ctx_t EnvCtx;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Functions Exported Functions
  * @{
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);
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

#endif /* STM32G474E_EVAL_ENV_SENSOR_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
