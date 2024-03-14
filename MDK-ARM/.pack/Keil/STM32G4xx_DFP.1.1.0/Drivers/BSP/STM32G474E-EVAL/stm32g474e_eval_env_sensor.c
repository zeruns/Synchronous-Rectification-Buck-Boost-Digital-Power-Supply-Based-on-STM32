/**
  ******************************************************************************
  * @file    stm32g474e_eval_env_sensor.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the I2C TS751
  *          temperature sensor mounted on STM32G474E-EVAL board .
  *          It implements a high level communication layer for read and write
  *          from/to this sensor. The needed STM323G474 hardware resources (I2C and
  *          GPIO) are defined in stm32g474e_eval.h file, and the initialization is
  *          performed in BSP_ENV_SENSOR_Init() function declared in stm32g474e_eval.c
  *          file.
  *          You can easily tailor this driver to any other development board,
  *          by just adapting the defines for hardware resources and
  *          ENV_SENSOR_IO_Init() function.
  *
  *     +-----------------------------------------------------------------------+
  *     |                        Pin assignment                                 |
  *     +--------------------------------------------+-------------+------------+
  *     |  STM32G474 I2C Pins                       |   STTS751    |   Pin      |
  *     +-------------------------------------------+--------------+------------+
  *     | .                                         |   Addr/Therm |    1       |
  *     | .                                         |   GND        |    2 (0V)  |
  *     | .                                         |   VDD        |    3 (3.3V)|
  *     | ENV_SENSOR_I2C_SCL_PIN/ SCL               |   SCL        |    4       |
  *     | ENV_SENSOR_I2C_SMBUSALERT_PIN/ SMBUS ALERT|   SMBUS ALERT|    5       |
  *     | ENV_SENSOR_I2C_SDA_PIN/ SDA               |   SDA        |    6       |
  *     +-------------------------------------------+--------------+------------+
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

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_env_sensor.h"
#include "stm32g474e_eval_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32G474E-EVAL STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR STM32G474E-EVAL ENV_SENSOR
  * @brief      This file includes the STTS751 Temperature Sensor driver of
  *             STM32G474E-EVAL boards.
  * @{
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Variables Exported Variables
  * @{
  */
void *EnvCompObj = NULL;
ENV_SENSOR_Ctx_t EnvCtx = {0};
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Private_Variables Private Variables
  * @{
  */
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv = NULL;
static ENV_SENSOR_CommonDrv_t *EnvDrv = NULL;
/**
  * @}
  */

/** STM32G474E-EVAL_ENV_SENSOR_Private_FunctionPrototypes Private Functions
  */
#if (USE_ENV_SENSOR_STTS751 == 1)
static int32_t STTS751_0_Probe(uint32_t Functions);
#endif
/**
  */

/** @addtogroup STM32G474E-EVAL_ENV_SENSOR_Exported_Functions
  * @{
  */
/**
  * @brief  Initializes peripherals used by the I2C Temperature Sensor driver.
  * @param  Instance environmental sensor instance to be used
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE  for instance 0
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret;

  if ((Functions != ENV_TEMPERATURE) || (Instance >= ENV_INSTANCES_NBR) )
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(STTS751_0_Probe(Functions) != BSP_ERROR_NONE)
  {
      ret = BSP_ERROR_NO_INIT;
  }
  else
  {
      ret = BSP_ERROR_NONE;
  }

  if(ret == BSP_ERROR_NONE)
  {
        if(EnvFuncDrv->Enable(EnvCompObj) < 0 )
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
  }

  return ret;
}

/**
  * @brief  DeInitializes peripherals used by the I2C Temperature Sensor driver.
  * @param  Instance environmental sensor instance to be used
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv->DeInit(EnvCompObj) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get environmental sensor instance capabilities
  * @param  Instance Environmental sensor instance
  * @param  Capabilities pointer to Environmental sensor capabilities
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv->GetCapabilities(EnvCompObj, Capabilities) < 0)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Returns the Temperature ID status.
  * @param  Instance environmental sensor instance to be used
  * @param  Id WHOAMI value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv->ReadID(EnvCompObj, Id) < 0)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get environmental sensor value
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE for instance 0
  * @param  Value pointer to environmental sensor value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value)
{
  int32_t ret;

  if( (Instance >= ENV_INSTANCES_NBR) || ( Function != ENV_TEMPERATURE ) )
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvFuncDrv->GetValue(EnvCompObj, Value) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Enables the Temperature Sensor.
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE for instance 0
  * @retval BSP Status.
  */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret =  BSP_ERROR_NONE;

  if( (Instance >= ENV_INSTANCES_NBR) || (Function != ENV_TEMPERATURE) || ((EnvCtx.Functions & Function) != Function))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
     if(EnvFuncDrv->Enable(EnvCompObj) < 0)
     {
       ret = BSP_ERROR_COMPONENT_FAILURE;
     }
  }
  return ret;
}

/**
  * @brief  Disable environmental sensor sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE for instance 0
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= ENV_INSTANCES_NBR) || ( Function != ENV_TEMPERATURE ) || ((EnvCtx.Functions & Function) != Function))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(EnvFuncDrv->Disable(EnvCompObj) < 0 )
    {
        ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  return ret;
}

/**
  * @brief  Get environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE for instance 0
  * @param  Odr pointer to Output Data Rate read value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  int32_t ret = BSP_ERROR_NONE;

  if( (Instance >= ENV_INSTANCES_NBR) || ( Function != ENV_TEMPERATURE ) )
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }

  if (EnvFuncDrv->GetOutputDataRate(EnvCompObj, Odr) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  return ret;
}

/**
  * @brief  Set environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE for instance 0
  * @param  Odr Output Data Rate value to be set
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  if( (Instance >= ENV_INSTANCES_NBR) || ( Function != ENV_TEMPERATURE ) )
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (EnvFuncDrv->SetOutputDataRate(EnvCompObj, Odr) < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    return BSP_ERROR_NONE;
  }
}

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Private_Functions Private Functions
  * @{
  */
#if (USE_ENV_SENSOR_STTS751 == 1)
/**
  * @brief  Register Bus IOs for instance 0 if STTS751 ID is OK
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE
  * @retval BSP status
  */
static int32_t STTS751_0_Probe(uint32_t Functions)
{
  int32_t                ret;
  STTS751_IO_t           IOCtx;
  uint8_t                id;
  static STTS751_Object_t STTS751Obj_0;
  STTS751_Capabilities_t  Cap;

  /* Configure the environmental sensor driver */
  IOCtx.BusType     = STTS751_I2C_BUS; /* I2C */
  IOCtx.Address     = STTS751_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C3_Init;
  IOCtx.DeInit      = BSP_I2C3_DeInit;
  IOCtx.ReadReg     = BSP_I2C3_ReadReg;
  IOCtx.WriteReg    = BSP_I2C3_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if(STTS751_RegisterBusIO (&STTS751Obj_0, &IOCtx) < 0)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(STTS751_ReadID(&STTS751Obj_0, &id) < 0)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != STTS751_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;

    (void)STTS751_GetCapabilities(&STTS751Obj_0, &Cap);
    EnvCtx.Functions   = ((uint32_t)Cap.Temperature);

    EnvCompObj = &STTS751Obj_0;
    /* The second cast (void*) is added to bypass Misra R11.3 rule */
    EnvDrv = (ENV_SENSOR_CommonDrv_t *)(void*) &STTS751_COMMON_Driver;

    if(((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (Cap.Temperature == 1U))
    {
      /* The second cast (void*) is added to bypass Misra R11.3 rule */
      EnvFuncDrv = (ENV_SENSOR_FuncDrv_t *)(void*) &STTS751_TEMP_Driver;

      if(EnvDrv->Init(EnvCompObj) < 0)
      {
        ret += BSP_ERROR_COMPONENT_FAILURE;
      }
    }

    if(ret != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
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

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
