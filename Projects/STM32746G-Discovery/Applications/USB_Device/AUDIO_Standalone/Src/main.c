// main.c
char* kVersion = "UAC 10am 13/2/18";
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f7xx_hal.h"

#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_ioreq.h"

#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_audio.h"
//}}}
#define AUDIO_CHANNELS    2
#define AUDIO_SAMPLE_RATE 48000
//{{{  packet defines
#define AUDIO_PACKETS               40

#define AUDIO_BYTES_PER_SAMPLE      2
#define AUDIO_PACKETS_PER_SECOND    1000

#define AUDIO_PACKET_SAMPLES        (AUDIO_SAMPLE_RATE / AUDIO_PACKETS_PER_SECOND)
#define AUDIO_PACKET_SIZE           (AUDIO_CHANNELS * AUDIO_BYTES_PER_SAMPLE * AUDIO_PACKET_SAMPLES)

#define AUDIO_MAX_CHANNELS          4
#define AUDIO_MAX_SAMPLE_RATE       48000
#define AUDIO_MAX_PACKET_SAMPLES    (AUDIO_MAX_SAMPLE_RATE / AUDIO_PACKETS_PER_SECOND)
#define AUDIO_MAX_PACKET_SIZE       (AUDIO_MAX_CHANNELS * AUDIO_BYTES_PER_SAMPLE * AUDIO_MAX_PACKET_SAMPLES)

#define AUDIO_SLOTS                 4
#define AUDIO_SLOTS_PACKET_SIZE     (AUDIO_SLOTS * AUDIO_BYTES_PER_SAMPLE * AUDIO_PACKET_SAMPLES)
#define AUDIO_SLOTS_PACKET_BUF_SIZE (AUDIO_PACKETS * AUDIO_SLOTS_PACKET_SIZE)

#define AUDIO_MAX_PACKET_SIZE_DESC  (uint8_t)(AUDIO_MAX_PACKET_SIZE & 0xFF), (uint8_t)((AUDIO_MAX_PACKET_SIZE >> 8) & 0xFF)
//}}}
//{{{  descriptor defines
#define USBD_VID              0x0483
#define USBD_PID              0x5730
#define USBD_LANGID_STRING    0x409
#define USB_SIZ_STRING_SERIAL 0x1A

#define AUDIO_OUT_EP                        0x01

#define AUDIO_DESCRIPTOR_TYPE               0x21
#define USB_DEVICE_CLASS_AUDIO              0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL         0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING       0x02
#define AUDIO_PROTOCOL_UNDEFINED            0x00
#define AUDIO_STREAMING_GENERAL             0x01
#define AUDIO_STREAMING_FORMAT_TYPE         0x02

// Audio Descriptor Types
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE     0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE      0x25

// Audio Control Interface Descriptor Subtypes
#define AUDIO_CONTROL_HEADER                0x01
#define AUDIO_CONTROL_INPUT_TERMINAL        0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL       0x03
#define AUDIO_CONTROL_FEATURE_UNIT          0x06

#define AUDIO_INPUT_TERMINAL_DESC_SIZE      0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE     0x09
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE 0x07

#define AUDIO_CONTROL_MUTE         0x01
#define AUDIO_CONTROL_VOLUME       0x02
#define AUDIO_DEFAULT_VOLUME       100

#define AUDIO_FORMAT_TYPE_I        0x01
#define AUDIO_ENDPOINT_GENERAL     0x01
#define AUDIO_REQ_GET_CUR          0x81
#define AUDIO_REQ_SET_CUR          0x01
#define AUDIO_OUT_STREAMING_CTRL   0x02
//}}}

//{{{  graphics, debug
static int oldFaster = 1;
static int writePtrOnRead = 0;
static int debugOffset = 1;
static int debugLine = 0;
static int debugSize = 16;
static char str[40];
//{{{
static void debug (int col) {

  BSP_LCD_SetTextColor (col);
  BSP_LCD_DisplayStringAtLine (debugOffset + (debugLine % debugSize), (uint8_t*)str);
  debugLine++;
  BSP_LCD_ClearStringLine (debugOffset + (debugLine % debugSize));
  }
//}}}
//{{{
static void touch() {

  TS_StateTypeDef TS_State;
  BSP_TS_GetState (&TS_State);
  if (TS_State.touchDetected == 1) {
    if (TS_State.touchWeight[0] > 128)
      BSP_LCD_SetTextColor (LCD_COLOR_GREEN);
    else
      BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);

    BSP_TS_GetState (&TS_State);
    BSP_LCD_FillCircle (TS_State.touchX[0], TS_State.touchY[0], 20);
    }
  }
//}}}
//{{{
static void initGraphics() {

  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit (1, LCD_FB_START_ADDRESS);
  BSP_LCD_SelectLayer(1);

  BSP_LCD_SetBackColor (LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetFont (&Font16);
  BSP_LCD_DisplayOn();
  }
//}}}
//}}}

USBD_HandleTypeDef gUsbDevice;
PCD_HandleTypeDef gPcdHandle;
//{{{  interrupt, system handlers
void NMI_Handler() {}
void SVC_Handler() {}
void PendSV_Handler() {}
void DebugMon_Handler() {}

void BusFault_Handler() { while (1) {} }
void HardFault_Handler() { while (1) {} }
void MemManage_Handler() { while (1) {} }
void UsageFault_Handler() { while (1) {} }

void SysTick_Handler() { HAL_IncTick(); }

extern SAI_HandleTypeDef haudio_out_sai;
void DMA2_Stream4_IRQHandler() { HAL_DMA_IRQHandler (haudio_out_sai.hdmatx); }
//}}}
//{{{  usbd pcd handler
void OTG_FS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }
void OTG_HS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }

//{{{
void HAL_PCD_MspInit (PCD_HandleTypeDef* pcdHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  if (pcdHandle->Instance == USB_OTG_FS) {
    /* Configure USB FS GPIOs */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure DM DP Pins */
    GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Enable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Set USBFS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);

    /* Enable USBFS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    }

  else if(pcdHandle->Instance == USB_OTG_HS) {
    /* Configure USB FS GPIOs */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* CLK */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* D0 */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* D1 D2 D3 D4 D5 D6 D7 */
    GPIO_InitStruct.Pin = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_5 |\
      GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* STP */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* NXT */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* DIR */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();

    /* Enable USB HS Clocks */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    /* Set USBHS Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 5, 0);

    /* Enable USBHS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
    }
  }
//}}}
//{{{
void HAL_PCD_MspDeInit (PCD_HandleTypeDef* pcdHandle) {

  if (pcdHandle->Instance == USB_OTG_FS) {
    /* Disable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
    }
  else if (pcdHandle->Instance == USB_OTG_HS) {
    /* Disable USB HS Clocks */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
    __HAL_RCC_SYSCFG_CLK_DISABLE();
    }
  }
//}}}

//{{{
void HAL_PCD_SOFCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_SOF (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_SuspendCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_Suspend (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ResumeCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_Resume (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ConnectCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_DevConnected (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_DisconnectCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_DevDisconnected (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_SetupStageCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_SetupStage(pcdHandle->pData, (uint8_t*)pcdHandle->Setup);
  }
//}}}
//{{{
void HAL_PCD_DataOutStageCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {
  USBD_LL_DataOutStage (pcdHandle->pData, epnum, pcdHandle->OUT_ep[epnum].xfer_buff);
  }
//}}}
//{{{
void HAL_PCD_DataInStageCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {
  USBD_LL_DataInStage (pcdHandle->pData, epnum, pcdHandle->IN_ep[epnum].xfer_buff);
  }
//}}}
//{{{
void HAL_PCD_ISOOUTIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {
  USBD_LL_IsoOUTIncomplete (pcdHandle->pData, epnum);
  }
//}}}
//{{{
void HAL_PCD_ISOINIncompleteCallback (PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {
  USBD_LL_IsoINIncomplete (pcdHandle->pData, epnum);
  }
//}}}
//{{{
void HAL_PCD_ResetCallback (PCD_HandleTypeDef* pcdHandle) {

  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  /* Set USB Current Speed */
  switch(pcdHandle->Init.speed) {
    case PCD_SPEED_HIGH: speed = USBD_SPEED_HIGH; break;
    case PCD_SPEED_FULL: speed = USBD_SPEED_FULL; break;
    default: speed = USBD_SPEED_FULL; break;
    }

  /* Reset Device */
  USBD_LL_Reset (pcdHandle->pData);
  USBD_LL_SetSpeed (pcdHandle->pData, speed);
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_LL_Init (USBD_HandleTypeDef* device) {

#ifdef USE_USB_FS
  /* Set LL Driver parameters */
  gPcdHandle.Instance = USB_OTG_FS;
  gPcdHandle.Init.dev_endpoints = 4;
  gPcdHandle.Init.use_dedicated_ep1 = 0;
  gPcdHandle.Init.ep0_mps = 0x40;
  gPcdHandle.Init.dma_enable = 0;
  gPcdHandle.Init.low_power_enable = 0;
  gPcdHandle.Init.phy_itface = PCD_PHY_EMBEDDED;
  gPcdHandle.Init.Sof_enable = 0;
  gPcdHandle.Init.speed = PCD_SPEED_FULL;
  gPcdHandle.Init.vbus_sensing_enable = 0;
  gPcdHandle.Init.lpm_enable = 0;

  /* Link The driver to the stack */
  gPcdHandle.pData = device;
  device->pData = &gPcdHandle;

  /* Initialize LL Driver */
  HAL_PCD_Init (&gPcdHandle);
  HAL_PCDEx_SetRxFiFo (&gPcdHandle, 0x80);
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x60);
#endif

#ifdef USE_USB_HS
  /* Set LL Driver parameters */
  gPcdHandle.Instance = USB_OTG_HS;
  gPcdHandle.Init.dev_endpoints = 6;
  gPcdHandle.Init.use_dedicated_ep1 = 0;
  gPcdHandle.Init.ep0_mps = 0x40;

  // Be aware that enabling DMA mode will result in data being sent only by
  // multiple of 4 packet sizes. This is due to the fact that USB DMA does
  // not allow sending data from non word-aligned addresses.
  // For this specific application, it is advised to not enable this option unless required. */
  gPcdHandle.Init.dma_enable = 0;
  gPcdHandle.Init.low_power_enable = 0;
  gPcdHandle.Init.lpm_enable = 0;
  gPcdHandle.Init.phy_itface = PCD_PHY_ULPI;
  gPcdHandle.Init.Sof_enable = 0;
  gPcdHandle.Init.speed = PCD_SPEED_HIGH;
  gPcdHandle.Init.vbus_sensing_enable = 1;

  /* Link The driver to the stack */
  gPcdHandle.pData = device;
  device->pData = &gPcdHandle;

  /* Initialize LL Driver */
  HAL_PCD_Init (&gPcdHandle);
  HAL_PCDEx_SetRxFiFo (&gPcdHandle, 0x100);
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x200);
#endif

  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_DeInit (USBD_HandleTypeDef* device) {
  HAL_PCD_DeInit (device->pData);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Start (USBD_HandleTypeDef* device) {
  HAL_PCD_Start (device->pData);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Stop (USBD_HandleTypeDef* device) {
  HAL_PCD_Stop (device->pData);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_OpenEP (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
  HAL_PCD_EP_Open (device->pData, ep_addr, ep_mps, ep_type);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_CloseEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_Close (device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_FlushEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_Flush (device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_StallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_SetStall (device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_ClearStallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  HAL_PCD_EP_ClrStall (device->pData, ep_addr);
  return USBD_OK;
  }
//}}}
//{{{
uint8_t USBD_LL_IsStallEP (USBD_HandleTypeDef* device, uint8_t ep_addr) {

  PCD_HandleTypeDef* gPcdHandle = device->pData;
  if ((ep_addr & 0x80) == 0x80)
    return gPcdHandle->IN_ep[ep_addr & 0xF].is_stall;
  else
    return gPcdHandle->OUT_ep[ep_addr & 0xF].is_stall;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_SetUSBAddress (USBD_HandleTypeDef* device, uint8_t device_addr) {
  HAL_PCD_SetAddress (device->pData, device_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Transmit (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Transmit (device->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_PrepareReceive (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
  HAL_PCD_EP_Receive (device->pData, ep_addr, pbuf, size);
  return USBD_OK;
  }
//}}}
//{{{
uint32_t USBD_LL_GetRxDataSize (USBD_HandleTypeDef* device, uint8_t ep_addr) {
  return HAL_PCD_EP_GetRxCount (device->pData, ep_addr);
  }
//}}}
//{{{
void USBD_LL_Delay (uint32_t Delay) {
  HAL_Delay (Delay);
  }
//}}}
//}}}
//{{{  usb audio
//{{{  device descriptor
__ALIGN_BEGIN static const uint8_t kDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12,                                // bLength
  USB_DESC_TYPE_DEVICE,                // bDescriptorType
  0x00, 0x02,                          // bcdUSB
  0x00,                                // bDeviceClass
  0x00,                                // bDeviceSubClass
  0x00,                                // bDeviceProtocol
  USB_MAX_EP0_SIZE,                    // bMaxPacketSize
  LOBYTE(USBD_VID), HIBYTE(USBD_VID),  // vid
  LOBYTE(USBD_PID), HIBYTE(USBD_PID),  // pid
  0x00, 0x02,                          // bcdDevice rel 2.00
  USBD_IDX_MFC_STR,                    // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,                // Index of product string
  USBD_IDX_SERIAL_STR,                 // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION           // bNumConfigurations
  };
//}}}
//{{{  configuration descriptor
#define AUDIO_CONFIG_DESC_SIZ 118
__ALIGN_BEGIN static const uint8_t kConfigDescriptor[AUDIO_CONFIG_DESC_SIZ] __ALIGN_END = {
  // Configuration Descriptor
  9,                           // bLength
  USB_DESC_TYPE_CONFIGURATION, // bDescriptorType
  LOBYTE(AUDIO_CONFIG_DESC_SIZ), HIBYTE(AUDIO_CONFIG_DESC_SIZ), // wTotalLength
  2,                           // bNumInterfaces
  1,                           // bConfigurationValue
  0,                           // iConfiguration
  0xC0,                        // bmAttributes - BUS Powred
  0x32,                        // bMaxPower = 100 mA

  // Control Interface Descriptor
  9,                           // bLength
  USB_DESC_TYPE_INTERFACE,     // bDescriptorType
  0,                           // bInterfaceNumber
  0,                           // bAlternateSetting
  0,                           // bNumEndpoints
  USB_DEVICE_CLASS_AUDIO,      // bInterfaceClass
  AUDIO_SUBCLASS_AUDIOCONTROL, // bInterfaceSubClass
  AUDIO_PROTOCOL_UNDEFINED,    // bInterfaceProtocol
  0,                           // iInterface

  //{{{  speaker control input,feature,output
  //  class-specific AC interface descriptor
  9,                               // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_CONTROL_HEADER,            // bDescriptorSubtype
  0x00,0x01,                       // bcdADC - 1.00
  39,0,                            // wTotalLength
  1,                               // bInCollection
  1,                               // baInterfaceNr

  // audio input terminal descriptor
  12,                              // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_CONTROL_INPUT_TERMINAL,    // bDescriptorSubtype
  1,                               // bTerminalID
  1,1,                             // wTerminalType AUDIO_TERMINAL_USB_STREAMING - 0x0101
  0x00,                            // bAssocTerminal
  AUDIO_CHANNELS,                  // bNrChannels
  0x33,0x00,                       // wChannelConfig - 0x0033 - leftFront rightFront, leftSurround, rightSurround
  0,                               // iChannelNames
  0,                               // iTerminal

  // audio feature Unit Descriptor
  9,                               // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_CONTROL_FEATURE_UNIT,      // bDescriptorSubtype
  AUDIO_OUT_STREAMING_CTRL,        // bUnitID
  1,                               // bSourceID
  1,                               // bControlSize
  AUDIO_CONTROL_MUTE,0,            // bmaControls | AUDIO_CONTROL_VOLUME,
  0,                               // iTerminal

  // audio output Terminal Descriptor
  9,                               // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_CONTROL_OUTPUT_TERMINAL,   // bDescriptorSubtype
  3,                               // bTerminalID
  1,3,                             // wTerminalType - speaker 0x0301
  0,                               // bAssocTerminal
  2,                               // bSourceID
  0,                               // iTerminal
  //}}}
  //{{{  speaker streaming interface
  // Speaker AS Interface Descriptor Audio Streaming Zero Band   Interface 1 Alternate Setting 0
  9,                               // bLength
  USB_DESC_TYPE_INTERFACE,         // bDescriptorType
  1,                               // bInterfaceNumber
  0,                               // bAlternateSetting
  0,                               // bNumEndpoints
  USB_DEVICE_CLASS_AUDIO,          // bInterfaceClass
  AUDIO_SUBCLASS_AUDIOSTREAMING,   // bInterfaceSubClass
  AUDIO_PROTOCOL_UNDEFINED,        // bInterfaceProtocol
  0,                              // iInterface

  // Speaker AS Interface Descriptor Audio Streaming Operational Interface 1 Alternate Setting 1
  9,                               // bLength
  USB_DESC_TYPE_INTERFACE,         // bDescriptorType
  1,                               // bInterfaceNumber
  1,                               // bAlternateSetting
  1,                               // bNumEndpoints
  USB_DEVICE_CLASS_AUDIO,          // bInterfaceClass
  AUDIO_SUBCLASS_AUDIOSTREAMING,   // bInterfaceSubClass
  AUDIO_PROTOCOL_UNDEFINED,        // bInterfaceProtocol
  0,                               // iInterface

  // Speaker Audio Streaming Interface Descriptor
  7,                               // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_STREAMING_GENERAL,         // bDescriptorSubtype
  1,                               // bTerminalLink
  1,                               // bDelay
  1,0,                             // wFormatTag AUDIO_FORMAT_PCM - 0x0001

  // Speaker Audio Type I Format Interface Descriptor
  20,                              // bLength
  AUDIO_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType
  AUDIO_STREAMING_FORMAT_TYPE,     // bDescriptorSubtype
  AUDIO_FORMAT_TYPE_I,             // bFormatType
  AUDIO_CHANNELS,                  // bNrChannels
  2,                               // bSubFrameSize - 2bytes per frame (16bits)
  16,                              // bBitResolution - 16bits per sample
  4,                               // bSamFreqType - single frequency supported
  48000 & 0xFF, (48000 >> 8) & 0xFF, 48000 >> 16, // audio sampling frequency coded on 3 bytes
  44100 & 0xFF, (44100 >> 8) & 0xFF, 44100 >> 16, // audio sampling frequency coded on 3 bytes
  32000 & 0xFF, (32000 >> 8) & 0xFF, 32000 >> 16, // audio sampling frequency coded on 3 bytes
  16000 & 0xFF, (16000 >> 8) & 0xFF, 16000 >> 16, // audio sampling frequency coded on 3 bytes
  //}}}

  // Endpoint1 Standard Descriptor
  9,                                 // bLength
  USB_DESC_TYPE_ENDPOINT,            // bDescriptorType
  AUDIO_OUT_EP,                      // bEndpointAddress 1 - out endpoint
  USBD_EP_TYPE_ISOC,                 // bmAttributes
  AUDIO_MAX_PACKET_SIZE_DESC,        // wMaxPacketSize bytes
  1,                                 // bInterval
  0,                                 // bRefresh
  0,                                 // bSynchAddress

  // Endpoint Audio Streaming Descriptor
  7,                                  // bLength
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,     // bDescriptorType
  AUDIO_ENDPOINT_GENERAL,             // bDescriptor
  0,                                  // bmAttributes
  0,                                  // bLockDelayUnits
  0,                                  // wLockDelay
  };
//}}}
//{{{  device qualifier descriptor
__ALIGN_BEGIN static const uint8_t kDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC,     // bLength
  USB_DESC_TYPE_DEVICE_QUALIFIER, // bDescriptorType */
  0x00, 0x02,                     // bcdUSb
  0x00,                           // bDeviceClass
  0x00,                           // bDeviceSubClass
  0x00,                           // bDeviceProtocol
  0x40,                           // bMaxPacketSize0
  0x01,                           // bNumConfigurations
  0x00,                           // bReserved
  };
//}}}
//{{{  language id descriptor
__ALIGN_BEGIN static const uint8_t kLangIdDescriptor[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING), HIBYTE(USBD_LANGID_STRING),
  };
//}}}
//{{{  serial string descriptor
__ALIGN_BEGIN static uint8_t kStringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
__ALIGN_BEGIN static uint8_t strDesc[256] __ALIGN_END;

//{{{  audioDescriptor
//{{{
static void intToUnicode (uint32_t value, uint8_t* pbuf, uint8_t len) {

  uint8_t idx = 0;
  for (idx = 0; idx < len; idx ++) {
    if (((value >> 28)) < 0xA )
      pbuf[2 * idx] = (value >> 28) + '0';
    else
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;

    value = value << 4;
    pbuf[2 * idx + 1] = 0;
    }
  }
//}}}
//{{{
static void getSerialNum() {

  #define DEVICE_ID1 (0x1FFF7A10)
  #define DEVICE_ID2 (0x1FFF7A14)
  #define DEVICE_ID3 (0x1FFF7A18)

  uint32_t deviceserial0, deviceserial1, deviceserial2;
  deviceserial0 = *(uint32_t*)DEVICE_ID1;
  deviceserial1 = *(uint32_t*)DEVICE_ID2;
  deviceserial2 = *(uint32_t*)DEVICE_ID3;
  deviceserial0 += deviceserial2;
  if (deviceserial0 != 0) {
    intToUnicode (deviceserial0, &kStringSerial[2] ,8);
    intToUnicode (deviceserial1, &kStringSerial[18] ,4);
    }
  }
//}}}

//{{{
static uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDescriptor);
  return (uint8_t*)kDeviceDescriptor;
  }
//}}}
//{{{
static uint8_t* langIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIdDescriptor);
  return (uint8_t*)kLangIdDescriptor;
  }
//}}}
//{{{
static uint8_t* manufacturerStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  #define USBD_MANUFACTURER_STRING  "colin"
  USBD_GetString ((uint8_t *)USBD_MANUFACTURER_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* productStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)((speed == USBD_SPEED_HIGH) ? "HS stm32 audio" : "FS stm32 audio"), strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* serialStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)kStringSerial;
  }
//}}}
//{{{
static uint8_t* configStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  #define USBD_CONFIGURATION_STRING "AUDIO Config"
  USBD_GetString ((uint8_t*)USBD_CONFIGURATION_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* interfaceStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {

  #define USBD_INTERFACE_STRING     "AUDIO Interface"
  USBD_GetString ((uint8_t*)USBD_INTERFACE_STRING, strDesc, length);
  return strDesc;
  }
//}}}

static USBD_DescriptorsTypeDef audioDescriptor = {
  deviceDescriptor,
  langIDStrDescriptor,
  manufacturerStrDescriptor,
  productStrDescriptor,
  serialStrDescriptor,
  configStrDescriptor,
  interfaceStrDescriptor,
  };
//}}}
//{{{  tAudioData
typedef struct {
  uint8_t       mBuffer[AUDIO_SLOTS_PACKET_BUF_SIZE];
  uint8_t       mPlayStarted;
  uint16_t      mWritePtr;
  __IO uint32_t mAltSetting;

  uint8_t       mCommand;
  uint8_t       mData[USB_MAX_EP0_SIZE];
  uint8_t       mLength;
  uint8_t       mUnit;
  } tAudioData;
//}}}
//{{{  audioClass
//{{{
static uint8_t usbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Open EP OUT
  USBD_LL_OpenEP (device, AUDIO_OUT_EP, USBD_EP_TYPE_ISOC, AUDIO_PACKET_SIZE);

  // allocate audioData
  tAudioData* audioData = malloc (sizeof (tAudioData));
  audioData->mPlayStarted = 0;
  audioData->mWritePtr = 0;
  audioData->mAltSetting = 0;
  device->pClassData = audioData;

  BSP_AUDIO_OUT_Init (OUTPUT_DEVICE_BOTH, AUDIO_DEFAULT_VOLUME, AUDIO_SAMPLE_RATE);
  BSP_AUDIO_OUT_SetAudioFrameSlot (SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3);

  // Prepare Out endpoint to receive 1st packet
  USBD_LL_PrepareReceive (device, AUDIO_OUT_EP, audioData->mBuffer, AUDIO_PACKET_SIZE);

  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // close out ep
  USBD_LL_CloseEP (device, AUDIO_OUT_EP);

  // stop audio
  BSP_AUDIO_OUT_Stop (CODEC_PDWN_SW);
  free (device->pClassData);
  device->pClassData = NULL;

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  sprintf (str, "%d setup %02x %02x %d %d %d",
           debugLine, req->bmRequest, req->bRequest, req->wValue, req->wLength, req->wIndex);
  debug (LCD_COLOR_WHITE);

  tAudioData* audioData = (tAudioData*)device->pClassData;
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
          if ((req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE) {
            USBD_CtlSendData (device, (uint8_t*)kConfigDescriptor + 18, MIN(9, req->wLength));
            sprintf (str, "%d - USB_REQ_GET_DESCRIPTOR - AUDIO tx %d ",
                     debugLine, MIN(9, req->wLength));
            debug (LCD_COLOR_WHITE);
            }
          else {
            sprintf (str, "- USB_REQ_GET_DESCRIPTOR");
            debug (LCD_COLOR_WHITE);
            }
          break;

        case USB_REQ_GET_INTERFACE :
          USBD_CtlSendData (device, (uint8_t*)&audioData->mAltSetting, 1);
          sprintf (str, "%d - USB_REQ_GET_INTERFACE - tx 1", debugLine);
          debug (LCD_COLOR_WHITE);
          break;

        case USB_REQ_SET_INTERFACE :
          sprintf (str, "%d - USB_REQ_SET_INTERFACE", debugLine);
          debug (LCD_COLOR_WHITE);

          if ((uint8_t)(req->wValue) <= USBD_MAX_NUM_INTERFACES)
            audioData->mAltSetting = (uint8_t)req->wValue;
          else // NAK
            USBD_CtlError (device, req);
          break;

        default:
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
      break;

    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        case AUDIO_REQ_GET_CUR:
          memset (audioData->mData, 0, 64);
          USBD_CtlSendData (device, audioData->mData, req->wLength);
          sprintf (str, "%d - AUDIO_REQ_GET_CUR - tx% d", debugLine, req->wLength);
          debug (LCD_COLOR_WHITE);
          break;

        case AUDIO_REQ_SET_CUR:
          sprintf (str, "%d - AUDIO_REQ_SET_CUR - rx %d", debugLine, req->wLength);
          debug (LCD_COLOR_WHITE);
          if (req->wLength) {
            // prepare to rx buffer from ep0
            USBD_CtlPrepareRx (device, audioData->mData, req->wLength);
            audioData->mCommand = AUDIO_REQ_SET_CUR; // set request value
            audioData->mLength = req->wLength;       // set request data length
            audioData->mUnit = HIBYTE(req->wIndex);  // set request target unit
            }
          break;

        default:
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
    }
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbEp0TxReady (USBD_HandleTypeDef* device) {

  //sprintf (str, "usbEp0TxReady"); debug (LCD_COLOR_RED);
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbEp0RxReady (USBD_HandleTypeDef* device) {
// only SET_CUR request is managed

  tAudioData* audioData = (tAudioData*)device->pClassData;

  sprintf (str, "%d epRxReady %d %d %d", debugLine, audioData->mCommand, audioData->mUnit, audioData->mLength);
  debug (LCD_COLOR_YELLOW);

  if (audioData->mCommand == AUDIO_REQ_SET_CUR) {
    if (audioData->mUnit == AUDIO_OUT_STREAMING_CTRL) {
      BSP_AUDIO_OUT_SetMute (audioData->mData[0]);
      audioData->mCommand = 0;
      audioData->mLength = 0;
      }
    }

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbDataIn (USBD_HandleTypeDef* device, uint8_t epNum) {

  sprintf (str, "%d usbDataIn", debugLine);
  debug (LCD_COLOR_RED);
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbDataOut (USBD_HandleTypeDef* device, uint8_t epNum) {

  if (epNum == AUDIO_OUT_EP) {
    tAudioData* audioData = (tAudioData*)device->pClassData;
    if (!audioData->mPlayStarted)
      if (audioData->mWritePtr >= AUDIO_SLOTS_PACKET_BUF_SIZE / 2) {
        BSP_AUDIO_OUT_Play ((uint16_t*)audioData->mBuffer, AUDIO_SLOTS_PACKET_BUF_SIZE);
        audioData->mPlayStarted = 1;
        }

    if (AUDIO_CHANNELS == 2) {
      // twiddle samples into slots
      uint16_t* srcPtr = (uint16_t*)(audioData->mBuffer + audioData->mWritePtr + 4*AUDIO_PACKET_SAMPLES);
      uint16_t* dstPtr = (uint16_t*)(audioData->mBuffer + audioData->mWritePtr + 8*AUDIO_PACKET_SAMPLES);
      for (int sample = 0; sample < AUDIO_PACKET_SAMPLES*2; sample++) {
        *--dstPtr = *--srcPtr;
        *--dstPtr = *srcPtr;
        }
      }

    // prepare outEndpoint to rx next audio packet
    audioData->mWritePtr += AUDIO_SLOTS_PACKET_SIZE;
    if (audioData->mWritePtr >= AUDIO_SLOTS_PACKET_BUF_SIZE)
      audioData->mWritePtr = 0;
    USBD_LL_PrepareReceive (device, AUDIO_OUT_EP, &audioData->mBuffer[audioData->mWritePtr], AUDIO_PACKET_SIZE);
    }

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbSof (USBD_HandleTypeDef* device) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t usbIsoInInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbIsoOutInComplete (USBD_HandleTypeDef* device, uint8_t epNum) {
  return USBD_OK;
  }
//}}}

//{{{
static uint8_t* usbGetConfigDescriptor (uint16_t* length) {

  //sprintf (str, "%d usbGetConfigDescriptor", debugLine); debug (LCD_COLOR_CYAN);
  *length = sizeof (kConfigDescriptor);
  return (uint8_t*)kConfigDescriptor;
  }
//}}}
//{{{
static uint8_t* usbGetDeviceQualifierDescriptor (uint16_t *length) {

  //sprintf (str, "%d usbGetDeviceQualifierDescriptor", debugLine); debug (LCD_COLOR_WHITE);
  *length = sizeof (kDeviceQualifierDescriptor);
  return (uint8_t*)kDeviceQualifierDescriptor;
  }
//}}}

static USBD_ClassTypeDef audioClass = {
  usbInit,
  usbDeInit,
  usbSetup,
  usbEp0TxReady,
  usbEp0RxReady,
  usbDataIn,
  usbDataOut,
  usbSof,
  usbIsoInInComplete,
  usbIsoOutInComplete,
  usbGetConfigDescriptor,
  usbGetConfigDescriptor,
  usbGetConfigDescriptor,
  usbGetDeviceQualifierDescriptor,
  };
//}}}

// BSP_audio out
//{{{
static void audioClock (int faster) {
// Set the PLL configuration according to the audio frequency
// target = 48000*2*2 * 256 = 49.152Mhz

// slower
// - PLLI2S_VCO: VCO_344 N
// - I2S_CLK = PLLI2S_VCO / PLLI2SQ = 344/7 = 49.142 Mhz
// - I2S_CLK1 = I2S_CLK / PLLI2SDIVQ = 49.142/1 = 49.142 Mhz

// faster
// - PLLI2S_VCO: VCO_295 N
// - I2S_CLK = PLLI2S_VCO / PLLI2SQ = 295/6 = 49.166 Mhz
// - I2S_CLK1 = I2S_CLK / PLLI2SDIVQ = 49.1666/1 = 49.142 Mhz

  if (faster != oldFaster) {
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig (&RCC_ExCLKInitStruct);
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = faster ? 295 : 344;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = faster ? 6 : 7;
    RCC_ExCLKInitStruct.PLLI2SDivQ = 1;
    HAL_RCCEx_PeriphCLKConfig (&RCC_ExCLKInitStruct);
    oldFaster = faster;
    }
  }
//}}}
//{{{
void BSP_AUDIO_OUT_ClockConfig (SAI_HandleTypeDef* hsai, uint32_t freq, void* Params) {
  audioClock (0);
  }
//}}}
//{{{
void BSP_AUDIO_OUT_TransferComplete_CallBack() {

  writePtrOnRead = ((tAudioData*)gUsbDevice.pClassData)->mWritePtr / AUDIO_SLOTS_PACKET_SIZE;
  if (writePtrOnRead > AUDIO_PACKETS/2) // faster
    audioClock (1);
  else if (writePtrOnRead < AUDIO_PACKETS/2) // slower
    audioClock (0);
  }
//}}}
//}}}

int main() {
  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  //{{{  configure system clock
  // System Clock source            = PLL (HSE)
  // SYSCLK(Hz)                     = 216000000
  // HCLK(Hz)                       = 216000000
  // AHB Prescaler                  = 1
  // APB1 Prescaler                 = 4
  // APB2 Prescaler                 = 2
  // HSE Frequency(Hz)              = 25000000
  // PLL_M                          = 25
  // PLL_N                          = 432
  // PLL_P                          = 2
  // PLLSAI_N                       = 384
  // PLLSAI_P                       = 8
  // VDD(V)                         = 3.3
  // Main regulator output voltage  = Scale1 mode
  // Flash Latency(WS)              = 7

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  // Enable HSE Oscillator and activate PLL with HSE as source
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
    while(1) {}

  // Activate the OverDrive to reach the 216 Mhz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    while(1) {}

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    while(1) {}
  //}}}

  BSP_LED_Init (LED1);
  initGraphics();

  BSP_TS_Init (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  // init usbDevice library
  gUsbDevice.pClassData = NULL;
  USBD_Init (&gUsbDevice, &audioDescriptor, 0);
  USBD_RegisterClass (&gUsbDevice, &audioClass);
  USBD_Start (&gUsbDevice);

  while (1) {
    touch();
    char str1[40];
    sprintf (str1, "%s %d %d %s %d", kVersion, AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, oldFaster ? "faster" : "slower", writePtrOnRead);
    BSP_LCD_SetTextColor (oldFaster ? LCD_COLOR_WHITE : LCD_COLOR_YELLOW);
    BSP_LCD_DisplayStringAtLine (0, (uint8_t*)str1);
    HAL_Delay (40);
    }
  }
