/**
  ******************************************************************************
  * @file    stm32g474e_eval_lcd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32g474e_eval_lcd.c driver.
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
#ifndef STM32G474E_EVAL_LCD_H
#define STM32G474E_EVAL_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"

#include "../Components/Common/lcd.h"

/* Include audio component Driver */
#ifndef USE_LCD_CTRL_HX8347D
#define USE_LCD_CTRL_HX8347D          1U
#endif

#if (USE_LCD_CTRL_HX8347D == 1)
/* Include audio component Driver */
#include "../Components/hx8347d/hx8347d.h"
#endif

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_LCD
  * @{
  */

/** @defgroup STM32G474E-EVAL_LCD_Exported_Constants Exported Constants
  * @{
  */
#define   LCD_INSTANCES_NBR        1U

/**
  * @brief  LCD status structure definition
  */
/* Default LCD width and height */
#define   LCD_DEFAULT_WIDTH        320U
#define   LCD_DEFAULT_HEIGHT       240U

/**
  * @brief  LCD color
  */
/* Definition of BSP LCD COLOR */
#define LCD_COLOR_RGB565_BLUE                 0x001FU
#define LCD_COLOR_RGB565_GREEN                0x07E0U
#define LCD_COLOR_RGB565_RED                  0xF800U
#define LCD_COLOR_RGB565_CYAN                 0x07FFU
#define LCD_COLOR_RGB565_MAGENTA              0xF81FU
#define LCD_COLOR_RGB565_YELLOW               0xFFE0U
#define LCD_COLOR_RGB565_LIGHTBLUE            0x841FU
#define LCD_COLOR_RGB565_LIGHTGREEN           0x87F0U
#define LCD_COLOR_RGB565_LIGHTRED             0xFC10U
#define LCD_COLOR_RGB565_LIGHTCYAN            0x87FFU
#define LCD_COLOR_RGB565_LIGHTMAGENTA         0xFC1FU
#define LCD_COLOR_RGB565_LIGHTYELLOW          0xFFF0U
#define LCD_COLOR_RGB565_DARKBLUE             0x0010U
#define LCD_COLOR_RGB565_DARKGREEN            0x0400U
#define LCD_COLOR_RGB565_DARKRED              0x8000U
#define LCD_COLOR_RGB565_DARKCYAN             0x0410U
#define LCD_COLOR_RGB565_DARKMAGENTA          0x8010U
#define LCD_COLOR_RGB565_DARKYELLOW           0x8400U
#define LCD_COLOR_RGB565_WHITE                0xFFFFU
#define LCD_COLOR_RGB565_LIGHTGRAY            0xD69AU
#define LCD_COLOR_RGB565_GRAY                 0x8410U
#define LCD_COLOR_RGB565_DARKGRAY             0x4208U
#define LCD_COLOR_RGB565_BLACK                0x0000U
#define LCD_COLOR_RGB565_BROWN                0xA145U
#define LCD_COLOR_RGB565_ORANGE               0xFD20U
/* Definition of Official ST COLOR */
#define LCD_COLOR_RGB565_ST_BLUE_DARK         0x0001U
#define LCD_COLOR_RGB565_ST_BLUE              0x01EBU
#define LCD_COLOR_RGB565_ST_BLUE_LIGHT        0x06A7U
#define LCD_COLOR_RGB565_ST_GREEN_LIGHT       0x05ECU
#define LCD_COLOR_RGB565_ST_GREEN_DARK        0x001CU
#define LCD_COLOR_RGB565_ST_YELLOW            0x07F0U
#define LCD_COLOR_RGB565_ST_BROWN             0x02C8U
#define LCD_COLOR_RGB565_ST_PINK              0x0681U
#define LCD_COLOR_RGB565_ST_PURPLE            0x02CDU
#define LCD_COLOR_RGB565_ST_GRAY_DARK         0x0251U
#define LCD_COLOR_RGB565_ST_GRAY              0x04BAU
#define LCD_COLOR_RGB565_ST_GRAY_LIGHT        0x05E7U

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI packets
 */
#define LCD_ORIENTATION_PORTRAIT         0U /*!< Portrait orientation choice of LCD screen  */
#define LCD_ORIENTATION_LANDSCAPE        2U /*!< Landscape orientation choice of LCD screen */
#define LCD_ORIENTATION_PORTRAIT_ROT180  1U /* Portrait rotated 180° orientation choice of LCD screen  */
#define LCD_ORIENTATION_LANDSCAPE_ROT180 3U /* Landscape rotated 180° orientation choice of LCD screen */

/* LCD Data/Command Interface pins = SPI_MOSI_DIR (MFX13) */
/* LCD CS pin */
#define LCD_CS_GPIO_PIN                 GPIO_PIN_9
#define LCD_CS_GPIO_PORT                GPIOC
#define LCD_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define LCD_CS_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_LCD_Exported_Variables
  * @{
  */
extern const GUI_Drv_t LCD_Driver;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Exported_Types Exported Types
  * @{
  */
typedef struct
{
  uint32_t Width;
  uint32_t Height;
  uint32_t IsMspCallbacksValid;
}BSP_LCD_Ctx_t;

/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_LCD_Private_Variables
  * @{
  */
extern void                 *LcdCompObj;
extern BSP_LCD_Ctx_t        LcdCtx[];
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_LCD_Exported_Functions Exported Functions
  * @{
  */
int32_t  BSP_LCD_Init(uint32_t Instance, uint32_t Orientation);
int32_t  BSP_LCD_DeInit(uint32_t Instance);

/* LCD generic APIs: Display control */
int32_t  BSP_LCD_DisplayOn(uint32_t Instance);
int32_t  BSP_LCD_DisplayOff(uint32_t Instance);
int32_t  BSP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness);
int32_t  BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness);
int32_t  BSP_LCD_GetXSize(uint32_t Instance, uint32_t *XSize);
int32_t  BSP_LCD_GetYSize(uint32_t Instance, uint32_t *YSize);
int32_t  BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat);
int32_t  BSP_LCD_SetOrientation(uint32_t Instance, uint32_t Orientation);
int32_t  BSP_LCD_GetOrientation(uint32_t Instance, uint32_t *Orientation);

int32_t  BSP_LCD_SetCursor(uint32_t Instance, uint32_t Xpos, uint32_t Ypos);
/* LCD generic APIs: Draw operations. This list of APIs is required for
   lcd gfx utilities */
int32_t BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
int32_t BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
int32_t BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color);

int32_t BSP_LCD_Clear(uint32_t Instance, uint32_t Color);

/* LCD generic APIs: Draw operations. This list of APIs is required for
   lcd gfx utilities */
int32_t  BSP_LCD_SetActiveLayer(uint32_t Instance, uint32_t LayerIndex);
int32_t  BSP_LCD_SetPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
int32_t  BSP_LCD_GetPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);

/* LCD specific APIs */
int32_t BSP_LCD_WriteReg(uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_LCD_ReadReg(uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_LCD_SendData(uint8_t *pData, uint16_t Length);

static void LCD_MspInit(void);
static void LCD_MspDeInit(void);
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

#endif /* STM32G474E_EVAL_LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
