/**
  ******************************************************************************
  * @file    stm32g474e_eval_audio.c
  * @author  MCD Application Team
  * @brief   This file provides the Audio driver for the STM32G474E-EVAL
  *          evaluation board.
  @verbatim
  How To use this driver:
  -----------------------
   + This driver supports STM32G4xx devices on STM32G474E-EVAL (MB1397) Evaluation boards.
   + Call the function BSP_AUDIO_OUT_Init() for AUDIO OUT initialization:
        Instance : Select the output instance. Can only be 0 (SAI)
        AudioInit: Audio Out structure to select the following parameters
                   - Device: Select the output device (headphone, speaker, ..)
                   - SampleRate: Select the output sample rate (8Khz .. 96Khz)
                   - BitsPerSample: Select the output resolution (16 or 32bits per sample)
                   - ChannelsNbr: Select the output channels number(1 for mono, 2 for stereo)
                   - Volume: Select the output volume(0% .. 100%)

      This function configures all the hardware required for the audio application (codec, I2C, SAI,
      GPIOs, DMA and interrupt if needed). This function returns BSP_ERROR_NONE if configuration is OK.
      If the returned value is different from BSP_ERROR_NONE or the function is stuck then the communication with
      the codec or the MFX has failed (try to un-plug the power or reset device in this case).

      User can update the SAI or the clock configurations by overriding the weak MX functions MX_SAI1_Init()
      and MX_SAI1_ClockConfig()
      User can override the default MSP configuration and register his own MSP callbacks (defined at application level)
      by calling BSP_AUDIO_OUT_RegisterMspCallbacks() function
      User can restore the default MSP configuration by calling BSP_AUDIO_OUT_RegisterDefaultMspCallbacks()
      To use these two functions, user have to enable USE_HAL_SAI_REGISTER_CALLBACKS within stm32g4xx_hal_conf.h file

   + Call the function BSP_AUDIO_OUT_Play() to play audio stream:
        Instance : Select the output instance. Can only be 0 (SAI)
        pBuf: pointer to the audio data file address
        NbrOfBytes: Total size of the buffer to be sent in Bytes

   + Call the function BSP_AUDIO_OUT_Pause() to pause playing
   + Call the function BSP_AUDIO_OUT_Resume() to resume playing.
       Note. After calling BSP_AUDIO_OUT_Pause() function for pause, only BSP_AUDIO_OUT_Resume() should be called
          for resume (it is not allowed to call BSP_AUDIO_OUT_Play() in this case).
       Note. This function should be called only when the audio file is played or paused (not stopped).
   + Call the function BSP_AUDIO_OUT_Stop() to stop playing.
   + Call the function BSP_AUDIO_OUT_Mute() to mute the player.
   + Call the function BSP_AUDIO_OUT_UnMute() to unmute the player.
   + Call the function BSP_AUDIO_OUT_IsMute() to get the mute state(BSP_AUDIO_MUTE_ENABLED or BSP_AUDIO_MUTE_DISABLED).
   + Call the function BSP_AUDIO_OUT_SetDevice() to update the AUDIO OUT device.
   + Call the function BSP_AUDIO_OUT_GetDevice() to get the AUDIO OUT device.
   + Call the function BSP_AUDIO_OUT_SetSampleRate() to update the AUDIO OUT sample rate.
   + Call the function BSP_AUDIO_OUT_GetSampleRate() to get the AUDIO OUT sample rate.
   + Call the function BSP_AUDIO_OUT_SetBitsPerSample() to update the AUDIO OUT resolution.
   + Call the function BSP_AUDIO_OUT_GetBitPerSample() to get the AUDIO OUT resolution.
   + Call the function BSP_AUDIO_OUT_SetChannelsNbr() to update the AUDIO OUT number of channels.
   + Call the function BSP_AUDIO_OUT_GetChannelsNbr() to get the AUDIO OUT number of channels.
   + Call the function BSP_AUDIO_OUT_SetVolume() to update the AUDIO OUT volume.
   + Call the function BSP_AUDIO_OUT_GetVolume() to get the AUDIO OUT volume.
   + Call the function BSP_AUDIO_OUT_GetState() to get the AUDIO OUT state.

   + BSP_AUDIO_OUT_SetDevice(), BSP_AUDIO_OUT_SetSampleRate(), BSP_AUDIO_OUT_SetBitsPerSample() and
     BSP_AUDIO_OUT_SetChannelsNbr() cannot be called while the state is AUDIO_OUT_STATE_PLAYING.
   + For each mode, you may need to implement the relative callback functions into your code.
      The Callback functions are named AUDIO_OUT_XXX_CallBack() and only their prototypes are declared in
      the stm32g474e_eval_audio.h file. (refer to the example for more details on the callbacks implementations)

   + Call the function BSP_AUDIO_IN_Init() for AUDIO IN initialization:
        Instance : Select the input instance. Can be 0 (SAI)
        AudioInit: Audio In structure to select the following parameters
                   - Device: Select the input device (analog mic)
                   - SampleRate: Select the input sample rate (8Khz .. 96Khz)
                   - BitsPerSample: Select the input resolution (16 or 32bits per sample)
                   - ChannelsNbr: Select the input channels number(1 for mono, 2 for stereo)
                   - Volume: Select the input volume(0% .. 100%)

      This function configures all the hardware required for the audio application (codec, I2C, SAI,
      GPIOs, DMA and interrupt if needed). This function returns BSP_ERROR_NONE if configuration is OK.
      If the returned value is different from BSP_ERROR_NONE or the function is stuck then the communication with
      the codec or the MFX has failed (try to un-plug the power or reset device in this case).

      User can update the clock configurations by overriding the weak MX functions MX_SAI1_Init(),
      User can override the default MSP configuration and register his own MSP callbacks (defined at application level)
      by calling BSP_AUDIO_IN_RegisterMspCallbacks() function
      User can restore the default MSP configuration by calling BSP_AUDIO_IN_RegisterDefaultMspCallbacks()
      To use these two functions, user have to enable USE_HAL_SAI_REGISTER_CALLBACKS
      within stm32g4xx_hal_conf.h file

   + Call the function BSP_EVAL_AUDIO_IN_Record() to record audio stream. The recorded data are stored to user buffer in raw
        (L, R, L, R ...)
        Instance : Select the input instance. Can be 0 (SAI)
        pBuf: pointer to user buffer
        NbrOfBytes: Total size of the buffer to be sent in Bytes

   + Call the function BSP_AUDIO_IN_Pause() to pause recording
   + Call the function BSP_AUDIO_IN_Resume() to resume recording.
   + Call the function BSP_AUDIO_IN_Stop() to stop recording.
   + Call the function BSP_AUDIO_IN_SetDevice() to update the AUDIO IN device.
   + Call the function BSP_AUDIO_IN_GetDevice() to get the AUDIO IN device.
   + Call the function BSP_AUDIO_IN_SetSampleRate() to update the AUDIO IN sample rate.
   + Call the function BSP_AUDIO_IN_GetSampleRate() to get the AUDIO IN sample rate.
   + Call the function BSP_AUDIO_IN_SetBitPerSample() to update the AUDIO IN resolution.
   + Call the function BSP_AUDIO_IN_GetBitPerSample() to get the AUDIO IN resolution.
   + Call the function BSP_AUDIO_IN_SetChannelsNbr() to update the AUDIO IN number of channels.
   + Call the function BSP_AUDIO_IN_GetChannelsNbr() to get the AUDIO IN number of channels.
   + Call the function BSP_AUDIO_IN_SetVolume() to update the AUDIO IN volume.
   + Call the function BSP_AUDIO_IN_GetVolume() to get the AUDIO IN volume.
   + Call the function BSP_AUDIO_IN_GetState() to get the AUDIO IN state.

   + For each mode, you may need to implement the relative callback functions into your code.
      The Callback functions are named AUDIO_IN_XXX_CallBack() and only their prototypes are declared in
      the stm32g474e_eval_audio.h file. (refer to the example for more details on the callbacks implementations)

   + The driver API and the callback functions are at the end of the stm32g474e_eval_audio.h file.
  Known Limitations:
  ------------------
   1- If the TDM Format used to play in parallel 2 audio Streams (the first Stream is configured in codec SLOT0 and second
      Stream in SLOT1) the Pause/Resume, volume and mute feature will control both streams.
   2- Parsing of audio file is not implemented (in order to determine audio file properties: Mono/Stereo, Data size,
      File size, Audio Frequency, Audio Data header size ...). The configuration is fixed for the given audio file.

  @endverbatim
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
#include "stm32g474e_eval_audio.h"
#include "stm32g474e_eval_bus.h"
#include "stm32g474e_eval_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL 
  * @{
  */

/** @defgroup STM32G474E-EVAL_AUDIO STM32G474E-EVAL AUDIO
  * @brief This file includes the low layer driver for wm8994 Audio Codec
  *        available on STM32G474E-EVAL evaluation board(MB1397).
  * @{
  */


/** @defgroup STM32G474E-EVAL_AUDIO_Exported_Variables Exported Variables
  * @{
  */
/* Audio in and out component object */
void *AudioCompObj = NULL;
/* Play handle */
SAI_HandleTypeDef                      haudio_out_sai;

/* Play context */
AUDIO_OUT_Ctx_t AudioOutCtx[AUDIO_OUT_INSTANCES_NBR] = {0};

/* Recording handles */
SAI_HandleTypeDef                      haudio_in_sai;

/* Recording context */
AUDIO_IN_Ctx_t  AudioInCtx[AUDIO_IN_INSTANCES_NBR] = {0};
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_AUDIO_Private_Variables Private Variables
  * @{
  */
/* saved pll clock config */
static RCC_OscInitTypeDef RCC_OscInitStructSaved;
static uint32_t RCC_InitSysClkSaved;
static RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStructSaved;
/* Audio in and out driver */
static AUDIO_Drv_t *AudioDrv = NULL;
/**
  * @}
  */

/** STM32G474E-EVAL_AUDIO_Private_FunctionPrototypes Private Functions
  */
#if (USE_AUDIO_CODEC_WM8994 == 1)
static int32_t WM8994_Probe(void);
#endif

/* SAI Msp config */
static void  SAI_MspInit(SAI_HandleTypeDef *hsai);
static void  SAI_MspDeInit(SAI_HandleTypeDef *hsai);

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
static void SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
static void SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
static void SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
static void SAI_ErrorCallback(SAI_HandleTypeDef *hsai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
/**
  */

/** @addtogroup STM32G474E-EVAL_AUDIO_OUT_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the audio out peripherals.
  * @param  Instance Audio out instance.
  * @param  AudioInit Audio out init structure.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit)
{

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Fill AudioOutCtx structure */
    AudioOutCtx[Instance].Device         = AudioInit->Device;
    AudioOutCtx[Instance].SampleRate     = AudioInit->SampleRate;
    AudioOutCtx[Instance].BitsPerSample  = AudioInit->BitsPerSample;
    AudioOutCtx[Instance].ChannelsNbr    = AudioInit->ChannelsNbr;
    AudioOutCtx[Instance].Volume         = AudioInit->Volume;
    AudioOutCtx[Instance].State          = AUDIO_OUT_STATE_RESET;

    if(AudioInit->Device <= AUDIO_OUT_DEVICE_AUTO)
    {
#if (USE_AUDIO_CODEC_WM8994 == 1)
      if(WM8994_Probe() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
#endif
    }

    /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
    if(MX_SAI1_ClockConfig(&haudio_out_sai, AudioInit->SampleRate) != HAL_OK)
    {
      return BSP_ERROR_CLOCK_FAILURE;
    }

    /* SAI data transfer preparation:
    Prepare the Media to be used for the audio transfer from memory to SAI peripheral */
    haudio_out_sai.Instance = AUDIO_OUT_SAIx;

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register the SAI MSP Callbacks */
    if(AudioOutCtx[Instance].IsMspCallbacksValid == 0U)
    {
      if(BSP_AUDIO_OUT_RegisterDefaultMspCallbacks(0) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
    }
#else
    SAI_MspInit(&haudio_out_sai);
#endif /* #if (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    MX_SAI_InitTypeDef mx_out_config;

    /* Prepare haudio_out_sai handle */
    mx_out_config.AudioFrequency    = AudioInit->SampleRate;
    mx_out_config.AudioMode         = SAI_MODEMASTER_TX;
    mx_out_config.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
    mx_out_config.MonoStereoMode    = (AudioInit->ChannelsNbr == 1U) ? SAI_MONOMODE : SAI_STEREOMODE;
    if (AudioInit->BitsPerSample == AUDIO_RESOLUTION_24B)
    {
      mx_out_config.DataSize          = SAI_DATASIZE_24;
      mx_out_config.FrameLength       = 64;
      mx_out_config.ActiveFrameLength = 32;
    }
    else
    {
      mx_out_config.DataSize          = SAI_DATASIZE_16;
      mx_out_config.FrameLength       = 32;
      mx_out_config.ActiveFrameLength = 16;
    }

    mx_out_config.OutputDrive       = SAI_OUTPUTDRIVE_ENABLE;
    mx_out_config.Synchro           = SAI_ASYNCHRONOUS;
    mx_out_config.SynchroExt        = SAI_SYNCEXT_DISABLE;

    switch(AudioInit->Device)
    {
    case AUDIO_OUT_DEVICE_BOTH:
    case AUDIO_OUT_DEVICE_AUTO:
      mx_out_config.SlotActive         = CODEC_AUDIOFRAME_SLOT_0123;
      break;
    case AUDIO_OUT_DEVICE_SPEAKER:
      mx_out_config.SlotActive         = CODEC_AUDIOFRAME_SLOT_13;
      break;
    case AUDIO_OUT_DEVICE_HEADPHONE:
    default:
      mx_out_config.SlotActive         = CODEC_AUDIOFRAME_SLOT_01;
      break;
    }

    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&haudio_out_sai);

    /* SAI peripheral initialization: this __weak function can be redefined by the application  */
    if(MX_SAI1_Init(&haudio_out_sai, &mx_out_config) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
    /* Enable SAI peripheral to generate MCLK */
    __HAL_SAI_ENABLE(&haudio_out_sai);

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
    if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
    if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }

#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
#if (USE_AUDIO_CODEC_WM8994 == 1)
    if(AudioInit->Device <= AUDIO_OUT_DEVICE_AUTO)
    {
      WM8994_Init_t codec_init;

      switch (AudioInit->BitsPerSample)
      {
      case AUDIO_RESOLUTION_24B:
        codec_init.Resolution   = WM8994_RESOLUTION_24b;
        break;
      case AUDIO_RESOLUTION_32B:
        codec_init.Resolution   = WM8994_RESOLUTION_32b;
        break;
      case AUDIO_RESOLUTION_16B:
      default:
        codec_init.Resolution   =  WM8994_RESOLUTION_16b;
        break;
      }
      /* Fill codec_init structure */
      codec_init.Frequency    = AudioInit->SampleRate;
      codec_init.InputDevice  = (AudioInCtx[0].State == AUDIO_IN_STATE_RESET) ? WM8994_IN_NONE : WM8994_IN_LINE1;
      codec_init.OutputDevice = AudioInit->Device;
      codec_init.Volume       = (AudioInit->Volume);

      /* Initialize the codec internal registers */
      if(AudioDrv->Init(AudioCompObj, &codec_init) < 0)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
#endif
    /* Update BSP AUDIO OUT state */
    AudioOutCtx[Instance].State = AUDIO_OUT_STATE_STOP;

    return BSP_ERROR_NONE;
  }
}

/**
  * @brief  De-initialize the audio out peripherals.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
    SAI_MspDeInit(&haudio_out_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */

    /* Initialize the haudio_out_sai Instance parameter */
    haudio_out_sai.Instance = AUDIO_OUT_SAIx;
    /* Call the Media layer stop function */
    if(AudioDrv->DeInit(AudioCompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if(HAL_SAI_DeInit(&haudio_out_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Update BSP AUDIO OUT state */
      AudioOutCtx[Instance].State = AUDIO_OUT_STATE_RESET;
    }
    /* restore clock */
    (void)MX_SAI1_ClockDeConfig(&haudio_out_sai);
  }

  /* Return BSP status */
  return ret;
}

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP AUDIO OUT Msp Callbacks
  * @param Instance AUDIO OUT Instance. It can only be 0
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_SAI_RESET_HANDLE_STATE(&haudio_out_sai);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPINIT_CB_ID, SAI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPDEINIT_CB_ID, SAI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      AudioOutCtx[Instance].IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP AUDIO OUT Msp Callback registering
  * @param Instance     AUDIO OUT Instance. It can only be 0
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_RegisterMspCallbacks (uint32_t Instance, BSP_AUDIO_OUT_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_SAI_RESET_HANDLE_STATE(&haudio_out_sai);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      AudioOutCtx[Instance].IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Start playing audio stream from a data buffer for a determined size.
  * @param  Instance Audio out instance.
  * @param  pData Pointer on data buffer.
  * @param  NbrOfBytes Size of buffer in bytes. Maximum size is 65535 bytes.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Play(uint32_t Instance, uint8_t* pData, uint32_t NbrOfBytes)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= AUDIO_OUT_INSTANCES_NBR) || (NbrOfBytes > 0xFFFFU))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if((AudioOutCtx[Instance].State == AUDIO_OUT_STATE_STOP) || (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET))
  {
    if(HAL_SAI_Transmit_DMA(&haudio_out_sai, pData, (uint16_t)(NbrOfBytes /(AudioOutCtx[Instance].BitsPerSample/8U))) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(AudioDrv->Play(AudioCompObj) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Update BSP AUDIO OUT state */
      AudioOutCtx[Instance].State = AUDIO_OUT_STATE_PLAYING;
    }
  }
  else
  {
    ret = BSP_ERROR_BUSY;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Pause playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Pause(uint32_t Instance)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio out state */
  else if(AudioOutCtx[Instance].State != AUDIO_OUT_STATE_PLAYING)
  {
    ret = BSP_ERROR_BUSY;
  }/* Call the Audio Codec Pause/Resume function */
  else if(AudioDrv->Pause(AudioCompObj) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Call the Media layer pause function */
  else if(HAL_SAI_DMAPause(&haudio_out_sai) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Update BSP AUDIO OUT state */
    AudioOutCtx[Instance].State = AUDIO_OUT_STATE_PAUSE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Resume playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Resume(uint32_t Instance)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio out state */
  else if(AudioOutCtx[Instance].State != AUDIO_OUT_STATE_PAUSE)
  {
    ret = BSP_ERROR_BUSY;
  }/* Call the Media layer pause/resume function */
  else if(HAL_SAI_DMAResume(&haudio_out_sai) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(AudioDrv->Resume(AudioCompObj) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update BSP AUDIO OUT state */
    AudioOutCtx[Instance].State = AUDIO_OUT_STATE_PLAYING;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Stop playback of audio stream.
  * @param  Instance Audio out instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_OUT_Stop(uint32_t Instance)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_STOP)
  {
    /* Nothing to do */
  }
  else if ((AudioOutCtx[Instance].State != AUDIO_OUT_STATE_PLAYING) &&
           (AudioOutCtx[Instance].State != AUDIO_OUT_STATE_PAUSE))
  {
    ret = BSP_ERROR_BUSY;
  }/* Call the Media layer stop function */
  else if(AudioDrv->Stop(AudioCompObj, CODEC_PDWN_SW) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(HAL_SAI_DMAStop(&haudio_out_sai) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    /* Update BSP AUDIO OUT state */
    AudioOutCtx[Instance].State = AUDIO_OUT_STATE_STOP;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Controls the current audio volume level.
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @param  Volume    Volume level to be set in percentage from 0% to 100% (0 for
  *         Mute and 100 for Max volume level).
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_SetVolume(uint32_t Instance, uint32_t Volume)
{
int32_t ret = BSP_ERROR_NONE;

  if ((Instance >= AUDIO_OUT_INSTANCES_NBR) || (Volume > 100U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Call the codec volume control function with converted volume value */
  else if(AudioDrv->SetVolume(AudioCompObj, AUDIO_VOLUME_OUTPUT, Volume) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update audio out context */
    if(Volume == 0U)
    {
      AudioOutCtx[Instance].IsMute = BSP_AUDIO_MUTE_ENABLED;
    }
    else
    {
      AudioOutCtx[Instance].IsMute = BSP_AUDIO_MUTE_DISABLED;
    }

    AudioOutCtx[Instance].Volume = Volume;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the current audio volume level.
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @param  Volume    pointer to volume to be returned
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetVolume(uint32_t Instance, uint32_t *Volume)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    *Volume = AudioOutCtx[Instance].Volume;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Enables the MUTE
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_Mute(uint32_t Instance)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  /* Check audio out mute status */
  else if (AudioOutCtx[Instance].IsMute == 1U)
  {
    /* Nothing to do */
  }/* Call the Codec Mute function */
  else if(AudioDrv->SetMute(AudioCompObj, CODEC_MUTE_ON) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update Mute State */
    AudioOutCtx[Instance].IsMute = BSP_AUDIO_MUTE_ENABLED;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Disables the MUTE mode
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_UnMute(uint32_t Instance)
{
int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  /* Check audio out mute status */
  else if (AudioOutCtx[Instance].IsMute == 0U)
  {
    /* Nothing to do */
  }
  /* Call the audio codec mute function */
  else if (AudioDrv->SetMute(AudioCompObj, CODEC_MUTE_OFF) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update audio out mute status */
    AudioOutCtx[Instance].IsMute = 0U;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Check whether the MUTE mode is enabled or not
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @param  IsMute    pointer to mute state
  * @retval Mute status
  */
int32_t BSP_AUDIO_OUT_IsMute(uint32_t Instance, uint32_t *IsMute)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    *IsMute = AudioOutCtx[Instance].IsMute;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Switch dynamically (while audio file is played) the output target
  *         (speaker or headphone).
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @param  Device    The audio output device
  * @note   This API can be only called if the AUDIO OUT state is RESET or
  *         STOP to guarantee a correct SAI behavior
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_SetDevice(uint32_t Instance, uint32_t Device)
{
int32_t ret;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if((AudioOutCtx[Instance].State != AUDIO_OUT_STATE_RESET) && (AudioOutCtx[Instance].State != AUDIO_OUT_STATE_STOP))
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    switch(Device)
    {
    case AUDIO_OUT_DEVICE_AUTO:
      haudio_out_sai.SlotInit.SlotActive         = CODEC_AUDIOFRAME_SLOT_0123;
      break;
    case AUDIO_OUT_DEVICE_SPEAKER:
      haudio_out_sai.SlotInit.SlotActive         = CODEC_AUDIOFRAME_SLOT_13;
      break;
    case AUDIO_OUT_DEVICE_HEADPHONE:
    default:
      haudio_out_sai.SlotInit.SlotActive        = CODEC_AUDIOFRAME_SLOT_02;
      break;
    }

    if(HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Enable the SAI to generate MCLK clock for codec configuration */
      __HAL_SAI_ENABLE(&haudio_out_sai);

      /* Call the Codec output device function */
      if(AudioDrv->SetOutputMode(AudioCompObj, Device) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register SAI TC, HT and Error callbacks */
      else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
      else
      {
        ret = BSP_ERROR_NONE;
        /* Update AudioOutCtx structure */
        AudioOutCtx[Instance].Device = Device;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the Output Device
  * @param  Instance  AUDIO OUT Instance. It can only be 0.
  * @param  Device    The audio output device
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetDevice(uint32_t Instance, uint32_t *Device)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Get AudioOutCtx Device */
    *Device = AudioOutCtx[Instance].Device;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Updates the audio frequency.
  * @param  Instance   AUDIO OUT Instance. It can only be 0.
  * @param  SampleRate Audio frequency used to play the audio stream.
  * @note   This API should be called after the BSP_AUDIO_OUT_Init() to adjust the
  *         audio frequency.
  * @note   This API can be only called if the AUDIO OUT state is RESET or
  *         STOP to guarantee a correct SAI behavior
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_SetSampleRate(uint32_t Instance, uint32_t SampleRate)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check audio out state */
  else if (AudioOutCtx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    ret = BSP_ERROR_BUSY;
  }
  /* Check if record on instance 0 is on going and corresponding sample rate */
  else if ((AudioInCtx[0].State != AUDIO_IN_STATE_RESET) &&
           (AudioInCtx[0].SampleRate != SampleRate))
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }/* Check if sample rate is modified */
  else if (AudioOutCtx[Instance].SampleRate == SampleRate)
  {
    /* Nothing to do */
  }
  else
  {
    /* Update the SAI audio frequency configuration */
    haudio_out_sai.Init.AudioFrequency = SampleRate;

    /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
    if(MX_SAI1_ClockConfig(&haudio_out_sai, SampleRate) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Enable the SAI to generate MCLK clock for codec configuration */
      __HAL_SAI_ENABLE(&haudio_out_sai);

      /* Call the Codec output device function */
      if(AudioDrv->SetFrequency(AudioCompObj, SampleRate) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register SAI TC, HT and Error callbacks */
      if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
      if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
      if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

      /* Store new sample rate */
      AudioOutCtx[Instance].SampleRate = SampleRate;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the audio frequency.
  * @param  Instance    AUDIO OUT Instance. It can only be 0.
  * @param  SampleRate  Audio frequency used to play the audio stream.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetSampleRate(uint32_t Instance, uint32_t *SampleRate)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    *SampleRate = AudioOutCtx[Instance].SampleRate;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the audio Resolution.
  * @param  Instance       AUDIO OUT Instance. It can only be 0.
  * @param  BitsPerSample  Audio Resolution used to play the audio stream.
  * @note   This API can be only called if the AUDIO OUT state is RESET or
  *         STOP to guarantee a correct SAI behavior
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample)
{
int32_t ret = BSP_ERROR_NONE;
  uint16_t resolution;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (BitsPerSample == AUDIO_RESOLUTION_8B))
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 0U) && (AudioInCtx[0].State != AUDIO_IN_STATE_RESET) &&
           (AudioInCtx[0].BitsPerSample != BitsPerSample))
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }/* Check audio out state */
  else if (AudioOutCtx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Update data size, frame length and active frame length parameters of SAI handle */
    if (BitsPerSample == AUDIO_RESOLUTION_16B)
    {
      resolution   = 0;
      haudio_out_sai.Init.DataSize               = SAI_DATASIZE_16;
      haudio_out_sai.FrameInit.FrameLength       = 32;
      haudio_out_sai.FrameInit.ActiveFrameLength = 16;
    }
    else
    {
      if (BitsPerSample == AUDIO_RESOLUTION_24B)
      {
        haudio_out_sai.Init.DataSize               = SAI_DATASIZE_24;
      }
      else
      {
        haudio_out_sai.Init.DataSize               = SAI_DATASIZE_32;
      }
      resolution   = 3;
      haudio_out_sai.FrameInit.FrameLength       = 64;
      haudio_out_sai.FrameInit.ActiveFrameLength = 32;
    }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Update SAI state only to keep current MSP functions */
    haudio_out_sai.State = HAL_SAI_STATE_RESET;
#else
    SAI_MspInit(&haudio_out_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

    if(HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      /* Enable the SAI to generate MCLK clock for codec configuration */
      __HAL_SAI_ENABLE(&haudio_out_sai);

      /* Call the Codec output device function */
      if(AudioDrv->SetResolution(AudioCompObj, resolution) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register SAI TC, HT and Error callbacks */
      else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the audio Resolution.
  * @param  Instance       AUDIO OUT Instance. It can only be 0.
  * @param  BitsPerSample  Audio Resolution used to play the audio stream.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio out state */
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Get audio Out resolution */
    *BitsPerSample = AudioOutCtx[Instance].BitsPerSample;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the audio Channels number.
  * @param  Instance       AUDIO OUT Instance. It can only be 0.
  * @param  ChannelNbr     Audio Channels number used to play the audio stream.
  * @note   This API can be only called if the AUDIO OUT state is RESET or
  *         STOP to guarantee a correct SAI behavior
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr)
{
int32_t ret = BSP_ERROR_NONE;

  if((Instance >= AUDIO_OUT_INSTANCES_NBR) || (ChannelNbr > 2U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio out state */
  else if (AudioOutCtx[Instance].State != AUDIO_OUT_STATE_STOP)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Update mono or stereo mode of SAI handle */
    haudio_out_sai.Init.MonoStereoMode = (ChannelNbr == 1U) ? SAI_MONOMODE : SAI_STEREOMODE;

    /* Re-initialize SAI1 with new parameter */
    if(HAL_SAI_Init(&haudio_out_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_COMPLETE_CB_ID, SAI_TxCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_TX_HALFCOMPLETE_CB_ID, SAI_TxHalfCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    else
    {
      /* Store new Channel number */
      AudioOutCtx[Instance].ChannelsNbr = ChannelNbr;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the audio Channels number.
  * @param  Instance       AUDIO OUT Instance. It can only be 0.
  * @param  ChannelNbr     Audio Channels number used to play the audio stream.
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio out state */
  else if (AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Get the current channels number of audio out stream */
    *ChannelNbr = AudioOutCtx[Instance].ChannelsNbr;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get Audio Out state
  * @param  Instance  Audio Out instance
  * @param  State     Audio Out state
  * @retval BSP status
  */
int32_t BSP_AUDIO_OUT_GetState(uint32_t Instance, uint32_t *State)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_OUT_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Return audio Output State */
    *State = AudioOutCtx[Instance].State;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Manage the BSP audio out transfer complete event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manage the BSP audio out half transfer complete event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manages the BSP audio out error event.
  * @param  Instance Audio out instance.
  * @retval None.
  */
__weak void BSP_AUDIO_OUT_Error_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  This function handles Audio Out DMA interrupt requests.
  * @param  Instance  Audio Out instance
  * @retval None
  */
void BSP_AUDIO_OUT_DMA_IRQHandler(uint32_t Instance)
{
  UNUSED(Instance);
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief  SAI1 clock Config.
  * @param  hsai SAI handle.
  * @param  SampleRate Audio sample rate used to play the audio stream.
  * @note   The SAI PLL configuration done within this function assumes that
  *         the SAI PLL input is HSI clock and that HSI is already enabled at 16 MHz.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SAI1_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);

  HAL_StatusTypeDef status;
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct; 
  
  /* save initial RCC configuration */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  /* save initial OSC configuration */
  RCC_OscInitStructSaved = RCC_OscInitStruct;
  RCC_InitSysClkSaved = (__HAL_RCC_GET_SYSCLK_SOURCE() >> RCC_CFGR_SWS_Pos);
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

  /* Set the PLL configuration according to the audio frequency */
  if((SampleRate == AUDIO_FREQUENCY_11K) || (SampleRate == AUDIO_FREQUENCY_22K) || (SampleRate == AUDIO_FREQUENCY_44K) ||
     (SampleRate == AUDIO_FREQUENCY_88K) || (SampleRate == AUDIO_FREQUENCY_176K))
  {
    /* SAI1 clock config requires 11.294MHz
    SAI_CK_x = (16 MHz / PLLM) * PLLN  / PLLQ = 16 / 4 * 23 / 8 = 11.5 MHz */
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 23;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;
  }
  /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_32K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K or AUDIO_FREQUENCY_192K */
  else if((SampleRate == AUDIO_FREQUENCY_8K) || (SampleRate == AUDIO_FREQUENCY_16K) || (SampleRate == AUDIO_FREQUENCY_32K) ||
     (SampleRate == AUDIO_FREQUENCY_48K) || (SampleRate == AUDIO_FREQUENCY_96K) || (SampleRate == AUDIO_FREQUENCY_192K))
  {
    /* SAI1 clock config requires 49.152MHz
    SAI_CK_x = (16 MHz / PLLM) * PLLN  / PLLQ = 16 / 4 * 49 / 4 = 49 MHz */
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 49;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
  }
  /* other AUDIO FREQUENCY are not supported */
  else
  {
    return HAL_ERROR;
  }
  /* in case PLL is system clock, first set system clock on HSI before
     reconfig PLL */
  if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK)
  {
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
  }

  status = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  if ( status == HAL_OK)
  {
     /* save all periph clock Init Struct */
     HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);
     RCC_ExCLKInitStructSaved = RCC_ExCLKInitStruct;
     /* set SAI peripheral clock */
     RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
     RCC_ExCLKInitStruct.Sai1ClockSelection   = RCC_SAI1CLKSOURCE_PLL;
     status = HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
  }

  return status;
}

/**
  * @brief  SAI1 clock deConfig.
  * @param  hsai SAI handle.
  * @note   The PLL configuration done within the value saved during MX_SAI1_ClockConfig
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SAI1_ClockDeConfig(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);
  /* in case PLL is system clock, first set system clock on HSI before
     reconfig PLL */
  if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK)
  {
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
  }
  /* restore initial pll clock config */
  (void)HAL_RCC_OscConfig(&RCC_OscInitStructSaved);
  /* restore all periph clock Init Struct*/
  (void)HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStructSaved);
  
  /* restore PLL */
  __HAL_RCC_SYSCLK_CONFIG(RCC_InitSysClkSaved);

  return HAL_OK;
}

/**
  * @brief  Initialize SAI1.
  * @param  hsai SAI handle.
  * @param  MXConfig SAI configuration structure.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SAI1_Init(SAI_HandleTypeDef* hsai, MX_SAI_InitTypeDef *MXConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hsai->Instance == SAI1_Block_B) || (hsai->Instance == SAI1_Block_A))
  {
    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(hsai);

    /* Configure SAI1_Block_ */
    hsai->Init.MonoStereoMode       = MXConfig->MonoStereoMode;
    hsai->Init.AudioFrequency       = MXConfig->AudioFrequency;
    hsai->Init.AudioMode            = MXConfig->AudioMode;
    hsai->Init.NoDivider            = SAI_MASTERDIVIDER_ENABLE;
    hsai->Init.Protocol             = SAI_FREE_PROTOCOL;
    hsai->Init.DataSize             = MXConfig->DataSize;
    hsai->Init.FirstBit             = SAI_FIRSTBIT_MSB;
    hsai->Init.ClockStrobing        = MXConfig->ClockStrobing;
    hsai->Init.Synchro              = MXConfig->Synchro;
    hsai->Init.OutputDrive          = MXConfig->OutputDrive;
    hsai->Init.FIFOThreshold        = SAI_FIFOTHRESHOLD_HF;
    hsai->Init.SynchroExt           = MXConfig->SynchroExt;
    hsai->Init.CompandingMode       = SAI_NOCOMPANDING;
    hsai->Init.TriState             = SAI_OUTPUT_NOTRELEASED;
    hsai->Init.Mckdiv               = 0U;
    hsai->Init.MckOutput            = SAI_MCK_OUTPUT_ENABLE;
    hsai->Init.MckOverSampling      = SAI_MCK_OVERSAMPLING_DISABLE;
    hsai->Init.PdmInit.Activation   = DISABLE;

    /* Configure SAI1_Block_ Frame */
    hsai->FrameInit.FrameLength       = MXConfig->FrameLength;
    hsai->FrameInit.ActiveFrameLength = MXConfig->ActiveFrameLength;
    hsai->FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai->FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;
    hsai->FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;

    /* Configure SAI1_Block_ Slot */
    hsai->SlotInit.FirstBitOffset     = 0;
    if ((MXConfig->DataSize == AUDIO_RESOLUTION_24B) || (MXConfig->DataSize == AUDIO_RESOLUTION_32B))
    {
      hsai->SlotInit.SlotSize         = SAI_SLOTSIZE_32B;
    }
    else
    {
      hsai->SlotInit.SlotSize         = SAI_SLOTSIZE_16B;
    }
    hsai->SlotInit.SlotNumber         = 2;
    hsai->SlotInit.SlotActive         = MXConfig->SlotActive;

    if(HAL_SAI_Init(hsai) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }
  return status;
}
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_AUDIO_IN_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the audio in peripherals.
  * @param  Instance Audio in instance.
  * @param  AudioInit Audio in init structure.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit)
{
  int32_t status;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Fill audio in context structure */
    AudioInCtx[Instance].Device         = AudioInit->Device;
    AudioInCtx[Instance].SampleRate     = AudioInit->SampleRate;
    AudioInCtx[Instance].BitsPerSample  = AudioInit->BitsPerSample;
    AudioInCtx[Instance].ChannelsNbr    = AudioInit->ChannelsNbr;
    AudioInCtx[Instance].Volume         = AudioInit->Volume;
    AudioInCtx[Instance].State          = AUDIO_IN_STATE_RESET;

    if(Instance == 0U)
    {
      /* PLL clock is set depending by the AudioFreq (44.1khz vs 48khz groups) */
      if(MX_SAI1_ClockConfig(&haudio_in_sai, AudioInit->SampleRate) != HAL_OK)
      {
        return BSP_ERROR_CLOCK_FAILURE;
      }

      haudio_in_sai.Instance = AUDIO_IN_SAIx;
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register the default SAI MSP callbacks */
      if(AudioInCtx[Instance].IsMspCallbacksValid == 0U)
      {
        if(BSP_AUDIO_IN_RegisterDefaultMspCallbacks(0) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#else
      SAI_MspInit(&haudio_in_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
      MX_SAI_InitTypeDef mx_config;

      /* Prepare haudio_in_sai handle */
      mx_config.AudioFrequency    = AudioInit->SampleRate;
      mx_config.AudioMode         = SAI_MODESLAVE_RX;
      mx_config.ClockStrobing     = SAI_CLOCKSTROBING_RISINGEDGE;
      mx_config.MonoStereoMode    = SAI_MONOMODE;
      mx_config.DataSize          = SAI_DATASIZE_16;
      mx_config.FrameLength       = 32;
      mx_config.ActiveFrameLength = 16;
      mx_config.OutputDrive       = SAI_OUTPUTDRIVE_ENABLE;
      mx_config.Synchro           = SAI_SYNCHRONOUS;
      mx_config.SynchroExt        = SAI_SYNCEXT_DISABLE;
      mx_config.SlotActive        = SAI_SLOTACTIVE_0;

      if(MX_SAI1_Init(&haudio_in_sai, &mx_config) != HAL_OK)
      {
        /* Return BSP_ERROR_PERIPH_FAILURE when operations are not correctly done */
        return BSP_ERROR_PERIPH_FAILURE;
      }

      /* Prepare haudio_out_sai handle */
      haudio_out_sai.Instance       = AUDIO_OUT_SAIx;
      /* Initialize SAI peripheral used to generate clock and synchro */
      mx_config.AudioMode  = SAI_MODEMASTER_TX;
      mx_config.Synchro    = SAI_ASYNCHRONOUS;
      mx_config.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;

      if(MX_SAI1_Init(&haudio_out_sai, &mx_config) != HAL_OK)
      {
        /* Return BSP_ERROR_PERIPH_FAILURE when operations are not correctly done */
        return BSP_ERROR_PERIPH_FAILURE;
      }

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      /* Register SAI TC, HT and Error callbacks */
      if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_COMPLETE_CB_ID, SAI_RxCpltCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
      if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_HALFCOMPLETE_CB_ID, SAI_RxHalfCpltCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
      if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
#if (USE_AUDIO_CODEC_WM8994 == 1)
      /* Initialize the codec internal registers */
      if(WM8994_Probe() == BSP_ERROR_NONE)
      {
        WM8994_Init_t codec_init;

        /* Fill codec_init structure */
        codec_init.Frequency    = AudioInit->SampleRate;
        codec_init.OutputDevice =(AudioOutCtx[Instance].State == AUDIO_OUT_STATE_RESET) ? WM8994_OUT_NONE : WM8994_OUT_HEADPHONE;;
        codec_init.InputDevice  = WM8994_IN_LINE1;
        codec_init.Resolution   = WM8994_RESOLUTION_16b;
        codec_init.Volume       = (AudioInit->Volume);

        /* Initialize the codec internal registers */
        if(AudioDrv->Init(AudioCompObj, &codec_init) < 0)
        {
          return BSP_ERROR_COMPONENT_FAILURE;
        }
      }
#endif
    }
    else /* (Instance == 1U) */
    {
      return BSP_ERROR_WRONG_PARAM;
    }

    /* Update BSP AUDIO IN state */
    AudioInCtx[Instance].State = AUDIO_IN_STATE_STOP;
    /* Return BSP status */
    status = BSP_ERROR_NONE;

  }
  return status;
}

/**
  * @brief  De-initialize the audio in peripherals.
  * @param  Instance Audio in instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_DeInit(uint32_t Instance)
{
  int32_t status;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Instance == 0U)
    {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 0)
      SAI_MspDeInit(&haudio_in_sai);
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 0) */

      /* DeInitialize the haudio_in_sai Instance parameter */
      haudio_in_sai.Instance = AUDIO_IN_SAIx;
      /* DeInitialize the haudio_out_sai Instance parameter */
      haudio_out_sai.Instance = AUDIO_OUT_SAIx;

      if(AudioDrv->DeInit(AudioCompObj) < 0)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }/* De-Initializes SAI handles */
      else if(HAL_SAI_DeInit(&haudio_in_sai) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        status = BSP_ERROR_NONE;
      }
      /* Update BSP AUDIO IN state */
      AudioInCtx[Instance].State = AUDIO_IN_STATE_RESET;
    }
    else
    {
     status = BSP_ERROR_WRONG_PARAM;
    }

    /* restore clock */
    (void)MX_SAI1_ClockDeConfig(&haudio_in_sai);
  }

  return status;
}

/**
  * @brief  Start audio recording.
  * @param  Instance AUDIO IN Instance. It can be 0 when SAI is used
  * @param  pBuf           Main buffer pointer for the recorded data storing
  * @param  NbrOfBytes     Size of the record buffer
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t* pBuf, uint32_t NbrOfBytes)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Instance == 0U)
    {
     /* If no playback is on going, just transmit some bytes on audio out stream
        to generate SAI clock and synchro signals */
      if ((AudioOutCtx[0].State != AUDIO_OUT_STATE_PLAYING) && (AudioOutCtx[0].State != AUDIO_OUT_STATE_PAUSE))
      {
        uint8_t TxData[2] = {0x00U, 0x00U};
        if(HAL_SAI_Transmit(&haudio_out_sai, TxData, 2, 1000) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
      }

      if (ret == BSP_ERROR_NONE)
      {
        /* Call the audio Codec Play function */
        if (AudioDrv->Play(AudioCompObj) < 0)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          /* Initiate a DMA transfer of audio samples from the serial audio interface */
          /* Because only 16 bits per sample is supported, DMA transfer is in halfword size */
          if (HAL_SAI_Receive_DMA(&haudio_in_sai, pBuf, (uint16_t) NbrOfBytes/2U) != HAL_OK)
          {
            ret = BSP_ERROR_PERIPH_FAILURE;
          }
          else
          {
            /* Update audio in state */
            AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
          }
        }
      }
    }
    else
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Stop audio recording.
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Stop(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Instance == 0U)
    {
      /* Call the Media layer stop function */
      if(AudioDrv->Stop(AudioCompObj, CODEC_PDWN_SW) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if(HAL_SAI_DMAStop(&haudio_in_sai) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else /*(Instance == 1U) */
    {
        ret = BSP_ERROR_NONE;
    }
    /* Update BSP AUDIO IN state */
    AudioInCtx[Instance].State = AUDIO_IN_STATE_STOP;

  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Pause the audio file stream.
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Pause(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Instance == 0U)
    {
      /* Call Audio Codec Pause function */
      if(AudioDrv->Pause(AudioCompObj) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if(HAL_SAI_DMAPause(&haudio_in_sai) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else /* (Instance == 1U) */
    {
        ret = BSP_ERROR_NONE;
    }
    /* Update BSP AUDIO IN state */
    AudioInCtx[Instance].State = AUDIO_IN_STATE_PAUSE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Resume the audio file stream.
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Resume(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Instance == 0U)
    {
      /* Call Audio Codec Pause function */
      if(HAL_SAI_DMAResume(&haudio_in_sai) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else if(AudioDrv->Resume(AudioCompObj) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else /* (Instance == 1U) */
    {
        ret = BSP_ERROR_NONE;
    }
    /* Update BSP AUDIO IN state */
    AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Audio In device
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @param  Device    The audio input device to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device)
{
    int32_t ret;

    if(Instance >= AUDIO_IN_INSTANCES_NBR)
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
    else if(AudioInCtx[Instance].State != AUDIO_IN_STATE_STOP)
    {
      ret = BSP_ERROR_BUSY;
    }
    else
    {
      UNUSED(Device);
      ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    }

    /* Return BSP status */
    return ret;
}

/**
  * @brief  Get Audio In device
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @param  Device    The audio input device used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio in state */
  else if (AudioInCtx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Return audio Input Device */
    *Device = AudioInCtx[Instance].Device;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Audio In frequency
  * @param  Instance     Audio IN instance
  * @param  SampleRate  Input frequency to be set
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t  SampleRate)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio in state */
  else if (AudioInCtx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    ret = BSP_ERROR_BUSY;
  }/* Check if playback on instance 0 is on going and corresponding sample rate */
  else if ((AudioOutCtx[0].State != AUDIO_OUT_STATE_RESET) &&
           (AudioOutCtx[0].SampleRate != SampleRate))
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }/* Check if sample rate is modified */
  else if (AudioInCtx[Instance].SampleRate == SampleRate)
  {
    /* Nothing to do */
  }
  else
  {
    /* Update SAI1 clock config */
    haudio_in_sai.Init.AudioFrequency = SampleRate;
    if (MX_SAI1_ClockConfig(&haudio_in_sai, SampleRate) != HAL_OK)
    {
      ret = BSP_ERROR_CLOCK_FAILURE;
    }
    /* Re-initialize SAI1 with new sample rate */
    else if (HAL_SAI_Init(&haudio_in_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_COMPLETE_CB_ID, SAI_RxCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_HALFCOMPLETE_CB_ID, SAI_RxHalfCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    /* Store new sample rate on audio in context */
    else
    {
      AudioInCtx[Instance].SampleRate = SampleRate;
    }
  }
  /* Return BSP status */
  return ret;
}

/**
* @brief  Get Audio In frequency
* @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
* @param  SampleRate  Audio Input frequency to be returned
* @retval BSP status
*/
int32_t BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio in state */
  else if (AudioInCtx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Return audio in frequency */
    *SampleRate = AudioInCtx[Instance].SampleRate;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Audio In Resolution
  * @param  Instance       AUDIO IN Instance. It can be 0 when SAI is used
  * @param  BitsPerSample  Input resolution to be set
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample)
{
int32_t ret = BSP_ERROR_NONE;
  uint32_t resolution;

  if((Instance >= AUDIO_IN_INSTANCES_NBR) || (BitsPerSample == AUDIO_RESOLUTION_8B))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio in state */
  else if (AudioInCtx[Instance].State != AUDIO_IN_STATE_STOP)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Update data size, frame length and active frame length parameters of SAI handle */
    if (BitsPerSample == AUDIO_RESOLUTION_16B)
    {
      resolution   = 0;
      haudio_in_sai.Init.DataSize               = SAI_DATASIZE_16;
      haudio_in_sai.FrameInit.FrameLength       = 32;
      haudio_in_sai.FrameInit.ActiveFrameLength = 16;
    }
    else
    {
      if (BitsPerSample == AUDIO_RESOLUTION_24B)
      {
        haudio_in_sai.Init.DataSize               = SAI_DATASIZE_24;
      }
      else
      {
        haudio_in_sai.Init.DataSize               = SAI_DATASIZE_32;
      }
      resolution   = 3;
      haudio_in_sai.FrameInit.FrameLength       = 64;
      haudio_in_sai.FrameInit.ActiveFrameLength = 32;
    }

    /* Call the Codec input device function */
    if(AudioDrv->SetResolution(AudioCompObj, resolution) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Re-initialize SAI1 with new sample rate */
    else if (HAL_SAI_Init(&haudio_in_sai) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register SAI TC, HT and Error callbacks */
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_COMPLETE_CB_ID, SAI_RxCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_RX_HALFCOMPLETE_CB_ID, SAI_RxHalfCpltCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_ERROR_CB_ID, SAI_ErrorCallback) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    /* Store new sample rate on audio in context */
    else
    {
      AudioInCtx[Instance].BitsPerSample = BitsPerSample;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get Audio In Resolution
  * @param  Instance       AUDIO IN Instance. It can be 0 when SAI is used
  * @param  BitsPerSample  Input resolution to be returned
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample)
{
int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Check audio in state */
  else if (AudioInCtx[Instance].State == AUDIO_IN_STATE_RESET)
  {
    ret = BSP_ERROR_BUSY;
  }
  else
  {
    /* Return audio in resolution */
    *BitsPerSample = AudioInCtx[Instance].BitsPerSample;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Audio In Channel number
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @param  ChannelNbr  Channel number to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= AUDIO_IN_INSTANCES_NBR) || (ChannelNbr > 2U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Update AudioIn Context */
    AudioInCtx[Instance].ChannelsNbr = ChannelNbr;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get Audio In Channel number
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @param  ChannelNbr  Channel number to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Channel number to be returned */
    *ChannelNbr = AudioInCtx[Instance].ChannelsNbr;
  }
  return ret;
}

/**
  * @brief  Set the current audio in volume level.
  * @param  Instance  AUDIO IN Instance. It can only be 0
  * @param  Volume    Volume level to be returnd
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(AudioDrv->SetVolume(AudioCompObj, AUDIO_VOLUME_INPUT, Volume) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Update AudioIn Context */
    AudioInCtx[Instance].Volume = Volume;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the current audio in volume level.
  * @param  Instance  AUDIO IN Instance. It can only be 0
  * @param  Volume    Volume level to be returned
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Input Volume to be returned */
    *Volume = AudioInCtx[Instance].Volume;
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get Audio In device
  * @param  Instance    AUDIO IN Instance. It can be 0 when SAI is used
  * @param  State     Audio Out state
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Input State to be returned */
    *State = AudioInCtx[Instance].State;
  }
  return ret;
}

#if ((USE_HAL_SAI_REGISTER_CALLBACKS == 1))
/**
  * @brief  Register default BSP AUDIO IN msp callbacks.
  * @param  Instance AUDIO IN Instance.
  * @retval BSP status.
  */
int32_t BSP_AUDIO_IN_RegisterDefaultMspCallbacks(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
   if (Instance == 0U)
   {
    __HAL_SAI_RESET_HANDLE_STATE(&haudio_in_sai);
    __HAL_SAI_RESET_HANDLE_STATE(&haudio_out_sai);

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPINIT_CB_ID, SAI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPINIT_CB_ID, SAI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPDEINIT_CB_ID, SAI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SAI_RegisterCallback(&haudio_out_sai, HAL_SAI_MSPDEINIT_CB_ID, SAI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      AudioInCtx[Instance].IsMspCallbacksValid = 1;
    }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
   }
   else /* Instance = 1 */
   {
     ret = BSP_ERROR_WRONG_PARAM;
   }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Register BSP AUDIO IN msp callbacks.
  * @param  Instance AUDIO IN Instance.
  * @param  CallBacks Pointer to MspInit/MspDeInit callback functions.
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_RegisterMspCallbacks(uint32_t Instance, BSP_AUDIO_IN_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Instance == 0U)
    {
#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
      if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPINIT_CB_ID, CallBacks->pMspSaiInitCb) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else if(HAL_SAI_RegisterCallback(&haudio_in_sai, HAL_SAI_MSPDEINIT_CB_ID, CallBacks->pMspSaiDeInitCb) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        AudioInCtx[Instance].IsMspCallbacksValid = 1;
      }
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
    }
    else /* Instance = 1 */
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* ((USE_HAL_SAI_REGISTER_CALLBACKS == 1)) */

/**
  * @brief  Manage the BSP audio in transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manage the BSP audio in half transfer complete event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  Manages the BSP audio in error event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
__weak void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  This function handles Audio In DMA interrupt requests.
  * @param  Instance Audio IN instance: Can be only 0 for SAI
  * @param  InputDevice Can be:
  *         - AUDIO_IN_DEVICE_ANALOG_MIC
  * @retval None
  */
void BSP_AUDIO_IN_DMA_IRQHandler(uint32_t Instance, uint32_t InputDevice)
{
  UNUSED(Instance);
  UNUSED(InputDevice);
  HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_AUDIO_Private_Functions Private Functions
  * @{
  */
#if (USE_AUDIO_CODEC_WM8994 == 1)
/**
  * @brief  Probe the WM8994 audio codec.
  * @retval BSP status.
  */
static int32_t WM8994_Probe(void)
{
  int32_t                  status = BSP_ERROR_NONE;
  WM8994_IO_t              IOCtx;
  uint32_t                 wm8994_id;
  static WM8994_Object_t   WM8994Obj;

  /* Configure the audio driver */
  IOCtx.Address     = AUDIO_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C3_Init;
  IOCtx.DeInit      = BSP_I2C3_DeInit;
  IOCtx.ReadReg     = BSP_I2C3_ReadReg16;
  IOCtx.WriteReg    = BSP_I2C3_WriteReg16;
  IOCtx.GetTick     = BSP_GetTick;

  if (WM8994_RegisterBusIO(&WM8994Obj, &IOCtx) != WM8994_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else
  {
    if (WM8994_ReadID(&WM8994Obj, &wm8994_id) != WM8994_OK)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if ((wm8994_id & 0xFFFFU) != WM8994_ID)
    {
      status = BSP_ERROR_UNKNOWN_COMPONENT;
    }
    /* Reset the Codec Registers */
    else if (WM8994_Reset(&WM8994Obj) != WM8994_OK)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      AudioDrv = (AUDIO_Drv_t *) &WM8994_Driver;
      AudioCompObj = &WM8994Obj;
    }
  }
  return status;
}
#endif /* USE_AUDIO_CODEC_WM8994 */

/**
  * @brief  Initialize SAI MSP.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_MspInit(SAI_HandleTypeDef *hsai)
{
  static DMA_HandleTypeDef hDmaSaiTx, hDmaSaiRx;;
  GPIO_InitTypeDef         GPIO_InitStruct;

  /* Enable SAI clock */
  __HAL_RCC_SAI1_CLK_ENABLE();

   /* SAI pins configuration: FS, SCK, MCLK and SD pins */
   __HAL_RCC_GPIOE_CLK_ENABLE();
   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull      = GPIO_NOPULL;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
   GPIO_InitStruct.Pin       = GPIO_PIN_2; /* SAI1_MCLK_A */
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   GPIO_InitStruct.Pin       = GPIO_PIN_4; /* SAI1_FS_A */
   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   __HAL_RCC_GPIOD_CLK_ENABLE();
   GPIO_InitStruct.Pin       = GPIO_PIN_6; /* SAI1_SD_A : PD6 */
   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
   __HAL_RCC_GPIOA_CLK_ENABLE();           /* SAI1_SCK_A */
   GPIO_InitStruct.Alternate = GPIO_AF14_SAI1;
   GPIO_InitStruct.Pin       = GPIO_PIN_8;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* Configure the hDmaSaiTx handle parameters */
   __HAL_RCC_DMA2_CLK_ENABLE();
   __HAL_RCC_DMAMUX1_CLK_ENABLE();

   if (hsai->Instance == AUDIO_OUT_SAIx)
   {
    hDmaSaiTx.Init.Request             = DMA_REQUEST_SAI1_A;
    hDmaSaiTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hDmaSaiTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiTx.Init.MemInc              = DMA_MINC_ENABLE;
    if (AudioOutCtx[0].BitsPerSample == AUDIO_RESOLUTION_16B)
    {
      hDmaSaiTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hDmaSaiTx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    }
    else /* AUDIO_RESOLUTION_24B */
    {
      hDmaSaiTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hDmaSaiTx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    }
    hDmaSaiTx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiTx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiTx.Instance                 = DMA2_Channel1;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmatx, hDmaSaiTx);
    /* Deinitialize the Stream for new transfer */
    if (HAL_DMA_DeInit(&hDmaSaiTx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* Configure the DMA Stream */
    if (HAL_DMA_Init(&hDmaSaiTx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, BSP_AUDIO_OUT_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  }

   /* Audio In Msp initialization */
   if(hsai->Instance == AUDIO_IN_SAIx)
   {
    /* SAI pins configuration: SD pin */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_SAI1;
    GPIO_InitStruct.Pin       = GPIO_PIN_3; /* SAI1_SD_B : PE3 */
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Configure the hDmaSaiRx handle parameters */
    hDmaSaiRx.Init.Request             = DMA_REQUEST_SAI1_B;
    hDmaSaiRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hDmaSaiRx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hDmaSaiRx.Init.MemInc              = DMA_MINC_ENABLE;
    if (AudioInCtx[0].BitsPerSample == AUDIO_RESOLUTION_16B)
    {
      hDmaSaiRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hDmaSaiRx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    }
    else /* AUDIO_RESOLUTION_24B */
    {
      hDmaSaiRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hDmaSaiRx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    }
    hDmaSaiRx.Init.Mode                = DMA_CIRCULAR;
    hDmaSaiRx.Init.Priority            = DMA_PRIORITY_HIGH;
    hDmaSaiRx.Instance                 = DMA2_Channel2;
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsai, hdmarx, hDmaSaiRx);
    /* Deinitialize the Stream for new transfer */
    if (HAL_DMA_DeInit(&hDmaSaiRx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* Configure the DMA Stream */
    if (HAL_DMA_Init(&hDmaSaiRx) != HAL_OK)
    {
      /* Nothing to do */
    }
    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, BSP_AUDIO_IN_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
  }
}

/**
  * @brief  Deinitialize SAI MSP.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
  if(hsai->Instance == AUDIO_OUT_SAIx)
  {
    /* Disable SAI DMA Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA2_Channel1_IRQn);

    /* Reset the DMA Stream configuration*/
    if (HAL_DMA_DeInit(hsai->hdmatx) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* Don't disable the DMA clock potentially used for other SAI block */
    /* __HAL_RCC_DMA2_CLK_DISABLE(); */
    /* __HAL_RCC_DMAMUX1_CLK_DISABLE(); */

    /* De-initialize SD pin*/
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_3);  /* SAI1_SD_B */

    /* Don't disable SAI clock potentially used for other SAI block */
    /*__HAL_RCC_SAI1_CLK_DISABLE(); */
  }

  if(hsai->Instance == AUDIO_IN_SAIx)
  {
    /* Disable SAI DMA Channel IRQ */
    HAL_NVIC_DisableIRQ(DMA2_Channel2_IRQn);

    /* Reset the DMA Stream configuration*/
    if (HAL_DMA_DeInit(hsai->hdmarx) != HAL_OK)
    {
      /* Nothing to do */
    }

    /* Don't disable the DMA clock potentially used for other SAI block */
    /* __HAL_RCC_DMA2_CLK_DISABLE(); */
    /* __HAL_RCC_DMAMUX1_CLK_DISABLE(); */

    /* De-initialize FS, SCK, MCK and SD pins*/
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);  /* SAI1_MCLK_A */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4);  /* SAI1_FS_A */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);  /* SAI1_SCK_A */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6);  /* SAI1_SD_A */

  }

  /* Don't disable SAI clock potentially used for other SAI block */
  /*__HAL_RCC_SAI1_CLK_DISABLE(); */
}

#if (USE_HAL_SAI_REGISTER_CALLBACKS == 1)
/**
  * @brief  SAI Tx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Tx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI Rx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Rx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI error callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
static void SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_Error_CallBack(0);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_Error_CallBack(0);
  }
}
#else /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */
/**
  * @brief  SAI Tx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Tx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI Rx transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_TransferComplete_CallBack(0);
  }
}

/**
  * @brief  SAI Rx half transfer complete callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_HalfTransfer_CallBack(0);
  }
}

/**
  * @brief  SAI error callback.
  * @param  hsai SAI handle.
  * @retval None.
  */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  if (hsai->Instance == SAI1_Block_A)
  {
    BSP_AUDIO_OUT_Error_CallBack(0);
  }
  if (hsai->Instance == SAI1_Block_B)
  {
    BSP_AUDIO_IN_Error_CallBack(0);
  }
}
#endif /* (USE_HAL_SAI_REGISTER_CALLBACKS == 1) */

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
