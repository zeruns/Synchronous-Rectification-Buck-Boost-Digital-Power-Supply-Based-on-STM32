/**
 ******************************************************************************
 * @file    stts751.h
 * @author  MCD Application Team
 * @brief   STTS751 header driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STTS751_H
#define STTS751_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stts751_regs.h"
#include <stddef.h>
#include <string.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup STTS751
  * @{
  */

/** @defgroup STTS751_Exported_Types STTS751 Exported Types
 * @{
 */

typedef int32_t (*STTS751_Init_Func)    (void);
typedef int32_t (*STTS751_DeInit_Func)  (void);
typedef int32_t (*STTS751_GetTick_Func) (void);
typedef int32_t (*STTS751_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*STTS751_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  STTS751_Init_Func          Init;
  STTS751_DeInit_Func        DeInit;
  uint32_t                   BusType; /*0 means I2C, 1 means SPI 4-Wires, 2 means SPI-3-Wires */
  uint8_t                    Address;
  STTS751_WriteReg_Func      WriteReg;
  STTS751_ReadReg_Func       ReadReg;
  STTS751_GetTick_Func       GetTick;
} STTS751_IO_t;

typedef struct
{
  STTS751_IO_t        IO;
  stts751_ctx_t       Ctx;
  uint8_t             is_initialized;
  uint8_t             temp_is_enabled;
} STTS751_Object_t;

typedef struct
{
  uint8_t Temperature;
  uint8_t LowPower;
  float   TempMaxOdr;
} STTS751_Capabilities_t;

/* STTS751 configuration */
typedef struct {
  uint8_t  conv_mode         ;
  uint8_t  conv_resolution   ;
  uint8_t  conv_rate         ;
  float    temp_limit_high   ;
  float    temp_limit_low    ;
  uint8_t  therm_limit       ;
  uint8_t  therm_hysteresis  ;
} STTS751_Config_t;

typedef struct
{
  int32_t ( *Init              ) ( STTS751_Object_t * );
  int32_t ( *DeInit            ) ( STTS751_Object_t * );
  int32_t ( *ReadID            ) ( STTS751_Object_t *, uint8_t * );
  int32_t ( *GetCapabilities   ) ( STTS751_Object_t *, STTS751_Capabilities_t * );
} STTS751_CommonDrv_t;

typedef struct
{
  int32_t ( *Enable            ) ( STTS751_Object_t * );
  int32_t ( *Disable           ) ( STTS751_Object_t * );
  int32_t ( *GetOutputDataRate ) ( STTS751_Object_t *, float* );
  int32_t ( *SetOutputDataRate ) ( STTS751_Object_t *, float );
  int32_t ( *GetTemperature    ) ( STTS751_Object_t *, float* );
} STTS751_TEMP_Drv_t;

/**
  * @}
  */

/** @defgroup STTS751_Exported_Constants STTS751 Exported Constants
 * @{
 */

/**
  * @}
  */

/** @addtogroup STTS751_Exported_Functions
 * @{
 */
int32_t STTS751_RegisterBusIO (STTS751_Object_t *pObj, STTS751_IO_t *pIO);
int32_t STTS751_Init(STTS751_Object_t *pObj);
int32_t STTS751_DeInit(STTS751_Object_t *pObj);
int32_t STTS751_ReadID(STTS751_Object_t *pObj, uint8_t *Id);
int32_t STTS751_GetCapabilities(STTS751_Object_t *pObj, STTS751_Capabilities_t *Capabilities);

int32_t STTS751_TEMP_Enable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_Disable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_GetOutputDataRate(STTS751_Object_t *pObj, float  *Odr );
int32_t STTS751_TEMP_SetOutputDataRate(STTS751_Object_t *pObj, float  Odr );
int32_t STTS751_TEMP_GetTemperature(STTS751_Object_t *pObj, float *Value );

int32_t STTS751_Delay(STTS751_Object_t *pObj, uint32_t Delay);

/**
  * @}
  */

/** @addtogroup STTS751_Exported_Variables
  * @{
  */
extern STTS751_CommonDrv_t STTS751_COMMON_Driver;
extern STTS751_TEMP_Drv_t  STTS751_TEMP_Driver;
extern STTS751_Capabilities_t  STTS751_TEMP_Capabilities ;
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
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
