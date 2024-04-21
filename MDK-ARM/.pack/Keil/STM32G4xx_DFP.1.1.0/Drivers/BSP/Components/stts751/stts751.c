/**
 ******************************************************************************
 * @file    stts751.c
 * @author  MCD Application Team
 * @brief   stts751 driver file
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

/* Includes ------------------------------------------------------------------*/
#include "stts751.h"
#include "cmsis_iar.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup STTS751 STTS751
  * @{
  */

/** @defgroup STTS751_Exported_Variables STTS751 Exported Variables
  * @{
  */
STTS751_CommonDrv_t STTS751_COMMON_Driver = {
  STTS751_Init,
  STTS751_DeInit,
  STTS751_ReadID,
  STTS751_GetCapabilities,
};

STTS751_TEMP_Drv_t STTS751_TEMP_Driver = {
  STTS751_TEMP_Enable,
  STTS751_TEMP_Disable,
  STTS751_TEMP_GetOutputDataRate,
  STTS751_TEMP_SetOutputDataRate,
  STTS751_TEMP_GetTemperature,
};

STTS751_Config_t  STTS751_TEMP_Config = {
  USE_ENV_SENSOR_CONV_MODE,
  USE_ENV_SENSOR_CONV_RESOLUTION,
  USE_ENV_SENSOR_CONV_RATE,
  USE_ENV_SENSOR_LIMIT_HIGH,
  USE_ENV_SENSOR_LIMIT_LOW,
  USE_ENV_SENSOR_THERM_LIMIT,
  USE_ENV_SENSOR_THERM_HYSTERESIS,
};
/**
  * @}
  */

/** @defgroup STTS751_Private_Function_Prototypes STTS751 Private Function Prototypes
  * @{
  */
static int32_t ReadRegWrap(void *handle, uint8_t Reg, uint8_t* Data, uint16_t len);
static int32_t WriteRegWrap(void *handle, uint8_t Reg, uint8_t* Data, uint16_t len);

int32_t STTS751_TEMP_SetConversionMode(STTS751_Object_t *pObj, uint8_t val);
int32_t STTS751_TEMP_SetConversionResolution(STTS751_Object_t *pObj, uint8_t val);
int32_t STTS751_TEMP_SetConversionRate(STTS751_Object_t *pObj, uint8_t val);

int32_t STTS751_TEMP_SetThermHysteresis(STTS751_Object_t *pObj, uint8_t val);
int32_t STTS751_TEMP_SetThermLimit(STTS751_Object_t *pObj, uint8_t val);
int32_t STTS751_TEMP_GetThermLimit(STTS751_Object_t *pObj, uint8_t *val);

int32_t STTS751_TEMP_SetTempLimitHigh(STTS751_Object_t *pObj, float temp);
int32_t STTS751_TEMP_GetTempLimitHigh(STTS751_Object_t *pObj, uint8_t *val);
int32_t STTS751_TEMP_SetTempLimitLow(STTS751_Object_t *pObj, float temp);
int32_t STTS751_TEMP_GetTempLimitLow(STTS751_Object_t *pObj, uint8_t *val);

int32_t STTS751_TEMP_GetDeviceReady(STTS751_Object_t *pObj);

int32_t STTS751_TEMP_GetDeviceID(STTS751_Object_t *pObj, uint8_t *buff[3]);
/**
  * @}
  */

/** @defgroup STTS751_Exported_Functions STTS751 Exported Functions
  * @{
  */
/**
  * @brief  Register Component Bus IO operations
  * @param  Component object pointer
  * @retval Component status
  */
int32_t STTS751_RegisterBusIO (STTS751_Object_t *pObj, STTS751_IO_t *pIO)
{
  int32_t ret = STTS751_OK;

  if (pObj == NULL)
  {
    ret = STTS751_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.BusType   = pIO->BusType;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.read_reg  = ReadRegWrap;
    pObj->Ctx.write_reg = WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if(pObj->IO.Init == NULL)
    {
      ret = STTS751_ERROR;
    }
    else if(pObj->IO.Init() != STTS751_OK)
    {
      ret = STTS751_ERROR;
    }
    else
    {
      if(pObj->IO.BusType == STTS751_I2C_BUS) /* only I2C */
      {
            ret = STTS751_OK;
      }
      else /* if not I2C */
      {
            ret = STTS751_ERROR;
      }
    }
  }
  return ret;
}

/**
  * @brief  Initialize the STTS751 sensor
  * @param  Component object pointer
  * @retval Component status
  */
int32_t STTS751_Init(STTS751_Object_t *pObj)
{
  int32_t ret = STTS751_OK;

  if(pObj->is_initialized == 0U)
  {
    if ( STTS751_TEMP_SetConversionMode(pObj, USE_ENV_SENSOR_CONV_MODE) == STTS751_OK )
         STTS751_TEMP_Config.conv_mode = USE_ENV_SENSOR_CONV_MODE;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetConversionResolution(pObj,USE_ENV_SENSOR_CONV_RESOLUTION) == STTS751_OK )
         STTS751_TEMP_Config.conv_resolution = USE_ENV_SENSOR_CONV_RESOLUTION;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetConversionRate(pObj,USE_ENV_SENSOR_CONV_RATE) == STTS751_OK )
         STTS751_TEMP_Config.conv_rate = USE_ENV_SENSOR_CONV_RATE;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetTempLimitHigh(pObj,USE_ENV_SENSOR_LIMIT_HIGH) == STTS751_OK )
         STTS751_TEMP_Config.temp_limit_high = USE_ENV_SENSOR_LIMIT_HIGH;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetTempLimitLow(pObj,USE_ENV_SENSOR_LIMIT_LOW) == STTS751_OK )
         STTS751_TEMP_Config.temp_limit_low = USE_ENV_SENSOR_LIMIT_LOW;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetThermLimit(pObj,USE_ENV_SENSOR_THERM_LIMIT) == STTS751_OK )
         STTS751_TEMP_Config.therm_limit = USE_ENV_SENSOR_THERM_LIMIT;
    else
         ret = STTS751_ERROR;

    if ( STTS751_TEMP_SetThermHysteresis(pObj,USE_ENV_SENSOR_THERM_HYSTERESIS) == STTS751_OK )
         STTS751_TEMP_Config.therm_hysteresis = USE_ENV_SENSOR_THERM_HYSTERESIS;
    else
         ret = STTS751_ERROR;

    if (ret != STTS751_ERROR)
        pObj->is_initialized = 1;
  }
  return ret;
}

/**
 * @brief Deinitialize the STTS751 sensor
 * @param pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_DeInit( STTS751_Object_t *pObj)
{
  if(pObj->is_initialized == 1U)
  {
    if(STTS751_TEMP_Disable(pObj) != STTS751_OK)
    {
      return STTS751_ERROR;
    }
  }

  pObj->is_initialized = 0;

  return STTS751_OK;
}

/**
  * @brief  Gives the Manufacturer ID of the sensor
  *         component has PRODUCT_ID/MANUFACTURER_ID/REVISION_ID
  * @param  Component object pointer
  * @retval Component status
  */
int32_t STTS751_ReadID(STTS751_Object_t *pObj, uint8_t *Id)
{

  uint8_t tmp = 0;
  uint32_t tsensor_id =0;

  /* Read ID register */
  stts751_read_reg(&(pObj->Ctx), STTS751_PRODUCT_ID, &tmp, 1);
  tsensor_id =  (uint32_t)( tmp << 16);
  /* Read ID register */
  stts751_read_reg(&(pObj->Ctx), STTS751_MANUFACTURER_ID, &tmp, 1);
  tsensor_id |= (uint32_t)( tmp << 8);
  /* Read ID register */
  stts751_read_reg(&(pObj->Ctx), STTS751_REVISION_ID, &tmp, 1);
  tsensor_id |= (uint32_t)(tmp);

  /* Return only the Manufacturer ID of the Temperature Sensor */
  *Id = (uint8_t)(tsensor_id >> 8);

  return STTS751_OK;
}

/**
  * @brief  Get STTS751 sensor capabilities
  * @param  pObj Component object pointer
  * @param  Capabilities pointer to STTS751 sensor capabilities
  * @retval Component status
  */
int32_t STTS751_GetCapabilities(STTS751_Object_t *pObj, STTS751_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  Capabilities->Temperature = 1;
  Capabilities->LowPower    = 0;
  Capabilities->TempMaxOdr  = 125.0f;

  return STTS751_OK;
}

/**
 * @brief Enable the STTS751 temperature sensor
 * @param pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_Enable(STTS751_Object_t *pObj)
{
  int32_t ret;

  if (pObj->is_initialized == 0)
  {
     ret = STTS751_ERROR;
  }
  else
  {
   /* Output data rate selection - power down. */
   if ( STTS751_TEMP_SetConversionMode(pObj, USE_ENV_SENSOR_CONV_MODE) == STTS751_OK )
   {
      STTS751_TEMP_Config.conv_mode = USE_ENV_SENSOR_CONV_MODE;
      /* Check if the component is already enabled : wait for device ready */
      if (STTS751_TEMP_GetDeviceReady(pObj) == STTS751_OK )
      {
         pObj->temp_is_enabled = 1U;
         ret = STTS751_OK;
       }
       else
       {
         pObj->temp_is_enabled = 0U;
         ret = STTS751_ERROR;
       }
    }
   else
         ret = STTS751_ERROR;

  }
  return ret;
}

/**
 * @brief Disable the STTS751 temperature sensor
 * @param pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_Disable(STTS751_Object_t *pObj )
{
  int32_t ret;

  /* Check if the component is already disabled */
  if (pObj->is_initialized == 0)
  {
     ret = STTS751_ERROR;
  }
  else if (pObj->temp_is_enabled == 0)
  {
     ret = STTS751_OK;
  }
  else
  {
     /* Output data rate selection - power down. */
     if (STTS751_TEMP_SetConversionMode(pObj, STTS751_ONE_SHOT_MODE) != STTS751_OK)
     {
       ret = STTS751_ERROR;
     }
     else
     {
       STTS751_TEMP_Config.conv_mode = STTS751_ONE_SHOT_MODE;
       pObj->temp_is_enabled = 0;
       ret = STTS751_OK;
     }
  }

  return ret;
}

/**
 * @brief Get the STTS751 temperature sensor output data rate
 * @param pObj the device pObj
 * @param Odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_GetOutputDataRate(STTS751_Object_t *pObj, float *Odr )
{
  uint8_t tmp;
  int32_t ret;

  ret = stts751_read_reg(&(pObj->Ctx), STTS751_CONVERSION_RATE, &tmp, 1);
  if(ret == STTS751_OK){
    *Odr  = SHIFT_CONV_RATE[(tmp & 0x0FU)];
  }
  return ret;
}

/**
 * @brief Set the STTS751 temperature sensor output data rate
 * @param pObj the device pObj
 * @param odr the output data rate value to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_SetOutputDataRate( STTS751_Object_t *pObj, float Odr )
{
  uint8_t tmp;
  int32_t ret;

  tmp =  ( __iar_builtin_CLZ(__iar_builtin_RBIT((int)(Odr * 16)))); /*POSITION_VAL((Odr * 16));*/

  ret = stts751_write_reg(&(pObj->Ctx), STTS751_CONVERSION_RATE, &tmp, 1);
  if(ret == STTS751_OK){
    STTS751_TEMP_Config.conv_mode = tmp;
  }
  return ret;
}

/**
 * @brief Get the STTS751 temperature value
 * @param pObj the device pObj
 * @param value pointer where the temperature value is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_GetTemperature(STTS751_Object_t *pObj, float *Value)
{
  uint8_t bufferrx[2] = {0,0};
  int32_t ret;

  if ((pObj->is_initialized == 1) && (pObj->temp_is_enabled == 1) )
  {
       /* Read Temperature registers */
       stts751_read_reg(&(pObj->Ctx), STTS751_TEMP_VALUE_HIGH_BYTE, &bufferrx[0], 1);
       stts751_read_reg(&(pObj->Ctx), STTS751_TEMP_VALUE_LOW_BYTE,  &bufferrx[1], 1);

  /* Give the Temperature value */
#if  (USE_ENV_SENSOR_CONV_RESOLUTION == STTS751_CONV_12BITS)
       *Value = (float)bufferrx[0] + (float)(bufferrx[1] >> 4)/16 ;
#elif (USE_ENV_SENSOR_CONV_RESOLUTION == STTS751_CONV_11BITS)
       *Value = (float)bufferrx[0] + (float)(bufferrx[1] >> 4)/8 ;
#elif (USE_ENV_SENSOR_CONV_RESOLUTION == STTS751_CONV_9BITS)
       *Value = (float)bufferrx[0] + (float)(bufferrx[1] >> 4)/2 ;
#else
       *Value = (float)bufferrx[0] + (float)(bufferrx[1] >> 4)/4 ;
#endif /* USE_ENV_SENSOR_CONV_RESOLUTION*/

     ret = STTS751_OK;
}
  else
     ret = STTS751_ERROR;

  return ret;
}

/**
  * @}
  */

/**
  * @brief  Wrap Read register component function to Bus IO function
  * @param  Component object pointer
  * @retval Component status
  */
static int32_t ReadRegWrap(void *handle, uint8_t Reg, uint8_t* pData, uint16_t Len)
{
  STTS751_Object_t *pObj = (STTS751_Object_t *)handle;

  if(pObj->IO.BusType == STTS751_I2C_BUS) /* I2C */
  {
    uint32_t i;
    int32_t ret = STTS751_OK;
    for(i = 0; i < Len; i++)
    {
      ret = pObj->IO.ReadReg(pObj->IO.Address, (Reg + i), &pData[i], 1);
      if(ret != STTS751_OK)
      {
        return STTS751_ERROR;
      }
    }

    return ret;
  } else /* SPI 4-Wires or SPI 3-Wires */
  {
    return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Len);
  }
}

/**
  * @brief  Wrap Write register component function to Bus IO function
  * @param  Component object pointer
  * @retval Component status
  */
static int32_t WriteRegWrap(void *handle, uint8_t Reg, uint8_t* pData, uint16_t Len)
{
  STTS751_Object_t *pObj = (STTS751_Object_t *)handle;

  if(pObj->IO.BusType == STTS751_I2C_BUS) /* I2C */
  {
    uint32_t i;
    int32_t ret = STTS751_OK;
    for(i = 0; i < Len; i++)
    {
      ret = pObj->IO.WriteReg(pObj->IO.Address, Reg, &pData[i], 1);
      if(ret != STTS751_OK)
      {
        return STTS751_ERROR;
      }
    }

    return ret;
  } else /* SPI 4-Wires or SPI 3-Wires */
  {
    return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Len);
  }
}

/**
  * @brief This function provides accurate delay (in milliseconds)
  * @param pObj pointer to component object
  * @param Delay specifies the delay time length, in milliseconds
  * @retval STTS751_OK
  */
int32_t STTS751_Delay(STTS751_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;
  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return STTS751_OK;
}


/** @defgroup STM32G474E_EVAL_TSENSOR_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  the the mode of conversion: [set].
  * @param  pObj pointer to component object
  * @param  the mode to set:
  *                           STTS751_CONTINUOUS_MODE
  *                           STTS751_ONE_SHOT_MODE
  * @retval Component status
  */
int32_t STTS751_TEMP_SetConversionMode(STTS751_Object_t *pObj, uint8_t val)
{
  uint8_t tmp;
  int32_t ret;

  ret = stts751_read_reg(&(pObj->Ctx), STTS751_CONFIGURATION, &(tmp), 1);
  if(ret == 0){
    tmp = (uint8_t) val & 0x40U;
    ret = stts751_write_reg(&(pObj->Ctx), STTS751_CONFIGURATION, &(tmp), 1);
  }
  return ret;
}

/**
  * @brief   set the resolution : [set] .
  *          the conversion rate must be consistent with the resolution rate
  * @param   pObj pointer to component object
  * @param   the resolution to set :
  *            STTS751_CONV_9BITS
  *            STTS751_CONV_10BITS
  *            STTS751_CONV_11BITS
  *            STTS751_CONV_12BITS
  * @retval  Component status
  */
int32_t STTS751_TEMP_SetConversionResolution(STTS751_Object_t *pObj, uint8_t val)
{
  uint8_t tmp;
  int32_t ret;

  ret = stts751_read_reg(&(pObj->Ctx), STTS751_CONFIGURATION, &(tmp), 1);
  if(ret == 0){
    tmp = (uint8_t) val & 0x0CU;
    ret = stts751_write_reg(&(pObj->Ctx), STTS751_CONFIGURATION, &(tmp), 1);
  }
  return ret;
}

/**
  * @brief  the the rate of conversion: [set]  Low-pass bandwidth selection.
  *         the conversion rate must be consistent with the resolution rate
  * @param  pObj pointer to component object
  * @param  the conversion rate to set 0-9
  * @retval Component status
  */
int32_t STTS751_TEMP_SetConversionRate(STTS751_Object_t *pObj, uint8_t val)
{
  uint8_t tmp;
  int32_t ret;

  ret = stts751_read_reg(&(pObj->Ctx), STTS751_CONVERSION_RATE, &(tmp), 1);
  if(ret == 0){
    tmp = (uint8_t) val & 0x0FU;
    ret = stts751_write_reg(&(pObj->Ctx), STTS751_CONVERSION_RATE, &(tmp), 1);
  }
  return ret;
}

/**
  * @brief   wait STTS751_TIMEOUT ms for device status ready.
  * @param   pObj pointer to component object
  * @retval  Component status
  */
int32_t  STTS751_TEMP_GetDeviceReady(STTS751_Object_t *pObj)
{
  uint8_t busy_status = 0x80U;
  int32_t ret = STTS751_OK;

  /* Check End of conversion flag */
  while( (busy_status & STTS751_STATUS_READY) != 0U)
  {
    if (stts751_read_reg(&(pObj->Ctx), STTS751_STATUS, &(busy_status), 1) == STTS751_OK)
    {
        ret = STTS751_OK;
    }
    else
    {
      STTS751_Delay(pObj, STTS751_TIMEOUT);
      ret = STTS751_ERROR;
      break;
    }
  }

  return ret;
}

/**
  * @brief  set the temperature high limit: [set] .
  * @param  pObj pointer to component object
  * @param  high temperature to set
  * @retval Component status
  */
int32_t  STTS751_TEMP_SetTempLimitHigh(STTS751_Object_t *pObj, float temp)
{
  int32_t ret = STTS751_OK;

  uint8_t bufferrx[2];

  bufferrx[0] = (uint8_t)temp ;
  bufferrx[1] = (uint8_t)((temp -  bufferrx[0])* 100U * SHIFT_CONV_RESOLUTION[USE_ENV_SENSOR_CONV_RESOLUTION>>2] );

  if (stts751_write_reg(&(pObj->Ctx), STTS751_TEMP_HIGH_LIMIT_HIGH_BYTE, &bufferrx[0], 1) != STTS751_OK )
  {
    ret = STTS751_ERROR;
  }
  else
  {
    if (stts751_write_reg(&(pObj->Ctx), STTS751_TEMP_HIGH_LIMIT_LOW_BYTE,  &bufferrx[1], 1) != STTS751_OK )
    {
      ret = STTS751_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  set the temperature low limit: [set] .
  * @param  pObj pointer to component object
  * @param  low temperature to set
  * @retval Component status
  */
int32_t  STTS751_TEMP_SetTempLimitLow(STTS751_Object_t *pObj, float temp)
{
  int32_t ret = STTS751_OK;

  uint8_t bufferrx[2];

  bufferrx[0] = (uint8_t)temp ;
  bufferrx[1] = (uint8_t)((temp -  bufferrx[0])* 100U * SHIFT_CONV_RESOLUTION[USE_ENV_SENSOR_CONV_RESOLUTION>>2] );

  if (stts751_write_reg(&(pObj->Ctx), STTS751_TEMP_LOW_LIMIT_HIGH_BYTE, &bufferrx[0], 1) != STTS751_OK)
  {
      ret = STTS751_ERROR;
  }
  else
  {
    if (stts751_write_reg(&(pObj->Ctx), STTS751_TEMP_LOW_LIMIT_LOW_BYTE,  &bufferrx[1], 1) != STTS751_OK)
    {
      ret = STTS751_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  set the temperature hysteresis
  * @param  pObj pointer to component object
  * @param  low temperature to set
  * @retval Component status
  */
int32_t STTS751_TEMP_SetThermHysteresis(STTS751_Object_t *pObj, uint8_t val)
{
  return stts751_write_reg(&(pObj->Ctx), STTS751_THERM_HYSTERISIS, &val, 1);
}

/**
  * @brief  set the thermal limit.
  * @param  pObj pointer to component object
  * @param  low temperature to set
  * @retval Component status
  */
int32_t STTS751_TEMP_SetThermLimit(STTS751_Object_t *pObj, uint8_t val)
{
  return stts751_write_reg(&(pObj->Ctx), STTS751_THERM_LIMIT, &val, 1);
}

/**
  * @brief  device_id: [get] DeviceWhoamI
  * @param  pObj pointer to component object
  * @param  uint8_t * : buffer that stores data read
  * @retval Component status
  */
int32_t STTS751_TEMP_GetDeviceID(STTS751_Object_t *pObj, uint8_t *buff[3])
{
  int32_t ret = STTS751_OK;

  if (stts751_read_reg(&(pObj->Ctx), STTS751_PRODUCT_ID, (uint8_t*) buff[0], 1) != STTS751_OK)
  {
    ret = STTS751_ERROR;
  }
  else
  {
    if ( stts751_read_reg(&(pObj->Ctx), STTS751_MANUFACTURER_ID, (uint8_t*) buff[1], 1) != STTS751_OK)
    {
      ret = STTS751_ERROR;
    }
    else
    {
      if (stts751_read_reg(&(pObj->Ctx), STTS751_REVISION_ID, (uint8_t*) buff[2], 1) != STTS751_OK)
      {
        ret = STTS751_ERROR;
      }
    }
  }

  return ret;
}

/**
  * @brief  dev_status: [get] indicates if the  temperature high limit has been reached
  * @param  pObj pointer to component object
  * @param  val
  *         STTS751_STATUS_REACHED
  *         STTS751_STATUS_NOT_REACHED
  * @retval Component status
  */
int32_t STTS751_TEMP_GetTempLimitHigh(STTS751_Object_t *pObj, uint8_t *val)
{
  int32_t ret = STTS751_OK;

  if (stts751_read_reg(&(pObj->Ctx), STTS751_STATUS, (uint8_t*) val, 1) != STTS751_OK)
  {
    ret = STTS751_ERROR;
  }
  else if ((*val & 0x40U) == 0U)
  {
       *val = STTS751_STATUS_NOT_REACHED;
  }
  else
  {
       *val = STTS751_STATUS_REACHED;
  }

  return ret;
}

/**
  * @brief  dev_status: [get] indicates if the  temperature low limit has been reached
  * @param  pObj pointer to component object
  * @param  val
  *         STTS751_STATUS_REACHED
  *         STTS751_STATUS_NOT_REACHED
  * @retval Component status
  */
int32_t STTS751_TEMP_GetTempLimitLow(STTS751_Object_t *pObj, uint8_t *val)
{
  int32_t ret = STTS751_OK;

  if (stts751_read_reg(&(pObj->Ctx), STTS751_STATUS, (uint8_t*) val, 1) != STTS751_OK)
  {
    ret = STTS751_ERROR;
  }
  else if ((*val & 0x20U) == 0U)
  {
       *val = STTS751_STATUS_NOT_REACHED;
  }
  else
  {
       *val = STTS751_STATUS_REACHED;
  }

  return ret;
}

/**
  * @brief  dev_status: [get] indicates if the measured temperature
  *                           has reached the therm limit
  * @param  pObj pointer to component object
  * @param  val
  *         STTS751_STATUS_REACHED
  *         STTS751_STATUS_NOT_REACHED
  * @retval Component status
  */
int32_t STTS751_TEMP_GetThermLimit(STTS751_Object_t *pObj, uint8_t *val)
{
  int32_t ret = STTS751_OK;

  if (stts751_read_reg(&(pObj->Ctx), STTS751_STATUS, (uint8_t*) val, 1) !=STTS751_OK)
  {
    ret = STTS751_ERROR;
  }
  else if ((*val & 0x01U) == 0U)
  {
        *val = STTS751_STATUS_NOT_REACHED;
  }
  else
  {
        *val = STTS751_STATUS_REACHED;
  }

  return ret;
}

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
