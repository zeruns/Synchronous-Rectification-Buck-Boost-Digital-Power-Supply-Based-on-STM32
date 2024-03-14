/*
 ******************************************************************************
 * @file    hx8347d_regs.h
 * @author  MCD Application Team
 * @brief   This file contains all the functions prototypes for the
 *          hx8347d_regs.c driver.
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
#ifndef HX8347D_REGS_H
#define HX8347D_REGS_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup HX8347D
  * @{
  */

/** @defgroup HX8347D_REGS_Exported_Constants HX8347D_REGS Exported Constants
  * @{
  */
/**
  * @brief  HX8347D Registers
  */
#define HX8347D_HIMAX_ID                        0x00U
#define HX8347D_DISPLAY_MODE_CTRL               0x01U
#define HX8347D_COLUMN_ADDRESS_START2           0x02U
#define HX8347D_COLUMN_ADDRESS_START1           0x03U
#define HX8347D_COLUMN_ADDRESS_END2             0x04U
#define HX8347D_COLUMN_ADDRESS_END1             0x05U
#define HX8347D_ROW_ADDRESS_START2              0x06U
#define HX8347D_ROW_ADDRESS_START1              0x07U
#define HX8347D_ROW_ADDRESS_END2                0x08U
#define HX8347D_ROW_ADDRESS_END1                0x09U
#define HX8347D_PARTIAL_AREA_START_ROW2         0x0AU
#define HX8347D_PARTIAL_AREA_START_ROW1         0x0BU
#define HX8347D_PARTIAL_AREA_END_ROW2           0x0CU
#define HX8347D_PARTIAL_AREA_END_ROW1           0x0DU
#define HX8347D_VERTICAL_SCROLL_TOP_AREA2       0x0EU
#define HX8347D_VERTICAL_SCROLL_TOP_AREA1       0x0FU
#define HX8347D_VERTICAL_SCROLL_HEIGHT_AREA2    0x10U
#define HX8347D_VERTICAL_SCROLL_HEIGHT_AREA1    0x11U
#define HX8347D_VERTICAL_SCROLL_BUTTON_AREA2    0x12U
#define HX8347D_VERTICAL_SCROLL_BUTTON_AREA1    0x13U
#define HX8347D_VERTICAL_SCROLL_ADDRESS_START2  0x14U
#define HX8347D_VERTICAL_SCROLL_ADDRESS_START1  0x15U
#define HX8347D_MEMORY_ACCESS_CTRL              0x16U
#define HX8347D_COLMOD_CTRL                     0x17U
#define HX8347D_OSC_CTRL1                       0x18U
#define HX8347D_OSC_CTRL2                       0x19U
#define HX8347D_POWER_CTRL1                     0x1AU
#define HX8347D_POWER_CTRL2                     0x1BU
#define HX8347D_POWER_CTRL3                     0x1CU
#define HX8347D_POWER_CTRL4                     0x1DU
#define HX8347D_POWER_CTRL5                     0x1EU
#define HX8347D_POWER_CTRL6                     0x1FU
#define HX8347D_READ_DATA                       0x22U
#define HX8347D_VCOM_CTRL1                      0x23U
#define HX8347D_VCOM_CTRL2                      0x24U
#define HX8347D_VCOM_CTRL3                      0x25U
#define HX8347D_DISPLAY_CTRL1                   0x26U
#define HX8347D_DISPLAY_CTRL2                   0x27U
#define HX8347D_DISPLAY_CTRL3                   0x28U
#define HX8347D_FRAME_CTRL1                     0x29U
#define HX8347D_FRAME_CTRL2                     0x2AU
#define HX8347D_FRAME_CTRL3                     0x2BU
#define HX8347D_FRAME_CTRL4                     0x2CU
#define HX8347D_CYCLE_CTRL1                     0x2DU
#define HX8347D_CYCLE_CTRL2                     0x2EU
#define HX8347D_DISPLAY_INVERSION               0x2FU
#define HX8347D_RGB_CTRL1                       0x31U
#define HX8347D_RGB_CTRL2                       0x32U
#define HX8347D_RGB_CTRL3                       0x33U
#define HX8347D_RGB_CTRL4                       0x34U
#define HX8347D_PANEL_CTRL                      0x36U
#define HX8347D_OTP1                            0x38U
#define HX8347D_OTP2                            0x39U
#define HX8347D_OTP3                            0x3AU
#define HX8347D_CABC_CTRL1                      0x3CU
#define HX8347D_CABC_CTRL2                      0x3DU
#define HX8347D_CABC_CTRL3                      0x3EU
#define HX8347D_CABC_CTRL4                      0x3FU
#define HX8347D_GAMMA_CTRL1                     0x40U
#define HX8347D_GAMMA_CTRL2                     0x41U
#define HX8347D_GAMMA_CTRL3                     0x42U
#define HX8347D_GAMMA_CTRL4                     0x43U
#define HX8347D_GAMMA_CTRL5                     0x44U
#define HX8347D_GAMMA_CTRL6                     0x45U
#define HX8347D_GAMMA_CTRL7                     0x46U
#define HX8347D_GAMMA_CTRL8                     0x47U
#define HX8347D_GAMMA_CTRL9                     0x48U
#define HX8347D_GAMMA_CTRL10                    0x49U
#define HX8347D_GAMMA_CTRL11                    0x4AU
#define HX8347D_GAMMA_CTRL12                    0x4BU
#define HX8347D_GAMMA_CTRL13                    0x4CU
#define HX8347D_GAMMA_CTRL14                    0x50U
#define HX8347D_GAMMA_CTRL15                    0x51U
#define HX8347D_GAMMA_CTRL16                    0x52U
#define HX8347D_GAMMA_CTRL17                    0x53U
#define HX8347D_GAMMA_CTRL18                    0x54U
#define HX8347D_GAMMA_CTRL19                    0x55U
#define HX8347D_GAMMA_CTRL20                    0x56U
#define HX8347D_GAMMA_CTRL21                    0x57U
#define HX8347D_GAMMA_CTRL22                    0x58U
#define HX8347D_GAMMA_CTRL23                    0x59U
#define HX8347D_GAMMA_CTRL24                    0x5AU
#define HX8347D_GAMMA_CTRL25                    0x5BU
#define HX8347D_GAMMA_CTRL26                    0x5CU
#define HX8347D_GAMMA_CTRL27                    0x5DU
#define HX8347D_MODE_CTRL                       0x60U
#define HX8347D_ID1                             0x61U
#define HX8347D_ID2                             0x62U
#define HX8347D_ID3                             0x63U
#define HX8347D_POWER_SAVING_INTERNAL_CTRL1     0x68U
#define HX8347D_POWER_SAVING_INTERNAL_CTRL2     0x69U
#define HX8347D_POWER_SAVING_INTERNAL_CTRL3     0x70U
#define HX8347D_POWER_SAVING_INTERNAL_CTRL4     0x71U
#define HX8347D_CABC_CTRL5                      0xC3U
#define HX8347D_CABC_CTRL6                      0xC5U
#define HX8347D_CABC_CTRL7                      0xC7U
#define HX8347D_GAIN_SELECT0                    0xCBU
#define HX8347D_GAIN_SELECT1                    0xCDU
#define HX8347D_GAIN_SELECT2                    0xCCU
#define HX8347D_GAIN_SELECT3                    0xCEU
#define HX8347D_GAIN_SELECT4                    0xCFU
#define HX8347D_GAIN_SELECT5                    0xD0U
#define HX8347D_GAIN_SELECT6                    0xD1U
#define HX8347D_GAIN_SELECT7                    0xD2U
#define HX8347D_GAIN_SELECT8                    0xD3U
#define HX8347D_SOURCE_OP_CTRL1                 0xE8U
#define HX8347D_SOURCE_OP_CTRL2                 0xE9U
#define HX8347D_POWER_CTRL_INTERNAL_USED1       0xEAU
#define HX8347D_POWER_CTRL_INTERNAL_USED2       0xEBU
#define HX8347D_SOURCE_CTRL_INTERNAl_USED1      0xECU
#define HX8347D_SOURCE_CTRL_INTERNAl_USED2      0xEDU

/**
  * @}
  */

/** @defgroup HX8347D_REGS_Exported_Types HX8347D_REGS Exported Types
  * @{
  */
typedef int32_t (*HX8347D_Write_Func)    (void *, uint16_t, uint8_t*, uint16_t);    /*  this will connect to the LCD_IO_WriteData */
typedef int32_t (*HX8347D_Read_Func)     (void *, uint16_t, uint8_t*, uint16_t);   /*  this will connect to the LCD_IO_ReadData */

typedef struct
{
  HX8347D_Write_Func     WriteReg;
  HX8347D_Read_Func      ReadReg;
  void                   *handle;
} hx8347d_ctx_t;
/**
  * @}
  */

/** @defgroup HX8347D_REGS_Exported_Functions HX8347D_REGS Exported Functions
  * @{
  */
int32_t hx8347d_write_reg(hx8347d_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);
int32_t hx8347d_read_reg(hx8347d_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);

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

#endif /* HX8347D_REGS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
