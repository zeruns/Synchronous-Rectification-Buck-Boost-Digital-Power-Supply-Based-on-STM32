/**
  ******************************************************************************
  * @file    hx8347d.c
  * @author  MCD Application Team
  * @brief   This file includes the LCD driver for HX8347D LCD.
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
#include "hx8347d.h"

/** @addtogroup BSP
* @{
*/

/** @addtogroup Components
* @{
*/

/** @addtogroup HX8347D
* @brief     This file provides a set of functions needed to drive the
*            HX8347D LCD.
* @{
*/

/** @defgroup HX8347D_Private_TypesDefinitions
* @{
*/
typedef struct
{
  uint32_t        Width;
  uint32_t        Height;
  uint32_t        Orientation;
} HX8347D_Ctx_t;
/**
* @}
*/

/** @defgroup HX8347D_Private_Defines
* @{
*/

/**
* @}
*/

/** @defgroup HX8347D_Private_Macros
* @{
*/

/**
* @}
*/

/** @defgroup HX8347D_Private_Variables
* @{
*/
HX8347D_Drv_t   HX8347D_Driver =
{
  HX8347D_Init,
  HX8347D_DeInit,
  HX8347D_ReadID,
  HX8347D_DisplayOn,
  HX8347D_DisplayOff,
  HX8347D_SetBrightness,
  HX8347D_GetBrightness,
  HX8347D_SetOrientation,
  HX8347D_GetOrientation,
  HX8347D_SetCursor,
  HX8347D_DrawBitmap,
  HX8347D_FillRGBRect,
  HX8347D_DrawHLine,
  HX8347D_DrawVLine,
  HX8347D_FillRect,
  HX8347D_GetPixel,
  HX8347D_SetPixel,
  HX8347D_GetXSize,
  HX8347D_GetYSize,
};

/* The below table handle the different values to be set to Memory Data Access Control
   depending on the orientation and pbm image writing where the data order is inverted
*/
static uint32_t OrientationTab[4][2] =
{
  {0x80U , 0x00U}, /* Portrait orientation choice of LCD screen               */
  {0x40U , 0xC0U}, /* Portrait rotated 180° orientation choice of LCD screen  */
  {0xE0U , 0xA0U}, /* Landscape orientation choice of LCD screen              */
  {0x20U , 0x60U}  /* Landscape rotated 180° orientation choice of LCD screen */
};

static HX8347D_Ctx_t HX8347DCtx;
static uint8_t RgbTab[640] = {0};
/**
* @}
*/

/** @defgroup HX8347D_Private_FunctionPrototypes Private Functions Prototypes
* @{
*/
static int32_t HX8347D_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length);
static int32_t HX8347D_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length);
static int32_t HX8347D_IO_Delay(HX8347D_Object_t *pObj, uint32_t Delay);
/**
* @}
*/

/** @addtogroup HX8347D_Exported_Functions
* @{
*/

/**
  * @brief  Register component IO bus
  * @param  pObj Component object pointer
  * @param  pIO  Component IO structure pointer
  * @retval Component status
  */
int32_t HX8347D_RegisterBusIO(HX8347D_Object_t *pObj, HX8347D_IO_t *pIO)
{
  int32_t ret;

  if(pObj == NULL)
  {
    ret = HX8347D_ERROR;
  }
  else
  {
    pObj->IO.Init           = pIO->Init;
    pObj->IO.DeInit         = pIO->DeInit;
    pObj->IO.WriteReg       = pIO->WriteReg;
    pObj->IO.ReadReg        = pIO->ReadReg;
    pObj->IO.GetTick        = pIO->GetTick;

    pObj->Ctx.ReadReg       = HX8347D_ReadRegWrap;
    pObj->Ctx.WriteReg      = HX8347D_WriteRegWrap;
    pObj->Ctx.handle    = pObj;
  }

  if (pObj->IO.Init != NULL)
  {
      ret = pObj->IO.Init();
  }
  else
  {
     ret = HX8347D_ERROR;
  }
  return ret;
}

/**
  * @brief  Initialise the HX8347D LCD Component.
  * @param  pObj Component object
  * @param  ColorCoding RGB mode
  * @param  Orientation Display orientation
  * @retval Component status
*/
int32_t HX8347D_Init(HX8347D_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation)
{
  int32_t ret;
  uint8_t pdata;
  volatile uint8_t data;

  if((pObj == NULL) || (Orientation > HX8347D_ORIENTATION_LANDSCAPE_ROT180))
  {
    ret = HX8347D_ERROR;
  }
  else if (pObj->IsInitialized == 0)
  {
    pObj->IsInitialized = 1;
    ret = HX8347D_OK;
    /* Driving ability setting */
    pdata = 0x00;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL_INTERNAL_USED1, &pdata, 1);
    pdata = 0x20;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL_INTERNAL_USED2, &pdata, 1);
    pdata = 0x0C;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_SOURCE_CTRL_INTERNAl_USED1, &pdata, 1);
    pdata = 0xC4;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_SOURCE_CTRL_INTERNAl_USED2, &pdata, 1);
    pdata = 0x40;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_SOURCE_OP_CTRL1, &pdata, 1);
    pdata = 0x38;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_SOURCE_OP_CTRL2, &pdata, 1);
    pdata = 0xA3;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL2, &pdata, 1);

    /* Adjust the Gamma Curve */
    pdata = 0x01;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL1, &pdata, 1);
    pdata = 0x00;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL2, &pdata, 1);
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL3, &pdata, 1);
    pdata = 0x10;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL4, &pdata, 1);
    pdata = 0x0E;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL5, &pdata, 1);
    pdata = 0x24;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL6, &pdata, 1);
    pdata = 0x04;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL7, &pdata, 1);
    pdata = 0x50;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL8, &pdata, 1);
    pdata = 0x02;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL9, &pdata, 1);
    pdata = 0x13;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL10, &pdata, 1);
    pdata = 0x19;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL11, &pdata, 1);
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL12, &pdata, 1);
    pdata = 0x16;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL13, &pdata, 1);
    pdata = 0x1B;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL14, &pdata, 1);
    pdata = 0x31;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL15, &pdata, 1);
    pdata = 0x2F;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL16, &pdata, 1);
    pdata = 0x3F;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL17, &pdata, 1);
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL18, &pdata, 1);
    pdata = 0x3E;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL19, &pdata, 1);
    pdata = 0x2F;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL20, &pdata, 1);
    pdata = 0x7B;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL21, &pdata, 1);
    pdata = 0x09;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL22, &pdata, 1);
    pdata = 0x06;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL23, &pdata, 1);
    pdata = 0x06;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL24, &pdata, 1);
    pdata = 0x0C;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL25, &pdata, 1);
    pdata = 0x1D;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL26, &pdata, 1);
    pdata = 0xCC;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_GAMMA_CTRL27, &pdata, 1);

    /* Power voltage setting */
    pdata = 0x1B;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL2, &pdata, 1);
    pdata = 0x01;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL1, &pdata, 1);
    pdata = 0x2F;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_VCOM_CTRL2, &pdata, 1);
    pdata = 0x57;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_VCOM_CTRL3, &pdata, 1);
    /*****VCOM offset ****/
    pdata = 0x86;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_VCOM_CTRL1, &pdata, 1);

    /* Power on setting up flow */
    pdata = 0x36;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL1, &pdata, 1); /* Display frame rate = 70Hz RADJ = '0110' */
    pdata = 0x01;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL2, &pdata, 1); /* OSC_EN = 1 */
    pdata = 0x06;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL3, &pdata, 1); /* AP[2:0] = 111 */
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL4, &pdata, 1); /* AP[2:0] = 111 */
    pdata = 0x90;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1); /* GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0*/
    pdata = 0x01;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL1, &pdata, 1); /* REF = 1 */

    (void)HX8347D_IO_Delay(pObj, 10);
    /* 262k/65k color selection */
    pdata = 0x05;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLMOD_CTRL, &pdata, 1); /* default 0x06 262k color,  0x05 65k color */
    /* SET PANEL */
    pdata = 0x09;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_PANEL_CTRL, &pdata, 1); /* SS_PANEL = 1, GS_PANEL = 0,REV_PANEL = 0, BGR_PANEL = 1 */

#if 0
    /* Display ON flow */
    pdata = 0x38;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1); /* GON=1, DTE=1, D=10 */
    (void)HX8347D_IO_Delay(pObj, 100);
    pdata = 0x3C;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1); /* GON=1, DTE=1, D=11 */
    (void)HX8347D_IO_Delay(pObj, 100);
#else
  volatile uint8_t *data;

  pdata = 0x38U;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
  pdata = 0x3CU;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
#endif
    /* Set GRAM Area - Partial Display Control */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_MODE_CTRL, &pdata, 1); /* DP_STB = 0, DP_STB_S = 0, SCROLL = 0, */
    /* Set Display orientation */
    ret += HX8347D_SetOrientation(pObj, Orientation);

    /* Set the Cursor */
    ret += HX8347D_SetCursor(pObj, 0, 0);

    /* Prepare to write GRAM */
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, &pdata, 1);
  }

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  De-Initialize the st7735 LCD Component.
  * @param  pObj Component object
  * @retval Component status
  */
int32_t HX8347D_DeInit(HX8347D_Object_t *pObj)
{
  int32_t ret = HX8347D_OK;
  uint8_t pdata;

  if(pObj->IsInitialized != 0U)
  {
    ret += HX8347D_DisplayOff(pObj);

    /* Power Off sequence ---------------------------------------------------------*/
    ret += hx8347d_read_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1);
    pdata &= 0xAFU;
    pdata |= 0x08U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1); /* PON = 0, VCOMG = 0 and DK = 1 */
    pdata |= 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1); /* STB = 1 */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL2, &pdata, 1); /* OSC_EN = 0 */

    pObj->IsInitialized = 0;
  }

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Get the hx8347d ID.
  * @param  pObj Component object
  * @param  Id Component ID
  * @retval The component status
  */
int32_t HX8347D_ReadID(HX8347D_Object_t *pObj, uint32_t *Id)
{
  int32_t ret;
  uint8_t hx8347d_id[2];

  /* Get ID from component */
  ret = hx8347d_read_reg(&pObj->Ctx, HX8347D_HIMAX_ID, hx8347d_id, 1);

  *Id = (uint32_t)hx8347d_id[0] | ((uint32_t)hx8347d_id[1] << 8U);

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Enables the Display.
  * @param  pObj Component object
  * @retval The component status
  */
int32_t HX8347D_DisplayOn(HX8347D_Object_t *pObj)
{
  int32_t ret = HX8347D_OK;
  uint8_t pdata;

  /* Power On sequence ---------------------------------------------------------*/
  pdata = 0x36U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL1, &pdata, 1); /* Display frame rate = 70Hz RADJ = '0110' */
  pdata = 0x01U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL2, &pdata, 1); /* OSC_EN = 1 */
  pdata = 0x06U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL3, &pdata, 1); /* AP[2:0] = 111 */
  pdata = 0x90U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1); /* GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0*/
  (void)HX8347D_IO_Delay(pObj, 10);
  /* 262k/65k color selection */
  pdata = 0x05U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLMOD_CTRL, &pdata, 1); /* default 0x06 262k color,  0x05 65k color */
  /* SET PANEL */
  pdata = 0x09U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_PANEL_CTRL, &pdata, 1); /* SS_PANEL = 1, GS_PANEL = 0,REV_PANEL = 0, BGR_PANEL = 1 */
#if 1
  /* Display On */
  pdata = 0x38U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1);
  (void)HX8347D_IO_Delay(pObj, 60);
  pdata = 0x3CU;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1);
#else
  volatile uint8_t *data;

  pdata = 0x38U;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
  pdata = 0x3CU;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
#endif
  if (ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }
  return ret;
}

/**
  * @brief  Disables the Display.
  * @param  pObj Component object
  * @retval The component status
  */
int32_t HX8347D_DisplayOff(HX8347D_Object_t *pObj)
{
  int32_t ret = HX8347D_OK;
  uint8_t pdata;

  /* Power Off sequence ---------------------------------------------------------*/
  pdata = 0x00U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLMOD_CTRL, &pdata, 1); /* default 0x06 262k color,  0x05 65k color */
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL1, &pdata, 1); /* Display frame rate = 70Hz RADJ = '0110' */
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_OSC_CTRL2, &pdata, 1); /* OSC_EN = 1 */
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL3, &pdata, 1); /* AP[2:0] = 111 */
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_POWER_CTRL6, &pdata, 1); /* GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0*/
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_PANEL_CTRL, &pdata, 1); /* SS_PANEL = 1, GS_PANEL = 0,REV_PANEL = 0, BGR_PANEL = 1 */

#if 0
  /* Display Off */
  pdata = 0x38U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1);
  (void)HX8347D_IO_Delay(pObj, 60);
  pdata = 0x04U;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, &pdata, 1);
#else
  volatile uint8_t *data;

  pdata = 0x38U;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
  pdata = 0x04U;
  data = (uint8_t *)&pdata;
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_DISPLAY_CTRL3, (uint8_t *)data, 1);
  (void)HX8347D_IO_Delay(pObj, 100);
#endif

  if (ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }
  return ret;
}

/**
  * @brief  Set the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be set
  * @retval Component status
  */
int32_t HX8347D_SetBrightness(HX8347D_Object_t *pObj, uint32_t Brightness)
{
  /* Feature not supported */
  (void)pObj;
  (void)Brightness;
  return HX8347D_ERROR;
}

/**
  * @brief  Get the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be returned
  * @retval Component status
  */
int32_t HX8347D_GetBrightness(HX8347D_Object_t *pObj, uint32_t *Brightness)
{
  /* Feature not supported */
  (void)pObj;
  (void)Brightness;
  return HX8347D_ERROR;
}

/**
  * @brief  Set the Display Orientation.
  * @param  pObj Component object
  * @param  Orientation HX8347D_ORIENTATION_PORTRAIT, HX8347D_ORIENTATION_PORTRAIT_ROT180
  *                     SHX8347D_ORIENTATION_LANDSCAPE or HX8347D_ORIENTATION_LANDSCAPE_ROT180
  * @retval The component status
  */
int32_t HX8347D_SetOrientation(HX8347D_Object_t *pObj, uint32_t Orientation)
{
  int32_t  ret = HX8347D_OK;
  uint8_t  pdata;


  /* Update GRAM Area - Partial Display Control */
  if ((Orientation == HX8347D_ORIENTATION_PORTRAIT) || (Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    /* 320 lines */
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    /* 240 columns */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }
  else
  {
    /* 240 lines */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    /* 320 columns */
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }

  pdata = OrientationTab[Orientation][0];
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_MEMORY_ACCESS_CTRL, &pdata, 1);

  if (ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }
  else
  {
    HX8347DCtx.Orientation   = Orientation;
  }

  return ret;
}

/**
  * @brief  Set the Display Orientation.
  * @param  pObj Component object
  * @param  Orientation HX8347D_ORIENTATION_PORTRAIT, HX8347D_ORIENTATION_LANDSCAPE
  *                      or HX8347D_ORIENTATION_LANDSCAPE_ROT180
  * @retval The component status
  */
int32_t HX8347D_GetOrientation(HX8347D_Object_t *pObj, uint32_t *Orientation)
{
  *Orientation = HX8347DCtx.Orientation;

  return HX8347D_OK;
}

/**
  * @brief  Set Cursor position.
  * @param  pObj Component object
  * @param  Xpos specifies the X position.
  * @param  Ypos specifies the Y position.
  * @retval The component status
  */
int32_t HX8347D_SetCursor(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos)
{
  int32_t ret = HX8347D_OK;
  uint8_t pdata;

  pdata = (uint8_t) (Ypos >> 8);
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_START2, &pdata, 1);
  pdata = (uint8_t) (Ypos & 0xFFU);
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_START1, &pdata, 1);
  pdata = (uint8_t) (Xpos >> 8);
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_START2, &pdata, 1);
  pdata = (uint8_t) (Xpos & 0xFFU);
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_START1, &pdata, 1);

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Displays a bitmap picture.
  * @param  pObj Component object
  * @param  Xpos Bmp X position in the LCD
  * @param  Ypos Bmp Y position in the LCD
  * @param  pBmp Bmp picture address.
  * @retval The component status
  */

int32_t HX8347D_DrawBitmap(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  int32_t  ret = HX8347D_OK;
  uint32_t index, size, i, j;
  uint32_t width, height;

  /* Get bitmap data address offset */
  index = (uint32_t)pBmp[10] + ((uint32_t)pBmp[11] << 8) + ((uint32_t)pBmp[12] << 16)  + ((uint32_t)pBmp[13] << 24);

  /* Read bitmap width */
  width = (uint32_t)pBmp[18] + ((uint32_t)pBmp[19] << 8) + ((uint32_t)pBmp[20] << 16)  + ((uint32_t)pBmp[21] << 24);

  /* Read bitmap height */
  height = (uint32_t)pBmp[22] + ((uint32_t)pBmp[23] << 8) + ((uint32_t)pBmp[24] << 16)  + ((uint32_t)pBmp[25] << 24);

  /* Read file size */
  size = ((uint32_t)pBmp[5] << 24) | ((uint32_t)pBmp[4] << 16) | ((uint32_t)pBmp[3] << 8) | (uint32_t)pBmp[2];
  size = size - index;

  pBmp = pBmp + size + index - 2*width;

  for(j = 0; j <height ; j++)
  {
    for(i = 0; i < width; i++)
    {
      RgbTab[2U*i]      = (uint8_t) (*(pBmp +1));
      RgbTab[(2U*i)+1U] = (uint8_t) (*(pBmp));
      pBmp += 2;
    }
    ret += HX8347D_SetCursor(pObj, Xpos, Ypos);
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, &RgbTab[0], (uint16_t) 2*width);

    Ypos++;
    pBmp -= 4*width;
  }

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Draw horizontal line.
  * @param  pObj Component object
  * @param  Xpos   specifies the X position.
  * @param  Ypos   specifies the Y position.
  * @param  Length specifies the Line length.
  * @param  Color  Specifies the RGB color in RGB565 format
  * @retval The component status
  */
int32_t HX8347D_FillRGBRect(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  int32_t  ret = HX8347D_OK;
  uint32_t i, j;

  for(j = 0; j < Height; j++)
  {

    /* Set Cursor */
    ret += HX8347D_SetCursor(pObj, Xpos, Ypos+j);

    /* Sent a complete line */
    for(i = 0; i < Width; i++)
    {
      RgbTab[2U*i]      = (uint8_t) (*(pData+1));
      RgbTab[(2U*i)+1U] = (uint8_t) (*pData);
      pData += 2;
    }

    /* Sent a complete line */
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, &RgbTab[0], Width * 2);
  }

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}


/**
  * @brief  Draw horizontal line.
  * @param  pObj Component object
  * @param  Xpos   specifies the X position.
  * @param  Ypos   specifies the Y position.
  * @param  Length specifies the Line length.
  * @param  Color  Specifies the RGB color in RGB565 format
  * @retval The component status
  */
int32_t HX8347D_DrawHLine(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t  ret = HX8347D_OK;
  uint32_t i;

  /* Set Cursor */
  ret += HX8347D_SetCursor(pObj, Xpos, Ypos);

  /* Sent a complete line */
  for(i = 0; i < Length; i++)
  {
    RgbTab[2U*i]      = (uint8_t) ((Color >> 8) & 0xFFU);
    RgbTab[(2U*i)+1U] = (uint8_t) (Color & 0xFFU);
  }
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, RgbTab, Length * 2);

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Draw vertical line.
  * @param  pObj Component object
  * @param  Color    Specifies the RGB color
  * @param  Xpos     specifies the X position.
  * @param  Ypos     specifies the Y position.
  * @param  Length   specifies the Line length.
  * @retval The component status
  */
int32_t HX8347D_DrawVLine(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t  ret = HX8347D_OK;
  uint8_t  pdata;
  uint32_t i;


  /* Invert row and column end addresses */
  if ((HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT) || (HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }
  else
  {
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }

  if ((HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT) || (HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    pdata = OrientationTab[HX8347DCtx.Orientation][0] | 0x20U;
  }
  else
  {
    pdata = OrientationTab[HX8347DCtx.Orientation][0] & 0xDFU;
  }

  /* Memory access control: Invert MV */
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_MEMORY_ACCESS_CTRL, &pdata, 1);

  /* Set Cursor */
  ret += HX8347D_SetCursor(pObj, Ypos, Xpos);

  /* Sent a complete line */
  for(i = 0; i < Length; i++)
  {
    RgbTab[2U*i]      = (uint8_t) ((Color >> 8) & 0xFFU);
    RgbTab[(2U*i)+1U] = (uint8_t) (Color & 0xFFU);
  }
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, RgbTab, 2*Length);

  /* Restore row and column end addresses */
  if ((HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT) || (HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }
  else
  {
    pdata = 0x00U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END2, &pdata, 1); /* EP */
    pdata = 0xEFU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_ROW_ADDRESS_END1, &pdata, 1); /* EP */
    pdata = 0x01U;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END2, &pdata, 1); /* EC */
    pdata = 0x3FU;
    ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_COLUMN_ADDRESS_END1, &pdata, 1); /* EC */
  }

  /* Memory access control: Re-invert MV */
  pdata = OrientationTab[HX8347DCtx.Orientation][0];
  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_MEMORY_ACCESS_CTRL, &pdata, 1);

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Fill rectangle
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Width Rectangle width
  * @param  Height Rectangle height
  * @param  Color Draw color
  * @retval Component status
  */
int32_t HX8347D_FillRect(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  int32_t ret = HX8347D_OK;
  uint32_t i;

  for(i = 0U; i < Height; i++)
  {
    if (HX8347D_DrawHLine(pObj, Xpos, (i + Ypos), Width, Color) != HX8347D_OK)
    {
      ret = HX8347D_ERROR;
      break;
    }
  }

  return ret;
}

/**
  * @brief  Write pixel.
  * @param  pObj Component object
  * @param  Xpos specifies the X position.
  * @param  Ypos specifies the Y position.
  * @param  Color the RGB pixel color in RGB565 format
  * @retval The component status
  */
int32_t HX8347D_SetPixel(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  int32_t  ret = HX8347D_OK;

  /* Set Cursor */
  ret += HX8347D_SetCursor(pObj, Xpos, Ypos);

  RgbTab[0] = (uint8_t) ((Color >> 8) & 0xFFU);
  RgbTab[1] = (uint8_t) (Color & 0xFFU);

  ret += hx8347d_write_reg(&pObj->Ctx, HX8347D_READ_DATA, RgbTab, 2);

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Read pixel.
  * @param  pObj Component object
  * @param  Xpos specifies the X position.
  * @param  Ypos specifies the Y position.
  * @param  Color the RGB pixel color in RGB565 format
  * @retval The component status
  */
int32_t HX8347D_GetPixel(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  int32_t  ret = HX8347D_OK;
  uint8_t  pdata[6];

  /* Set Cursor */
  ret += HX8347D_SetCursor(pObj, Xpos, Ypos);

  /* read pixel */
  ret += hx8347d_read_reg(&pObj->Ctx, HX8347D_READ_DATA, pdata, 3);

  /* Set color in RGB565 format */
  *Color = ((((uint32_t)pdata[3] << 8) & 0xF800U) |
            (((uint32_t)pdata[2] << 3) & 0x07E0U) |
            (((uint32_t)pdata[5] >> 3) & 0x001FU));

  if(ret != HX8347D_OK)
  {
    ret = HX8347D_ERROR;
  }

  return ret;
}

/**
  * @brief  Get the LCD pixel Width.
  * @param  pObj Component object
  * @param  The Lcd Pixel Width
  * @retval The component status
  */
int32_t HX8347D_GetXSize(HX8347D_Object_t *pObj, uint32_t *XSize)
{
  int32_t  ret = HX8347D_OK;

  if ((HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT) || (HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    *XSize = 240;
  }
  else
  {
    *XSize = 320;
  }

  return ret;
}

/**
  * @brief  Get the LCD pixel Height.
  * @param  pObj Component object
  * @param  The Lcd Pixel Height
  * @retval The component status
  */
int32_t HX8347D_GetYSize(HX8347D_Object_t *pObj, uint32_t *YSize)
{
  int32_t  ret = HX8347D_OK;

  if ((HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT) || (HX8347DCtx.Orientation == HX8347D_ORIENTATION_PORTRAIT_ROT180))
  {
    *YSize = 320;
  }
  else
  {
    *YSize = 240;
  }

  return ret;
}

/** @defgroup ST7735_Private_Functions  Private Functions
  * @{
  */


/**
  * @brief  Read register wrapped function.
  * @param  handle  Component object handle.
  * @param  Reg     The target register address to read.
  * @param  pData   The target register value to be red.
  * @param  Length  Buffer size to be red.
  * @retval error status.
  */
static int32_t HX8347D_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  HX8347D_Object_t *pObj = (HX8347D_Object_t *)handle;

  return pObj->IO.ReadReg(Reg, pData, Length);
}

/**
  * @brief  Write register wrapped function.
  * @param  handle Component object handle.
  * @param  Reg    The target register address to write.
  * @param  pData  The target register value to be written.
  * @param  Length Buffer size to be written.
  * @retval error status.
  */
static int32_t HX8347D_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  HX8347D_Object_t *pObj = (HX8347D_Object_t *)handle;

  return pObj->IO.WriteReg(Reg, pData, Length);
}

/**
  * @brief  HX8347D delay
  * @param  Delay  Delay in ms
  * @retval Component error status
  */
static int32_t HX8347D_IO_Delay(HX8347D_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;
  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return HX8347D_OK;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
