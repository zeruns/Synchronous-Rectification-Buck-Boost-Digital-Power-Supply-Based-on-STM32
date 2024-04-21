/**
  ******************************************************************************
  * @file    stm32g474e_eval_lcd.c
  * @author  MCD Application Team
  * @brief   This file includes the driver for Liquid Crystal Display (LCD) module
  *          mounted on STM32G474E-EVAL evaluation board.
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

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive indirectly an LCD TFT.
   - This driver supports the HX8347D LCD mounted on MB895 daughter board.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the LCD using the BSP_LCD_Init() function.

  + Display on LCD
     o Clear the whole LCD using the BSP_LCD_Clear() function.
     o Display a character on the specified line and column using the GUI_DisplayChar()
       function or a complete string line using the GUI_DisplayStringAtLine() function.
     o Display a string line on the specified position (x,y in pixel) and align mode
       using the GUI_DisplayStringAtLine() function.
     o Draw and fill a basic shapes (dot, line, rectangle, circle, ellipse, .. bitmap, raw picture)
       on LCD using a set of functions.

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_lcd.h"
#include "stm32g474e_eval_io.h"
#include "stm32g474e_eval_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_LCD STM32G474E-EVAL LCD
  * @brief      This file includes the LCD driver of
  *             STM32G474E-EVAL boards.
  * @{
  */

/** @defgroup STM32G474E-EVAL_LCD_Private_Constants Private Constants
  * @{
  */

/* LINK LCD */
#define START_BYTE_SET_INDEX    0x70U
#define START_BYTE_WRITE        0x72U
#define START_BYTE_READ         0x73U

/* LINK GUI */
const GUI_Drv_t LCD_Driver =
{
  BSP_LCD_DrawBitmap,
  BSP_LCD_FillRGBRect,
  BSP_LCD_DrawHLine,
  BSP_LCD_DrawVLine,
  BSP_LCD_FillRect,
  BSP_LCD_ReadPixel,
  BSP_LCD_WritePixel,
  BSP_LCD_GetXSize,
  BSP_LCD_GetYSize,
  NULL,
  BSP_LCD_GetPixelFormat
};
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Private_Macros Private Macros
  * @{
  */
#define POLY_X(Z)               ((int32_t)((Points + (Z))->X))
#define POLY_Y(Z)               ((int32_t)((Points + (Z))->Y))

#define ABS(X)  (((X) > 0U) ? (X) : -(X))

/* LCD DeSELECTion */
#define LCD_CS_HIGH()    {__DMB();HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_SET);__DMB();}
/* LCD SELECTion */
#define LCD_CS_LOW()     {__DMB();HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_RESET);__DMB();}
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Exported_Variables Exported Variables
  * @{
  */
void                 *LcdCompObj = NULL;
BSP_LCD_Ctx_t        LcdCtx[LCD_INSTANCES_NBR];
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Private_Variables Private Variables
  * @{
  */
static LCD_Drv_t     *LcdDrv = NULL;
/**
  * @}
  */

/** STM32G474E-EVAL_LCD_Private_FunctionPrototypes Private Functions
  */
/*******************************************************************************
 ********************************* LINK LCD ***********************************/
static int32_t LCD_IO_Init(void);
static int32_t LCD_IO_DeInit(void);

#if (USE_LCD_CTRL_HX8347D == 1)
static int32_t HX8347D_Probe(uint32_t Orientation);
#endif /* USE_LCD_CTRL_HX8347D */
/**
  */

/** @addtogroup STM32G474E-EVAL_LCD_Exported_Functions
  * @{
  */
/**
  * @brief  Initializes the LCD.
  * @param  Instance    LCD Instance
  * @param  Orientation LCD_ORIENTATION_PORTRAIT or LCD_ORIENTATION_LANDSCAPE
  *                     or LCD_ORIENTATION_PORTRAIT_ROT180
  *                     or LCD_ORIENTATION_LANDSCAPE_ROT180
  * @retval BSP status
  */
int32_t BSP_LCD_Init(uint32_t Instance, uint32_t Orientation)
{
  int32_t ret = BSP_ERROR_NONE;

  if ((Instance >= LCD_INSTANCES_NBR ) || (Orientation > LCD_ORIENTATION_LANDSCAPE_ROT180))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((Orientation == LCD_ORIENTATION_LANDSCAPE) || (Orientation == LCD_ORIENTATION_LANDSCAPE_ROT180))
    {
      LcdCtx[Instance].Width  = LCD_DEFAULT_WIDTH;
      LcdCtx[Instance].Height = LCD_DEFAULT_HEIGHT;
    }
    else
    {
      LcdCtx[Instance].Width  = LCD_DEFAULT_HEIGHT;
      LcdCtx[Instance].Height = LCD_DEFAULT_WIDTH;
    }
       /* registers the function and initialize the controller */
       if(HX8347D_Probe(Orientation) != BSP_ERROR_NONE)
       {
         ret = BSP_ERROR_UNKNOWN_COMPONENT;
       }
  }

  return ret;
}

/**
  * @brief  De-Initializes the LCD resources.
  * @param  Instance LCD Instance
  * @retval BSP status
  */
int32_t BSP_LCD_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(LcdDrv->DeInit(LcdCompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  LCD_MspDeInit();

  return ret;
}

/**
  * @brief  Gets the LCD Active LCD Pixel Format.
  * @param  Instance    LCD Instance
  * @param  PixelFormat Active LCD Pixel Format
  * @retval BSP status
  */
int32_t BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  }

  return ret;
}

/**
  * @brief  Gets the LCD X size.
  * @param  Instance LCD Instance
  * @param  pXSize pointer to Used LCD X size
  * @retval BSP status
  */
int32_t BSP_LCD_GetXSize(uint32_t Instance, uint32_t *pXSize)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->GetXSize != NULL)
  {
    if(LcdDrv->GetXSize(LcdCompObj, pXSize) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    *pXSize = LcdCtx[Instance].Width;
  }
  return ret;
}

/**
  * @brief  Gets the LCD Y size.
  * @param  Instance LCD Instance
  * @param  pYSize pointer to Used LCD Y size
  * @retval BSP status
  */
int32_t BSP_LCD_GetYSize(uint32_t Instance, uint32_t *pYSize)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->GetYSize != NULL)
  {
    if(LcdDrv->GetYSize(LcdCompObj, pYSize) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    *pYSize = LcdCtx[Instance].Height;
  }
  return ret;
}

/**
  * @brief  Switch On the display.
  * @param  Instance    LCD Instance
  * @retval BSP status
  */
int32_t BSP_LCD_DisplayOn(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->DisplayOn != NULL)
  {
    if(LcdDrv->DisplayOn(LcdCompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Switch Off the display.
  * @param  Instance    LCD Instance
  * @retval BSP status
  */
int32_t BSP_LCD_DisplayOff(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->DisplayOff != NULL)
  {
    if(LcdDrv->DisplayOff(LcdCompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Set the brightness value
  * @param  Instance    LCD Instance
  * @param  Brightness [00: Min (black), 100 Max]
  * @retval BSP status
  */
int32_t BSP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->SetBrightness != NULL)
  {
    if(LcdDrv->SetBrightness(LcdCompObj, Brightness) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  return ret;
}

/**
  * @brief  Get the brightness value
  * @param  Instance    LCD Instance
  * @param  Brightness [00: Min (black), 100 Max]
  * @retval BSP status
  */
int32_t BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->GetBrightness != NULL)
  {
    if(LcdDrv->GetBrightness(LcdCompObj, Brightness) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Set the LCD Orientation.
  * @param  Instance    LCD Instance
  * @param  Orientation LCD orientation to set
  * @retval BSP status
  */
int32_t  BSP_LCD_SetOrientation(uint32_t Instance, uint32_t Orientation)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->SetOrientation != NULL)
  {
    if(LcdDrv->SetOrientation(LcdCompObj, Orientation) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Get the LCD orientation.
  * @param  Instance    LCD Instance
  * @param  Orientation LCD Orientation used
  * @retval BSP status
  */
int32_t  BSP_LCD_GetOrientation(uint32_t Instance, uint32_t *Orientation)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->GetOrientation != NULL)
  {
    if(LcdDrv->GetOrientation(LcdCompObj, Orientation) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Get the LCD orientation.
  * @param  Instance    LCD Instance
  * @param  Xpos to set the cursor
  * @param  Ypos to set the cursor
  * @retval BSP status
  */
int32_t  BSP_LCD_SetCursor(uint32_t Instance, uint32_t Xpos, uint32_t Ypos)
{
 int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->SetCursor != NULL)
  {
    if(LcdDrv->SetCursor(LcdCompObj, Xpos, Ypos) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Draws a bitmap picture loaded in the internal Flash in currently active layer.
  * @param  Instance LCD Instance
  * @param  Xpos Bmp X position in the LCD
  * @param  Ypos Bmp Y position in the LCD
  * @param  pBmp Pointer to Bmp picture address in the internal Flash
  * @retval BSP status
  */
int32_t BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->DrawBitmap != NULL)
  {
    /* Draw the bitmap on LCD */
    if (LcdDrv->DrawBitmap(LcdCompObj, Xpos, Ypos, pBmp) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Fill a rectangle with a BitMap on LCD.
  * @param  Instance LCD Instance.
  * @param  pData Pointer to RGB line data
  * @param  Xpos X position.
  * @param  Ypos Y position.
  * @param  Width  width of the rectangle to fill.
  * @param  Height height of the rectangle to fill.
  * @retval BSP status.
  */
int32_t BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->FillRGBRect != NULL)
  {
    /* Fill the Rectangle lines on LCD */
    if (LcdDrv->FillRGBRect(LcdCompObj, Xpos, Ypos, pData, Width, Height) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Draws an horizontal line
  * @param  Instance LCD instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Length Line length
  * @param  Color Line color
  * @retval BSP status
  */
int32_t BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }

  else if(LcdDrv->DrawHLine != NULL)
  {
    /* Draw the horizontal line on LCD */
    if (LcdDrv->DrawHLine(LcdCompObj, Xpos, Ypos, Length, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Draws a vertical line
  * @param  Instance LCD instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Length Line length
  * @param  Color Line color
  * @retval BSP status
  */
int32_t BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(LcdDrv->DrawVLine != NULL)
  {
    /* Draw the vertical line on LCD */
    if (LcdDrv->DrawVLine(LcdCompObj, Xpos, Ypos, Length, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Draws a full rectangle in currently active layer.
  * @param  Instance LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Width Rectangle width
  * @param  Height Rectangle height
  * @param  Color Color of rectangle
  * @retval BSP status
  */
int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(LcdDrv->FillRect(LcdCompObj, Xpos, Ypos, Width, Height, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  clear the LCD in currently active layer.
  * @param  Instance LCD Instance
  * @param  Color to set
  * @retval BSP status
  */
int32_t BSP_LCD_Clear(uint32_t Instance, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(LcdDrv->FillRect(LcdCompObj, 0, 0, LcdCtx[Instance].Width, LcdCtx[Instance].Height, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}


/**
  * @brief  Reads a LCD pixel color.
  * @param  Instance LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color pointer to RGB pixel color
  * @retval BSP status
  */
int32_t  BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(LcdDrv->GetPixel(LcdCompObj, Xpos, Ypos, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Writes a LCD pixel.
  * @param  Instance LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color RGB pixel color
  * @retval BSP status
  */
int32_t  BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(LcdDrv->SetPixel(LcdCompObj, Xpos, Ypos, Color) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}


/**
  * @brief  Writes register on LCD register.
  * @param  Reg    Register to be written
  * @param  pData  pointer to the read data from LCD GRAM.
  * @param  Length length of data be read from the LCD GRAM
  * @retval BSP status
  */
int32_t BSP_LCD_WriteReg(uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  uint8_t pdata[2] = {(uint8_t)START_BYTE_SET_INDEX, (uint8_t)Reg};
  /* Reset LCD control line(/CS) and Send command */
  LCD_CS_LOW();

  if(BSP_SPI2_Send(pdata, 2) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  /* Deselect : Chip Select high */
  LCD_CS_HIGH();

  /* Send Data */
  if((ret == BSP_ERROR_NONE) && (Length > 0U))
  {
    if(BSP_LCD_SendData(pData, Length) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Send data to select the LCD GRAM.
  * @param  pData  pointer to data to write to LCD GRAM.
  * @param  Length length of data to write to LCD GRAM
  * @retval Error status
  */
int32_t BSP_LCD_SendData(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t pdata = START_BYTE_WRITE;

  /* Reset LCD control line CS */
  LCD_CS_LOW();

  /* Send Start Byte */
  if(BSP_SPI2_Send(&pdata, 1)!= BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }
  else
  {
    /* Send Data */
    if(BSP_SPI2_Send(pData, Length)!= BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }
  /* Deselect : Chip Select high */
  LCD_CS_HIGH();

  return ret;
}

/**
  * @brief  Read data from LCD data register.
  * @param  Reg    Register to be read
  * @param  pData  pointer to the read data from LCD GRAM.
  * @param  Length length of data be read from the LCD GRAM
  * @retval BSP status
  */
int32_t BSP_LCD_ReadReg(uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;
  uint16_t readvalue;
  uint8_t data[2];
  UNUSED(Length);

  /* Send Reg value to Read */
  if(BSP_LCD_WriteReg(Reg, pData, 0) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  /* Reset LCD control line(/CS) and Send command */
  LCD_CS_LOW();

  /* Send Start Byte */
  data[0] = START_BYTE_READ;
  data[1] = 0xFF;

  if (ret == BSP_ERROR_NONE)
  {
    if(BSP_SPI2_Send(data, 2) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(BSP_SPI2_Recv(data, 2) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      readvalue = (uint16_t)(data[0]) << 8;
      readvalue |= (uint16_t)data[1];
      HAL_Delay(10);
      *pData = (uint8_t)readvalue;
    }
  }
  /* Deselect : Chip Select high */
  LCD_CS_HIGH();

  return ret;
}

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Private_Functions Private Functions
  * @{
  */

/*******************************************************************************
 ********************************* LINK LCD ***********************************/

/**
  * @brief  Initialize the BSP LTDC Msp.
  * @retval None
  */
static void LCD_MspInit(void)
{
    /* turn LCD on = drive pin low (active low) */
    LCD_CS_LOW();
}

/**
  * @brief  De-Initializes the BSP LTDC Msp
  * @retval None
  */
static void LCD_MspDeInit(void)
{
    /* turn LCD off = drive pin high (active low) */
    LCD_CS_HIGH();
}

#if (USE_LCD_CTRL_HX8347D == 1)
/**
  * @brief  Register Bus IOs for instance 0 if HX8347D ID is OK
  * @param  Orientation
  * @retval BSP status
  */
static int32_t HX8347D_Probe(uint32_t Orientation)
{
  int32_t                 ret = BSP_ERROR_NONE;
  HX8347D_IO_t            IOCtx;
  static HX8347D_Object_t HX8347DObj;

  /* Configure the lcd driver : map to LCD_IO function*/
  IOCtx.Init             = LCD_IO_Init;
  IOCtx.DeInit           = LCD_IO_DeInit;
  IOCtx.ReadReg          = BSP_LCD_ReadReg;
  IOCtx.WriteReg         = BSP_LCD_WriteReg;
  IOCtx.GetTick          = BSP_GetTick;

  if(HX8347D_RegisterBusIO(&HX8347DObj, &IOCtx) != HX8347D_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    LcdCompObj = &HX8347DObj;

    LCD_MspInit();

    /* LCD Initialization */
    LcdDrv = (LCD_Drv_t *)&HX8347D_Driver;

    if(LcdDrv->Init(LcdCompObj, HX8347D_FORMAT_DEFAULT, Orientation) != HX8347D_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif

/**
  * @brief  Initializes LCD low level.
  * @retval BSP status
  */
static int32_t LCD_IO_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Configure LCD_CS_PIN pin: LCD Card CS pin : output / OpenDrain  */
  LCD_CS_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = LCD_CS_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* LCD_CS pin : GPIO C configuration : input */
  HAL_GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStruct);
  /* turn LCD on = drive pin low (active low) */
  /* LCD_CS_PIN and LCD_DC_PIN configuration is done in the MSP Init*/
  /* Set or Reset the control line */
  LCD_CS_LOW();
  LCD_CS_HIGH();

  if (BSP_SPI2_Init() != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

/**
  * @brief  DeInitializes LCD low level
  * @retval BSP status
  */
static int32_t LCD_IO_DeInit(void)
{
  HAL_GPIO_DeInit(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN);

  return BSP_ERROR_NONE;
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
