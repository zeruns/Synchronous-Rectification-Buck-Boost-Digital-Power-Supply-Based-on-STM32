/*
 ******************************************************************************
 * @file    stts751_regs.c
 * @author  MCD Application Team
 * @brief   STTS751 driver file
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

#include "stts751_regs.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/**
  * @addtogroup  stts751
  * @brief  This file provides a set of functions needed to drive the
  *         stts751 enhanced inertial module.
  * @{
  */

/** @defgroup STTS751_Exported_Functions STTS751 Exported Functions
  * @brief  This section provide a set of functions used to read and write
  *         a generic register of the device.
  * @{
  */

/**
  * @brief  Read generic device register
  * @param  stts751_ctx_t* ctx: read / write interface definitions
  * @param  uint8_t reg: register to read
  * @param  uint8_t* data: pointer to buffer that store the data read
  * @param  uint16_t len: number of consecutive register to read
  * @retval Component status
  */
int32_t stts751_read_reg(stts751_ctx_t* ctx, uint8_t reg, uint8_t* data,
                         uint16_t len)
{
  int32_t ret;
  ret = ctx->read_reg(ctx->handle, reg, data, len);
  return ret;
}

/**
  * @brief  Write generic device register
  * @param  stts751_ctx_t *ctx: read / write interface definitions
  * @param  uint8_t reg register to write
  * @param  uint8_t* data: pointer to data to write in register reg
  * @param  uint16_t len: number of consecutive register to write
  * @retval Component status
*/
int32_t stts751_write_reg(stts751_ctx_t* ctx, uint8_t reg, uint8_t* data,
                          uint16_t len)
{
  int32_t ret;
  ret = ctx->write_reg(ctx->handle, reg, data, len);
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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/