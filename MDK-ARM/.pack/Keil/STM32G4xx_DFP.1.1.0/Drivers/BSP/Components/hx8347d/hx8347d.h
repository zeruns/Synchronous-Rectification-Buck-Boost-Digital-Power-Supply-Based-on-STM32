/**
  ******************************************************************************
  * @file    hx8347d.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the hx8347d.c
  *          driver.
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
#ifndef HX8347D_H
#define HX8347D_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hx8347d_regs.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup HX8347D
  * @{
  */

/** @defgroup HX8347D_Exported_Types Exported Types
  * @{
  */
typedef int32_t (*HX8347D_Init_Func)     (void);
typedef int32_t (*HX8347D_DeInit_Func)   (void);
typedef int32_t (*HX8347D_GetTick_Func)  (void);
typedef int32_t (*HX8347D_Delay_Func)    (uint32_t);
typedef int32_t (*HX8347D_WriteReg_Func)(uint16_t, uint8_t*, uint16_t);
typedef int32_t (*HX8347D_ReadReg_Func) (uint16_t, uint8_t*, uint16_t);

typedef struct
{
  HX8347D_Init_Func             Init;
  HX8347D_DeInit_Func           DeInit;
  HX8347D_WriteReg_Func         WriteReg;
  HX8347D_ReadReg_Func          ReadReg;
  HX8347D_GetTick_Func          GetTick;
} HX8347D_IO_t;


typedef struct
{
  HX8347D_IO_t         IO;
  hx8347d_ctx_t        Ctx;
  uint8_t              IsInitialized;
} HX8347D_Object_t;

typedef struct
{
  /* Control functions */
  int32_t (*Init             )(HX8347D_Object_t*, uint32_t, uint32_t);
  int32_t (*DeInit           )(HX8347D_Object_t*);
  int32_t (*ReadID           )(HX8347D_Object_t*, uint32_t*);
  int32_t (*DisplayOn        )(HX8347D_Object_t*);
  int32_t (*DisplayOff       )(HX8347D_Object_t*);
  int32_t (*SetBrightness    )(HX8347D_Object_t*, uint32_t);
  int32_t (*GetBrightness    )(HX8347D_Object_t*, uint32_t*);
  int32_t (*SetOrientation   )(HX8347D_Object_t*, uint32_t);
  int32_t (*GetOrientation   )(HX8347D_Object_t*, uint32_t*);

  /* Drawing functions*/
  int32_t ( *SetCursor       ) (HX8347D_Object_t*, uint32_t, uint32_t);
  int32_t ( *DrawBitmap      ) (HX8347D_Object_t*, uint32_t, uint32_t, uint8_t *);
  int32_t ( *FillRGBRect     ) (HX8347D_Object_t*, uint32_t, uint32_t, uint8_t*, uint32_t, uint32_t);
  int32_t ( *DrawHLine       ) (HX8347D_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *DrawVLine       ) (HX8347D_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *FillRect        ) (HX8347D_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetPixel        ) (HX8347D_Object_t*, uint32_t, uint32_t, uint32_t*);
  int32_t ( *SetPixel        ) (HX8347D_Object_t*, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetXSize        ) (HX8347D_Object_t*, uint32_t *);
  int32_t ( *GetYSize        ) (HX8347D_Object_t*, uint32_t *);
}HX8347D_Drv_t;

/**
  * @}
  */

/** @defgroup HX8347D_Exported_Constants Exported Constants
  * @{
  */

/**
  * @brief  HX8347D return code
  */
#define HX8347D_OK                (0)
#define HX8347D_ERROR             (-1)

/**
  * @brief  HX8347D ID
  */
#define  HX8347D_ID    0x0047

/**
  * @brief  HX8347D Size
  */
#define  HX8347D_WIDTH              320U
#define  HX8347D_HEIGHT             240U

/**
 *  @brief LCD_OrientationTypeDef
 *  Possible values of Display Orientation
 */
#define HX8347D_ORIENTATION_PORTRAIT         0x00U /* Portrait orientation choice of LCD screen               */
#define HX8347D_ORIENTATION_PORTRAIT_ROT180  0x01U /* Portrait rotated 180° orientation choice of LCD screen  */
#define HX8347D_ORIENTATION_LANDSCAPE        0x02U /* Landscape orientation choice of LCD screen              */
#define HX8347D_ORIENTATION_LANDSCAPE_ROT180 0x03U /* Landscape rotated 180° orientation choice of LCD screen */

/**
 *  @brief  Possible values of pixel data format (ie color coding)
 */
#define HX8347D_FORMAT_RBG444                0x03U /* Pixel format chosen is RGB444 : 12 bpp */
#define HX8347D_FORMAT_RBG565                0x05U /* Pixel format chosen is RGB565 : 16 bpp */
#define HX8347D_FORMAT_RBG666                0x06U /* Pixel format chosen is RGB666 : 18 bpp */
#define HX8347D_FORMAT_DEFAULT               HX8347D_FORMAT_RBG565
/**
  * @}
  */

/** @defgroup HX8347D_Exported_Functions Exported Functions
  * @{
  */
int32_t HX8347D_RegisterBusIO(HX8347D_Object_t *pObj, HX8347D_IO_t *pIO);
int32_t HX8347D_Init(HX8347D_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation);
int32_t HX8347D_DeInit(HX8347D_Object_t *pObj);
int32_t HX8347D_ReadID(HX8347D_Object_t *pObj, uint32_t *Id);
int32_t HX8347D_DisplayOn(HX8347D_Object_t *pObj);
int32_t HX8347D_DisplayOff(HX8347D_Object_t *pObj);
int32_t HX8347D_SetBrightness(HX8347D_Object_t *pObj, uint32_t Brightness);
int32_t HX8347D_GetBrightness(HX8347D_Object_t *pObj, uint32_t *Brightness);
int32_t HX8347D_SetOrientation(HX8347D_Object_t *pObj, uint32_t Orientation);
int32_t HX8347D_GetOrientation(HX8347D_Object_t *pObj, uint32_t *Orientation);

int32_t HX8347D_SetCursor(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos);
int32_t HX8347D_DrawBitmap(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
int32_t HX8347D_FillRGBRect(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
int32_t HX8347D_DrawHLine(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t HX8347D_DrawVLine(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t HX8347D_DrawLine(HX8347D_Object_t *pObj, uint32_t X1pos, uint32_t Y1pos, uint32_t X2pos, uint32_t Y2pos, uint32_t Color);
int32_t HX8347D_FillRect(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t HX8347D_SetPixel(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
int32_t HX8347D_GetPixel(HX8347D_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t HX8347D_GetXSize(HX8347D_Object_t *pObj, uint32_t *XSize);
int32_t HX8347D_GetYSize(HX8347D_Object_t *pObj, uint32_t *YSize);

extern HX8347D_Drv_t   HX8347D_Driver;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* HX8347D_H */

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
