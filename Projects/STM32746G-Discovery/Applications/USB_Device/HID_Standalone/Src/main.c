// main.c
char* kVersion = "USB HID 16/2/18";
//{{{  includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f7xx_hal.h"

#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_ioreq.h"

#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
//}}}

//{{{  global debug vars
#define DEBUG_DISPLAY_LINES 16
#define DEBUG_MAX_LINES     1000

static int gTick = 0;
static int gLayer = 1;

static uint16_t gDebugLine = 0;
static char* gDebugStr[DEBUG_MAX_LINES];
static uint32_t gDebugTicks[DEBUG_MAX_LINES];
static uint32_t gDebugColour[DEBUG_MAX_LINES];

static int gHit = 0;
static int gHitX = 0;
static int gHitY = 0;
static int gLastX = 0;
static int gLastY = 0;

static int gScroll = 0;
//}}}
static TS_StateTypeDef gTS_State;

//{{{
static int getScrollScale() {
  return BSP_LCD_GetFont()->Height / 3;
  }
//}}}
//{{{
static int getScrollLines() {
  return gScroll / getScrollScale();
  }
//}}}
//{{{
static void setScrollValue (int scroll) {

  gScroll = scroll;

  if (scroll < 0)
    gScroll = 0;
  else if (getScrollLines() >  gDebugLine - DEBUG_DISPLAY_LINES)
    gScroll = (gDebugLine - DEBUG_DISPLAY_LINES) * getScrollScale();
  }
//}}}
//{{{
static void initLcd() {

  BSP_LCD_Init();

  BSP_LCD_LayerDefaultInit (0, LCD_FB_START_ADDRESS);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_SetBackColor (LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_Clear (LCD_COLOR_BLACK);
  BSP_LCD_SetFont (&Font16);
  BSP_LCD_SetLayerVisible (0, ENABLE);
  BSP_LCD_SetTransparency (0, 255);

  BSP_LCD_LayerDefaultInit (1, LCD_FB_START_ADDRESS + (BSP_LCD_GetXSize() * BSP_LCD_GetYSize() * 4));
  BSP_LCD_SelectLayer(1);
  BSP_LCD_SetBackColor (LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_Clear (LCD_COLOR_BLACK);
  BSP_LCD_SetFont (&Font16);
  BSP_LCD_SetLayerVisible (1, ENABLE);
  BSP_LCD_SetTransparency (1, 0);

  BSP_LCD_DisplayOn();

  for (int i = 0; i < DEBUG_MAX_LINES; i++)
    gDebugStr[i] = NULL;
  }
//}}}
//{{{
static void showLcd() {

  BSP_LCD_SelectLayer (gLayer);
  BSP_LCD_Clear (LCD_COLOR_BLACK);

  uint32_t wait = 20 - (HAL_GetTick() % 20);
  HAL_Delay (wait);
  gTick = HAL_GetTick();

  char str1[40];
  sprintf (str1, "%s %d %d", kVersion, (int)gTick, (int)wait);
  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_DisplayStringAtLine (0, str1);

  for (int displayLine = 0; (displayLine < gDebugLine) && (displayLine < DEBUG_DISPLAY_LINES); displayLine++) {
    int debugLine = (gDebugLine <= DEBUG_DISPLAY_LINES) ?
                      displayLine : gDebugLine - DEBUG_DISPLAY_LINES + displayLine - getScrollLines();
    debugLine = debugLine % DEBUG_MAX_LINES;
    BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
    char tickStr[20];
    sprintf (tickStr, "%2d.%03d", (int)gDebugTicks[debugLine] / 1000, (int)gDebugTicks[debugLine] % 1000);
    BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 0, tickStr);
    BSP_LCD_SetTextColor (gDebugColour[debugLine]);
    BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 7, gDebugStr[debugLine]);
    }

  for (int i = 0; i < gTS_State.touchDetected; i++) {
    BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);
    BSP_LCD_FillCircle (gTS_State.touchX[i], gTS_State.touchY[i], 
                        gTS_State.touchWeight[i] ? gTS_State.touchWeight[i] : 1);
    }

  BSP_LCD_SetTransparency (gLayer, 255);
  gLayer = !gLayer;
  BSP_LCD_SetTransparency (gLayer, 0);
  }
//}}}
//{{{
static void debug (uint32_t colour, const char* format, ... ) {

  gDebugColour[gDebugLine] = colour;
  gDebugTicks[gDebugLine] = HAL_GetTick();

  va_list args;
  va_start (args, format);
  free (gDebugStr[gDebugLine]);
  gDebugStr[gDebugLine] = (char*)malloc (vsnprintf (NULL, 0, format, args) + 1);
  vsnprintf (gDebugStr[gDebugLine], 40, format, args);
  va_end (args);

  gDebugLine = (gDebugLine+1) % DEBUG_MAX_LINES;
  }
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

void OTG_FS_IRQHandler() { HAL_PCD_IRQHandler (&gPcdHandle); }
//}}}
//{{{  usbd pcd handler
//{{{
void HAL_PCD_MspInit (PCD_HandleTypeDef* pcdHandle) {

  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure DM DP Pins */
  GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  /* Enable USB FS Clock */
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

  /* Set USBFS Interrupt priority */
  HAL_NVIC_SetPriority (OTG_FS_IRQn, 5, 0);

  /* Enable USBFS Interrupt */
  HAL_NVIC_EnableIRQ (OTG_FS_IRQn);
  }
//}}}
//{{{
void HAL_PCD_MspDeInit (PCD_HandleTypeDef* pcdHandle) {

  /* Disable USB FS Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
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
void HAL_PCD_DataInStageCallback( PCD_HandleTypeDef* pcdHandle, uint8_t epnum) {
  USBD_LL_DataInStage (pcdHandle->pData, epnum, pcdHandle->IN_ep[epnum].xfer_buff);
  }
//}}}

//{{{
void HAL_PCD_SOFCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_SOF (pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_ResetCallback (PCD_HandleTypeDef* pcdHandle) {

  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  /* Set USB Current Speed */
  switch (pcdHandle->Init.speed) {
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
void HAL_PCD_SuspendCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_Suspend(pcdHandle->pData);
  __HAL_PCD_GATE_PHYCLOCK (pcdHandle);
  }
//}}}
//{{{
void HAL_PCD_ResumeCallback (PCD_HandleTypeDef* pcdHandle) {
  __HAL_PCD_UNGATE_PHYCLOCK(pcdHandle);
  USBD_LL_Resume(pcdHandle->pData);
  }
//}}}

//{{{
void HAL_PCD_ConnectCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_DevConnected(pcdHandle->pData);
  }
//}}}
//{{{
void HAL_PCD_DisconnectCallback (PCD_HandleTypeDef* pcdHandle) {
  USBD_LL_DevDisconnected (pcdHandle->pData);
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
USBD_StatusTypeDef USBD_LL_Init (USBD_HandleTypeDef* device) {

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
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 0, 0x40);
  HAL_PCDEx_SetTxFiFo (&gPcdHandle, 1, 0x80);

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
  PCD_HandleTypeDef* pcdHandle = device->pData;
  if ((ep_addr & 0x80) == 0x80)
    return pcdHandle->IN_ep[ep_addr & 0x7F].is_stall;
  else
    return pcdHandle->OUT_ep[ep_addr & 0x7F].is_stall;
  }
//}}}

//{{{
USBD_StatusTypeDef USBD_LL_SetUSBAddress (USBD_HandleTypeDef* device, uint8_t device_addr) {
  HAL_PCD_SetAddress (device->pData, device_addr);
  return USBD_OK;
  }
//}}}
//{{{
USBD_StatusTypeDef USBD_LL_Transmit (USBD_HandleTypeDef* device, uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
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

//{{{  hidDescriptor handler
//{{{  defines
#define VID                      0x0483
#define PID                      0x5710
#define LANGID_STRING            0x409
#define MANUFACTURER_STRING      "Colin"
#define PRODUCT_FS_STRING        "fs hid mouse"
#define INTERFACE_FS_STRING      "hid fs interface"
#define CONFIGURATION_FS_STRING  "hid fs config"
#define DEVICE_ID1               (0x1FFF7A10)
#define DEVICE_ID2               (0x1FFF7A14)
#define DEVICE_ID3               (0x1FFF7A18)
#define USB_SIZ_STRING_SERIAL    0x1A

#define HID_EPIN_ADDR  0x81
#define HID_EPIN_SIZE  0x04
//}}}

//{{{
__ALIGN_BEGIN static const uint8_t kDeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12, USB_DESC_TYPE_DEVICE,
  0x00,                       // bcdUSB
  0x02,
  0x00,                       // bDeviceClass
  0x00,                       // bDeviceSubClass
  0x00,                       // bDeviceProtocol
  USB_MAX_EP0_SIZE,           // bMaxPacketSize
  LOBYTE(VID), HIBYTE(VID), // idVendor
  LOBYTE(PID), HIBYTE(PID), // idVendor
  0x00, 0x02,                 // bcdDevice rel. 2.00
  USBD_IDX_MFC_STR,           // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,       // Index of product string
  USBD_IDX_SERIAL_STR,        // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION  // bNumConfigurations
  };
//}}}
//{{{
// USB Standard Device Descriptor
__ALIGN_BEGIN static const uint8_t kLangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(LANGID_STRING), HIBYTE(LANGID_STRING),
  };
//}}}
//{{{
__ALIGN_BEGIN static uint8_t stringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
__ALIGN_BEGIN uint8_t strDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
//{{{
// USB HID device Configuration Descriptor
__ALIGN_BEGIN static const uint8_t kHidCfgDesc[34] __ALIGN_END = {
  0x09, USB_DESC_TYPE_CONFIGURATION,
  34,    // wTotalLength: Bytes returned
  0x00,
  0x01,  // bNumInterfaces: 1 interface
  0x01,  // bConfigurationValue: Configuration value
  0x00,  // iConfiguration: Index of string descriptor describing the configuration
  0xE0,  // bmAttributes: bus powered and Support Remote Wake-up
  0x32,  // MaxPower 100 mA: this current is used for detecting Vbus

  // Joystick Mouse interface Descriptor
  0x09, USB_DESC_TYPE_INTERFACE,
  0x00,  // bInterfaceNumber: Number of Interface
  0x00,  // bAlternateSetting: Alternate setting
  0x01,  // bNumEndpoints
  0x03,  // bInterfaceClass: HID
  0x01,  // bInterfaceSubClass : 1=BOOT, 0=no boot
  0x02,  // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
  0,     // iInterface: Index of string descriptor

  // Joystick Mouse HID Descriptor
  0x09, 0x21, // bDescriptorType: HID
  0x11,  // bcdHID: HID Class Spec release number
  0x01,
  0x00,  // bCountryCode: Hardware target country
  0x01,  // bNumDescriptors: Number of HID class descriptors to follow
  0x22,  // bDescriptorType
  74,    // wItemLength: Total length of Report descriptor
  0x00,

  // Mouse endpoint Descriptor
  0x07, USB_DESC_TYPE_ENDPOINT,
  HID_EPIN_ADDR,  // bEndpointAddress: Endpoint Address (IN)
  0x03,           // bmAttributes: Interrupt endpoint
  HID_EPIN_SIZE,  // wMaxPacketSize: 4 Byte max
  0x00,
  10,             // bInterval: Polling Interval (10 ms)
  };
//}}}
//{{{
// USB HID device Configuration Descriptor
__ALIGN_BEGIN static const uint8_t kHidDesc[9] __ALIGN_END = {
  0x09, 0x21, // hid config desriptor
  0x11,1, // bcdHID: HID Class Spec release number
  0x00,   // bCountryCode: Hardware target country
  0x01,   // bNumDescriptors: Number of HID class descriptors to follow
  0x22,   // bDescriptorType
  74,0,   // wItemLength: Total length of Report descriptor
  };
//}}}
//{{{
__ALIGN_BEGIN static const uint8_t kHidMouseReportDesc[74] __ALIGN_END = {
  0x05, 0x01, // Usage Page (Generic Desktop),
  0x09, 0x02, // Usage (Mouse),
  0xA1, 0x01, // Collection (Application),
    0x09, 0x01, // Usage (Pointer),
    0xA1, 0x00, // Collection (Physical),
      0x05, 0x09, // Usage Page (Buttons),
      0x19, 0x01, // Usage Minimum (01),
      0x29, 0x03, // Usage Maximum (03),

      0x15, 0x00, // Logical Minimum (0),
      0x25, 0x01, // Logical Maximum (1),
      0x95, 0x03, // Report Count (3),
      0x75, 0x01, // Report Size (1),

      0x81, 0x02, // Input (Data, Variable, Absolute), ;3 button bits

      0x95, 0x01, // Report Count (1),
      0x75, 0x05, // Report Size (5),
      0x81, 0x01, // Input (Constant), ;5 bit padding

      0x05, 0x01, // Usage Page (Generic Desktop),
      0x09, 0x30, // Usage (X),
      0x09, 0x31, // Usage (Y),
      0x09, 0x38,

      0x15, 0x81, // Logical Minimum (-127),
      0x25, 0x7F, // Logical Maximum (127),
      0x75, 0x08, // Report Size (8),
      0x95, 0x03, // Report Count (3),

      0x81, 0x06, // Input (Data, Variable, Relative), ;2 position bytes (X & Y)
    0xC0,       // end collection - Physical

    0x09, 0x3c, // usage
    0x05, 0xff,
    0x09, 0x01,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x02,
    0xb1, 0x22,
    0x75, 0x06,
    0x95, 0x01,
    0xb1, 0x01,
  0xC0        // end collection - Application
  };
//}}}
//{{{
// USB Standard Device Descriptor
__ALIGN_BEGIN static const uint8_t kHidDeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC, USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
  };
//}}}

//{{{
static void intToUnicode (uint32_t value , uint8_t* pbuf , uint8_t len) {
  uint8_t idx = 0;

  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[ 2* idx + 1] = 0;
  }
}
//}}}
//{{{
static void getSerialNum() {

  uint32_t deviceserial0 = *(uint32_t*)DEVICE_ID1;
  uint32_t deviceserial1 = *(uint32_t*)DEVICE_ID2;
  uint32_t deviceserial2 = *(uint32_t*)DEVICE_ID3;
  deviceserial0 += deviceserial2;

  if (deviceserial0 != 0) {
    intToUnicode (deviceserial0, &stringSerial[2] ,8);
    intToUnicode (deviceserial1, &stringSerial[18] ,4);
    }
  }
//}}}

//{{{
static uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDesc);
  return (uint8_t*)kDeviceDesc;
  }
//}}}
//{{{
static uint8_t* langIDStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIDDesc);
  return (uint8_t*)kLangIDDesc;
  }
//}}}
//{{{
static uint8_t* productStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)PRODUCT_FS_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* manufacturerStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)MANUFACTURER_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* serialStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)stringSerial;
  }
//}}}
//{{{
static uint8_t* configStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)CONFIGURATION_FS_STRING, strDesc, length);
  return strDesc;
  }
//}}}
//{{{
static uint8_t* interfaceStrDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)INTERFACE_FS_STRING, strDesc, length);
  return strDesc;
  }

//}}}

USBD_DescriptorsTypeDef hidDescriptor = {
  deviceDescriptor,
  langIDStrDescriptor,
  manufacturerStrDescriptor,
  productStrDescriptor,
  serialStrDescriptor,
  configStrDescriptor,
  interfaceStrDescriptor,
  };
//}}}
//{{{  hidClass handlers
typedef enum { HID_IDLE = 0, HID_BUSY, } eHidStateTypeDef;
//{{{  tHidData
typedef struct {
  uint32_t         mProtocol;
  uint32_t         mIdleState;
  uint32_t         mAltSetting;
  eHidStateTypeDef mState;
  } tHidData;
//}}}

//{{{
static uint8_t usbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Open EP IN
  USBD_LL_OpenEP (device, HID_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);
  device->pClassData = malloc (sizeof(tHidData));
  ((tHidData*)device->pClassData)->mState = HID_IDLE;
  return 0;
  }
//}}}
//{{{
static uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Close HID EPs
  USBD_LL_CloseEP (device, HID_EPIN_ADDR);
  free(device->pClassData);
  device->pClassData = NULL;
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t usbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  tHidData* hidData = (tHidData*)device->pClassData;
  debug (LCD_COLOR_YELLOW, "setup bmReq:%x req:%x v:%x l:%d",
                           req->bmRequest, req->bRequest, req->wValue, req->wLength);

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR: {
          if (req->wValue >> 8 == 0x22) { // hidReportDescriptor
            debug (LCD_COLOR_GREEN, "- getDescriptor report len:%d", req->wLength);
            USBD_CtlSendData (device, (uint8_t*)kHidMouseReportDesc, 74);
            }
          else if (req->wValue >> 8 == 0x21) { // hidDescriptor
            debug (LCD_COLOR_GREEN, "- getDescriptor hid");
            USBD_CtlSendData (device, (uint8_t*)kHidDesc, 9);
            }
          break;
          }
        case USB_REQ_GET_INTERFACE :
          debug (LCD_COLOR_GREEN, "- getInterface");
          USBD_CtlSendData (device, (uint8_t*)&hidData->mAltSetting, 1);
          break;
        case USB_REQ_SET_INTERFACE :
          debug (LCD_COLOR_GREEN, "- setInterface");
          hidData->mAltSetting = (uint8_t)(req->wValue);
          break;
        }
      break;

    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        case 0x0B: // HID_REQ_SET_PROTOCOL:
          hidData->mProtocol = (uint8_t)(req->wValue);
          debug (LCD_COLOR_GREEN, "- setProtocol %d", req->wValue);
          break;
        case 0x03: // HID_REQ_GET_PROTOCOL:
          USBD_CtlSendData (device, (uint8_t*)&hidData->mProtocol, 1);
          debug (LCD_COLOR_GREEN, "- getProtocol %d", hidData->mProtocol);
          break;
        case 0x0A: // reqSetIdle
          hidData->mIdleState = (uint8_t)(req->wValue >> 8);
          debug (LCD_COLOR_GREEN, "- setIdle %d", req->wValue);
          break;
        case 0x02: // reqGetIdle
          USBD_CtlSendData (device, (uint8_t*)&hidData->mIdleState, 1);
          debug (LCD_COLOR_GREEN, "- getIdle %d", hidData->mIdleState);
          break;
        default:
          USBD_CtlError (device, req);
          return USBD_FAIL;
        }
      break;
      }

  return USBD_OK;
  }
//}}}

//{{{
static uint8_t* usbGetCfgDesc (uint16_t* length) {
  *length = sizeof(kHidCfgDesc);
  return (uint8_t*)kHidCfgDesc;
  }
//}}}
//{{{
static uint8_t usbDataIn (USBD_HandleTypeDef* device, uint8_t epnum) {
  // Ensure that the FIFO is empty before a new transfer, this condition could
  // be caused by  a new transfer before the end of the previous transfer
  ((tHidData *)device->pClassData)->mState = HID_IDLE;
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t* usbGetDeviceQualifierDesc (uint16_t* length) {
  *length = sizeof (kHidDeviceQualifierDesc);
  return (uint8_t*)kHidDeviceQualifierDesc;
  }
//}}}

USBD_ClassTypeDef hidClass = {
  usbInit,
  usbDeInit,
  usbSetup,
  NULL, //EP0_TxSent
  NULL, //EP0_RxReady
  usbDataIn,
  NULL, // DataOut
  NULL, // SOF
  NULL,
  NULL,
  usbGetCfgDesc,
  usbGetCfgDesc,
  usbGetCfgDesc,
  usbGetDeviceQualifierDesc,
  };
//}}}
//{{{
static uint32_t hidGetPollingInterval (USBD_HandleTypeDef* device) {

  if (device->dev_speed == USBD_SPEED_HIGH)
    // high speed endpoints, values between 1..16 are allowed. Values correspond to interval/ of 2 ^ (bInterval-1).
    return (uint32_t)(((1 << (7 - 1)))/8);
  else
    // low, full speed endpoints
    return (uint32_t)10;
  }
//}}}
//{{{
static uint8_t hidSendReport (USBD_HandleTypeDef* device, uint8_t* report, uint16_t len) {

  tHidData* hidData = (tHidData*)device->pClassData;
  if (device->dev_state == USBD_STATE_CONFIGURED) {
    if (hidData->mState == HID_IDLE) {
      hidData->mState = HID_BUSY;
      USBD_LL_Transmit (device, HID_EPIN_ADDR, report, len);
      }
    }

  return USBD_OK;
  }
//}}}

//{{{
static void onPress (int x, int y) {

  //uint8_t HID_Buffer[4] = { 1,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer, 4);
  }
//}}}
//{{{
static void onMove (int x, int y) {

  //uint8_t HID_Buffer[4] = { 1,x,y,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer, 4);

  //gScroll += y;
  //setScrollValue (gScroll + y);
  }
//}}}
//{{{
static void onRelease (int x, int y) {

  //uint8_t HID_Buffer[4] = { 0,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer, 4);
  }
//}}}
//{{{
static void touch() {

  BSP_TS_GetState (&gTS_State);
  if (gTS_State.touchDetected)
  debug (LCD_COLOR_YELLOW, "%d x:%d y:%d w:%d e:%d a:%d %d",
         gTS_State.touchDetected, gTS_State.touchX[0],gTS_State.touchY[0],
         gTS_State.touchWeight[0], gTS_State.touchEventId[0], gTS_State.touchArea[0],
         gTS_State.gestureId);

  if (gTS_State.touchDetected) {
    // pressed
    if (gHit) {
      int x = gTS_State.touchX[0];
      int y = gTS_State.touchY[0];
      onMove (x - gLastX, y - gLastY);
      gLastX = x;
      gLastY = y;
      }
    else {
      gHitX = gTS_State.touchX[0];
      gHitY = gTS_State.touchY[0];
      onPress (gHitX, gHitY);
      gLastX = gHitX;
      gLastY = gHitY;
      }
    gHit = 1;
    }
  else if (gHit) {
    // released
    onRelease (gLastX, gLastY);
    gHit = 0;
    }
  }
//}}}

//{{{
int main() {

  SCB_EnableICache();
  SCB_EnableDCache();
  HAL_Init();
  //{{{  config system clock
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

  // Enable HSE Oscillator and activate PLL with HSE as source
  RCC_OscInitTypeDef RCC_OscInitStruct;
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
    while (1) {}

  // Activate the OverDrive to reach the 216 Mhz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    while (1) {}

  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    while (1) {}
  //}}}

  BSP_LED_Init (LED1);

  initLcd();
  BSP_TS_Init (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);

  USBD_Init (&gUsbDevice, &hidDescriptor, 0);
  USBD_RegisterClass (&gUsbDevice, &hidClass);
  USBD_Start (&gUsbDevice);

  while (1) {
    touch();
    showLcd();
    HAL_Delay (20);
    }
  }
//}}}
