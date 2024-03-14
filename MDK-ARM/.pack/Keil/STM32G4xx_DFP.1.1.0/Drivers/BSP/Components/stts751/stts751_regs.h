/*
 ******************************************************************************
 * @file    stts751_regs.h
 * @author  MCD Application Team
 * @brief   This file contains all the functions prototypes for the
 *          stts751_regs.c driver.
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
#ifndef STTS751_REGS_H
#define STTS751_REGS_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup stts751
 * @{
 */

#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

/** @defgroup ST_MEMS_common_types
  * @{
  */

typedef union{
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

typedef union{
	int16_t i16bit;
	uint8_t u8bit[2];
} axis1bit16_t;

typedef union{
	int32_t i32bit[3];
	uint8_t u8bit[12];
} axis3bit32_t;

typedef union{
	int32_t i32bit;
	uint8_t u8bit[4];
} axis1bit32_t;

typedef struct {
   uint8_t bit0       : 1;
   uint8_t bit1       : 1;
   uint8_t bit2       : 1;
   uint8_t bit3       : 1;
   uint8_t bit4       : 1;
   uint8_t bit5       : 1;
   uint8_t bit6       : 1;
   uint8_t bit7       : 1;
} bitwise_t;

#define PROPERTY_DISABLE                (0)
#define PROPERTY_ENABLE                 (1)

#endif /* MEMS_SHARED_TYPES*/

/**
  * @}
  */

/** @defgroup stts751_interface
  * @{
  */

typedef int32_t (*stts751_write_ptr)(void *, uint8_t, uint8_t*, uint16_t);
typedef int32_t (*stts751_read_ptr) (void *, uint8_t, uint8_t*, uint16_t);

typedef struct {
  /** Component mandatory fields **/
  stts751_write_ptr  write_reg;
  stts751_read_ptr   read_reg;
  /** Customizable optional pointer **/
  void *handle;
} stts751_ctx_t;

/**
  * @}
  */


/** @defgroup stts751_Infos
  * @{
  */
#define USE_ENV_SENSOR_CONV_MODE            STTS751_CONTINUOUS_MODE
#define USE_ENV_SENSOR_CONV_RESOLUTION      STTS751_CONV_12BITS
#define USE_ENV_SENSOR_CONV_RATE            STTS751_ONE_PER_SECOND
#define USE_ENV_SENSOR_ALERT                STTS751_ALERT_ENABLE
#define USE_ENV_SENSOR_LIMIT_HIGH           85.00
#define USE_ENV_SENSOR_LIMIT_LOW            00.00
#define USE_ENV_SENSOR_THERM_LIMIT          85
#define USE_ENV_SENSOR_THERM_HYSTERESIS     10

/** I2C Device Address 8 bit format: if SA0=0 -> 0xB9 if SA0=1 -> 0xBB **/
#define STTS751_I2C_ADD_H   0xBB
#define STTS751_I2C_ADD_L   0xB9

#define STTS751_OK                0
#define STTS751_ERROR            -1

#define STTS751_I2C_BUS          0U
#define STTS751_TIMEOUT          100U

#define STTS751_TEMP             1U
/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
/***************************** Read Access Only *******************************/
#define STTS751_TEMP_VALUE_HIGH_BYTE            0x00U  /*!< Temperature value high byte Register */
#define STTS751_TEMP_VALUE_LOW_BYTE             0x02U  /*!< Temperature value low byte Register */

#define STTS751_PRODUCT_ID                      0xFDU  /*!< Product Identifier Register */
#define STTS751_MANUFACTURER_ID                 0xFEU  /*!< Manufacturer Identifier Register */
#define STTS751_REVISION_ID                     0xFFU  /*!< Revision Number Identifier Register */

   /***************************** Write Access Only ******************************/
#define STTS751_ONE_SHOT                        0x0FU  /*!< Single Conversion Request Register */


/***************************** Read/Write Access ******************************/
#define STTS751_STATUS                          0x01U  /*!< Configuration Register */
#define STTS751_CONFIGURATION                   0x03U  /*!< Configuration Register */
#define STTS751_CONVERSION_RATE                 0x04U  /*!< Conversion Rate Register */
#define STTS751_TEMP_HIGH_LIMIT_HIGH_BYTE       0x05U  /*!< High Temperature Limit high byte Register */
#define STTS751_TEMP_HIGH_LIMIT_LOW_BYTE        0x06U  /*!< High Temperature Limit low byte Register */
#define STTS751_TEMP_LOW_LIMIT_HIGH_BYTE        0x07U  /*!< Low Temperature Limit high byte Register */
#define STTS751_TEMP_LOW_LIMIT_LOW_BYTE         0x08U  /*!< Low Temperature Limit low byte Register */


#define STTS751_THERM_LIMIT                     0x20U  /*!< THERM Limit Register */
#define STTS751_THERM_HYSTERISIS                0x21U  /*!< THERM Hysteris Register */
#define STTS751_SMBUS_TIMEOUT                   0x22U  /*!< SMBUS Timeout Enabling Register */

/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/
/** @defgroup EVENT_Pin_selection STTS751 EVENT Pin selection
  * @{
  */
#define STTS751_ALERT_ENABLE                    0x00U
#define STTS751_ALERT_DISABLE                   0x80U
/**
  * @}
  */

/** @defgroup Conversion_Mode_Selection
  * @{
  */
#define STTS751_CONTINUOUS_MODE                 0x00U
#define STTS751_ONE_SHOT_MODE                   0x40U
/**
  * @}
  */

/** @defgroup Conversion_Bits_Resolution_Selection STTS751 Conversion Bits Resolution Selection
  * @{
  */
#define STTS751_CONV_9BITS                      0x08U
#define STTS751_CONV_10BITS                     0x00U
#define STTS751_CONV_11BITS                     0x04U
#define STTS751_CONV_12BITS                     0x0CU
/**
  * @}
  */

/** @defgroup Conversion_Rate_Per_Second_Selection STTS751 Conversion Rate Per Second Selection
  * @{
  */
#define STTS751_ONE_SIXTEENTH_PER_SECOND        0x00U /* 0.0625 conversion/Second     */
#define STTS751_ONE_EIGHTH_PER_SECOND           0x01U /* 0.125 conversion/Second      */
#define STTS751_ONE_QUATER_PER_SECOND           0x02U /* 0.25 conversion/Second       */
#define STTS751_HALF_PER_SECOND                 0x03U /* 0.5 conversion/Second        */
#define STTS751_ONE_PER_SECOND                  0x04U /* 1 conversion/Second          */
#define STTS751_TWO_PER_SECOND                  0x05U /* 2 conversions/Second         */
#define STTS751_FOUR_PER_SECOND                 0x05U /* 4 conversions/Second         */
#define STTS751_HEIGH_PER_SECOND                0x07U /* 8 conversions/Second         */
#define STTS751_SIXTEEN_PER_SECOND              0x08U /* 16 conversions/Second        */
#define STTS751_THIRTY_TWO_PER_SECOND           0x09U /* 32 conversions/Second        */
/**
  * @}
  */

/** @defgroup Device_Identification STTS751 Device Identification
  * @{
  */

#define STTS751_MANUFACTURER            0x53U

/**
  * @}
  */

/** @defgroup Status STTS751 Status
  * @{
  */
#define STTS751_STATUS_READY           0x80U
#define STTS751_STATUS_REACHED         0x01U
#define STTS751_STATUS_NOT_REACHED     0x00U
/**
  * @}
  */
static const uint8_t SHIFT_CONV_RESOLUTION[] =
{
  4U,   /* 0:  STTS751_CONV_10BITS (default)  */
  8U,   /* 1:  STTS751_CONV_11BITS  */
  2U,   /* 2:  STTS751_CONV_9BITS   */
  16U,  /* 3:  STTS751_CONV_12BITS  */
};

static const float SHIFT_CONV_RATE[] =
{
   0.0625, /* 0: 0.0625 conversion/Second  */
   0.125,  /* 1: 0.125  conversion/Second  */
   0.25,   /* 2: 0.25   conversion/Second  */
   0.5,    /* 3: 0.5    conversion/Second  */
   1,      /* 4: 1      conversion/Second  */
   2,      /* 5: 2      conversions/Second */
   4,      /* 6: 4      conversions/Second */
   8,      /* 7: 8      conversions/Second */
   16,     /* 8: 16     conversions/Second */
   32,     /* 9: 32     conversions/Second */
};

typedef struct{
  uint8_t      Temperature_msb;
  uint8_t      Status;
  uint8_t      Temperature_lsb;
  uint8_t      Config;
  uint8_t      Conversion_rate;
  uint8_t      Temp_high_limit_msb;
  uint8_t      Temp_high_limit_lsb;
  uint8_t      Temp_low_limit_msb;
  uint8_t      Temp_low_limit_lsb;
  uint8_t      OneShot;
  uint8_t      Therm_limit;
  uint8_t      Therm_hysteresis;
} stts751_reg_t;

/** @addtogroup STTS751_Exported_Functions
 * @{
 */
int32_t stts751_read_reg(stts751_ctx_t *ctx, uint8_t reg, uint8_t* data,
                         uint16_t len);
int32_t stts751_write_reg(stts751_ctx_t *ctx, uint8_t reg, uint8_t* data,
                          uint16_t len);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*STTS751_REGS_H */
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
