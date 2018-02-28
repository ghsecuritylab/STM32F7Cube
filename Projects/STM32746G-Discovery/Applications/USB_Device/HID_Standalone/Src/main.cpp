// main.cpp - hid class usb
//{{{  includes
#include <string>
#include "../../../system.h"
#include "../../../cLcd.h"
#include "../../../cTouch.h"
#include "../../../cPs2.h"
#include "../../../usbd.h"
//}}}
std::string kVersion = "USB HID keyboard ps2 28/2/18";
#define HID_IN_ENDPOINT       0x81
#define HID_IN_ENDPOINT_SIZE  4

//{{{
class cApp : public cTouch {
public:
  cApp (int x, int y) : cTouch (x,y) {}
  cLcd* getLcd() { return mLcd; }
  cPs2* getPs2() { return mPs2; }

  void run (bool keyboard);

protected:
  virtual void onProx (int x, int y, int z);
  virtual void onPress (int x, int y);
  virtual void onMove (int x, int y, int z);
  virtual void onScroll (int x, int y, int z);
  virtual void onRelease (int x, int y);

private:
  cLcd* mLcd = nullptr;
  cTouch* mTouch = nullptr;
  cPs2* mPs2 = nullptr;
  };
//}}}
cApp* gApp;
extern "C" { void EXTI9_5_IRQHandler() { gApp->getPs2()->onIrq(); } }

//{{{  device descriptors
#define STM_VID      0x0483
#define STM_HID_PID  0x5710

// device descriptor
__ALIGN_BEGIN const uint8_t kDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
  sizeof(kDeviceDescriptor), USB_DESC_TYPE_DEVICE,
  0,2,                       // bcdUSB
  0,                         // bDeviceClass
  0,                         // bDeviceSubClass
  0,                         // bDeviceProtocol
  USB_MAX_EP0_SIZE,          // bMaxPacketSize
  LOBYTE(STM_VID), HIBYTE(STM_VID),
  LOBYTE(STM_HID_PID), HIBYTE(STM_HID_PID),
  0,2,                       // bcdDevice rel. 2.00
  USBD_IDX_MFC_STR,          // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,      // Index of product string
  USBD_IDX_SERIAL_STR,       // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION // bNumConfigurations
  };

uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDescriptor);
  return (uint8_t*)kDeviceDescriptor;
  }

// device qualifier descriptor
__ALIGN_BEGIN const uint8_t kHidDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  sizeof(kHidDeviceQualifierDescriptor), USB_DESC_TYPE_DEVICE_QUALIFIER,
  0,2,   // bcdUSb
  0,     // bDeviceClass
  0,     // bDeviceSubClass
  0,     // bDeviceProtocol
  0x40,  // bMaxPacketSize0
  1,     // bNumConfigurations
  0,     // bReserved
  };

//}}}
//{{{  keyboard hid report descriptor
__ALIGN_BEGIN uint8_t kHidReportDescriptor[] __ALIGN_END = {
  0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,  // Usage (Keyboard)
  0xA1, 0x01,  // Collection (Application)
    //0x85, 0x01,  //  Report ID (1)  - extra byte in report with id = 1
    0x05, 0x07,  //  Usage Page (Keyboard/Keypad)
    0x75, 0x01,  //  Report Size (1)
    0x95, 0x08,  //  Report Count (8)
    0x19, 0xE0,  //  Usage Minimum (0xE0)
    0x29, 0xE7,  //  Usage Maximum (0xE7)
    0x15, 0x00,  //  Logical Minimum (0)
    0x25, 0x01,  //  Logical Maximum (1)
    0x81, 0x02,  //  Input (Data, Var, Abs, No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,  //  Report Count
    0x75, 0x08,  //  Report Size (8)
    0x15, 0x00,  //  Logical Minimum (0)
    0x25, 0x64,  //  Logical Maximum (100)
    0x05, 0x07,  //  Usage Page (Keyboard/Keypad)
    0x19, 0x00,  //  Usage Minimum (0x00)
    0x29, 0x65,  //  Usage Maximum (0x65)
    0x81, 0x00,  //  Input (Data, Array, Abs, No Wrap,Linear,Preferred State,No Null Position)
  0xC0,        // End Collection
  };
//}}}
//{{{  mouse hid report descriptor
//__ALIGN_BEGIN const uint8_t kHidReportDescriptor[] __ALIGN_END = {
  //0x05, 0x01, // Usage Page (Generic Desktop),
  //0x09, 0x02, // Usage (Mouse),
  //0xA1, 0x01, // Collection (Application),
    //0x09, 0x01, // Usage (Pointer),
    //0xA1, 0x00, // Collection (Physical),
      //0x05, 0x09, // Usage Page (Buttons),
      //0x19, 0x01, // Usage Minimum (01),
      //0x29, 0x03, // Usage Maximum (03),

      //0x15, 0x00, // Logical Minimum (0),
      //0x25, 0x01, // Logical Maximum (1),
      //0x95, 0x03, // Report Count (3),
      //0x75, 0x01, // Report Size (1),

      //0x81, 0x02, // Input (Data, Variable, Absolute), ;3 button bits

      //0x95, 0x01, // Report Count (1),
      //0x75, 0x05, // Report Size (5),
      //0x81, 0x01, // Input (Constant), ;5 bit padding

      //0x05, 0x01, // Usage Page (Generic Desktop),
      //0x09, 0x30, // Usage (X),
      //0x09, 0x31, // Usage (Y),
      //0x09, 0x38,

      //0x15, 0x81, // Logical Minimum (-127),
      //0x25, 0x7F, // Logical Maximum (127),
      //0x75, 0x08, // Report Size (8),
      //0x95, 0x03, // Report Count (3),

      //0x81, 0x06, // Input (Data, Variable, Relative), ;2 position bytes (X & Y)
    //0xC0,       // end collection - Physical

    //0x09, 0x3c, // usage
    //0x05, 0xff,
    //0x09, 0x01,
    //0x15, 0x00,
    //0x25, 0x01,
    //0x75, 0x01,
    //0x95, 0x02,
    //0xb1, 0x22,
    //0x75, 0x06,
    //0x95, 0x01,
    //0xb1, 0x01,
  //0xC0        // end collection - Application
  //};
//}}}
//{{{  hid configuration descriptor
__ALIGN_BEGIN const uint8_t kHidConfigurationDescriptor[34] __ALIGN_END = {
  9, USB_DESC_TYPE_CONFIGURATION,
  sizeof(kHidConfigurationDescriptor),0,  // wTotalLength - bytes returned
  1,     // bNumInterfaces: 1 interface
  1,     // bConfigurationValue - configuration value
  0,     // iConfiguration - index of string descriptor describing the configuration
  0xE0,  // bmAttributes: bus powered and Support Remote Wake-up
  0x32,  // MaxPower 100 mA: this current is used for detecting Vbus

  // hid interface descriptor
  9, USB_DESC_TYPE_INTERFACE,
  0,     // bInterfaceNumber - number of Interface
  0,     // bAlternateSetting - alternate setting
  1,     // bNumEndpoints
  3,     // bInterfaceClass: HID
  1,     // bInterfaceSubClass -  no boot  - 0x01, // bInterfaceSubClass : 0 = no boot  1 = BOOT,
  1,     // nInterfaceProtocol - keyboard, - 0x02, // nInterfaceProtocol : 0 = none     1 = keyboard  2 = mouse
  0,     // iInterface - index of string descriptor

  // HID descriptor
  9, 0x21,
  0x11,1,  // bcdHID: HID Class Spec release number
  0,       // bCountryCode: Hardware target country
  1,       // bNumDescriptors: number HID class descriptors to follow
  0x22,    // bDescriptorType
  sizeof(kHidReportDescriptor),0, // wItemLength - total length of report descriptor

  // hid endpoint descriptor
  7, USB_DESC_TYPE_ENDPOINT,
  HID_IN_ENDPOINT,      // bEndpointAddress - endpoint address (IN)
  3,                    // bmAttributes - interrupt endpoint
  HID_IN_ENDPOINT_SIZE, // wMaxPacketSize
  0,
  10,                   // bInterval - polling interval (10 ms)
  };
//}}}
//{{{  string descriptors
#define LANGID_STRING            0x409
__ALIGN_BEGIN const uint8_t kLangIdDescriptor[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(LANGID_STRING), HIBYTE(LANGID_STRING),
  };

uint8_t* langIdStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIdDescriptor);
  return (uint8_t*)kLangIdDescriptor;
  }

__ALIGN_BEGIN uint8_t stringDescriptor[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

uint8_t* productStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"fs hid product", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* manufacturerStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"Colin", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* configurationStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"fs hid config", stringDescriptor, length);
  return stringDescriptor;
  }

uint8_t* interfaceStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)"fs hid interface", stringDescriptor, length);
  return stringDescriptor;
  }

#define USB_SIZ_STRING_SERIAL    0x1A
__ALIGN_BEGIN uint8_t stringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };

//{{{
void intToUnicode (uint32_t value , uint8_t* pbuf , uint8_t len) {
  uint8_t idx = 0;
  for( idx = 0 ; idx < len ; idx ++) {
    if( ((value >> 28)) < 0xA )
      pbuf[ 2* idx] = (value >> 28) + '0';
    else
      pbuf[2* idx] = (value >> 28) + 'A' - 10;
    value = value << 4;
    pbuf[ 2* idx + 1] = 0;
    }
  }
//}}}
//{{{
void getSerialNum() {
  #define DEVICE_ID1 (0x1FFF7A10)
  #define DEVICE_ID2 (0x1FFF7A14)
  #define DEVICE_ID3 (0x1FFF7A18)
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
uint8_t* serialStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)stringSerial;
  }
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
uint8_t usbInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Open EP IN
  USBD_LL_OpenEP (device, HID_IN_ENDPOINT, USBD_EP_TYPE_INTR, HID_IN_ENDPOINT_SIZE);
  device->pClassData = malloc (sizeof(tHidData));
  ((tHidData*)device->pClassData)->mState = HID_IDLE;
  return 0;
  }
//}}}
//{{{
uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Close HID EPs
  USBD_LL_CloseEP (device, HID_IN_ENDPOINT);
  free (device->pClassData);
  device->pClassData = NULL;
  return USBD_OK;
  }
//}}}
//{{{
uint8_t usbSetup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  auto hidData = (tHidData*)device->pClassData;
  gApp->getLcd()->debug (LCD_COLOR_YELLOW, "setup bmReq:%x req:%x v:%x l:%d",
                                            req->bmRequest, req->bRequest, req->wValue, req->wLength);

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR: {
          if (req->wValue >> 8 == 0x21) { // hidDescriptor
            gApp->getLcd()->debug (LCD_COLOR_RED, "-getDescriptor hid");
            USBD_CtlSendData (device, (uint8_t*)kHidReportDescriptor+18, 9);
            }
          else if (req->wValue >> 8 == 0x22) { // hidReportDescriptor
            gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getDescriptor report len:%d", req->wLength);
            USBD_CtlSendData (device, (uint8_t*)kHidReportDescriptor, sizeof(kHidReportDescriptor));
            }
          break;
          }
        case USB_REQ_GET_INTERFACE :
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getInterface");
          USBD_CtlSendData (device, (uint8_t*)&hidData->mAltSetting, 1);
          break;
        case USB_REQ_SET_INTERFACE :
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setInterface");
          hidData->mAltSetting = (uint8_t)(req->wValue);
          break;
        }
      break;

    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        case 0x02: // reqGetIdle
          USBD_CtlSendData (device, (uint8_t*)&hidData->mIdleState, 1);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getIdle %d", hidData->mIdleState);
          break;
        case 0x03: // reqGetProtocol
          USBD_CtlSendData (device, (uint8_t*)&hidData->mProtocol, 1);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-getProtocol %d", hidData->mProtocol);
          break;
        case 0x0A: // reqSetIdle
          hidData->mIdleState = (uint8_t)(req->wValue >> 8);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setIdle %d", req->wValue);
          break;
        case 0x0B: // reqSetProtocol
          hidData->mProtocol = (uint8_t)(req->wValue);
          gApp->getLcd()->debug (LCD_COLOR_GREEN, "-setProtocol %d", req->wValue);
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
uint8_t* usbGetConfigurationDescriptor (uint16_t* length) {
  *length = sizeof(kHidConfigurationDescriptor);
  return (uint8_t*)kHidConfigurationDescriptor;
  }
//}}}
//{{{
uint8_t usbDataIn (USBD_HandleTypeDef* device, uint8_t epnum) {
  // Ensure that the FIFO is empty before a new transfer, this condition could
  // be caused by  a new transfer before the end of the previous transfer
  ((tHidData*)device->pClassData)->mState = HID_IDLE;
  return USBD_OK;
  }
//}}}
//{{{
uint8_t* usbGetDeviceQualifierDescriptor (uint16_t* length) {
  *length = sizeof (kHidDeviceQualifierDescriptor);
  return (uint8_t*)kHidDeviceQualifierDescriptor;
  }
//}}}
//}}}
//{{{
void hidSendMouse (USBD_HandleTypeDef* device, uint8_t* report) {

  auto hidData = (tHidData*)device->pClassData;
  if (device->dev_state == USBD_STATE_CONFIGURED) {
    if (hidData->mState == HID_IDLE) {
      hidData->mState = HID_BUSY;
      USBD_LL_Transmit (device, HID_IN_ENDPOINT, report, HID_IN_ENDPOINT_SIZE);
      }
    }
  }
//}}}
//{{{
void hidSendKeyboard (uint8_t modifier, uint8_t code) {

  struct keyboardHID_t {
    //uint8_t id;
    uint8_t mModifiers;
    uint8_t mReserved;
    uint8_t mKey1;
    uint8_t mKey2;
    uint8_t mKey3;
    uint8_t mKey4;
    uint8_t mKey5;
    uint8_t mKey6;
    };
  struct keyboardHID_t keyboardHID;

  //keyboardHID.id = 1;
  keyboardHID.mModifiers = modifier;
  keyboardHID.mReserved = 0;
  keyboardHID.mKey1 = code;
  keyboardHID.mKey2 = 0;
  keyboardHID.mKey3 = 0;
  keyboardHID.mKey4 = 0;
  keyboardHID.mKey5 = 0;
  keyboardHID.mKey6 = 0;

  auto hidData = (tHidData*)gUsbDevice.pClassData;
  if (gUsbDevice.dev_state == USBD_STATE_CONFIGURED) {
    if (hidData->mState == HID_IDLE) {
      hidData->mState = HID_BUSY;
      USBD_LL_Transmit (&gUsbDevice, HID_IN_ENDPOINT, (uint8_t*)(&keyboardHID), HID_IN_ENDPOINT_SIZE);
      HAL_Delay (10);
      }
    else
      gApp->getLcd()->debug (LCD_COLOR_RED, "missed char %x", code);
    }
  else
    gApp->getLcd()->debug (LCD_COLOR_RED, "unconfigured");
  }
//}}}
//{{{
//int32_t hidGetPollingInterval (USBD_HandleTypeDef* device) {

  //if (device->dev_speed == USBD_SPEED_HIGH)
    //// high speed endpoints, values between 1..16 are allowed. Values correspond to interval/ of 2 ^ (bInterval-1).
    //return (uint32_t)(((1 << (7 - 1)))/8);
  //else
    //// low, full speed endpoints
    //return (uint32_t)10;
  //}
//}}}

//{{{
void cApp::run (bool keyboard) {

  // init lcd
  mLcd = new cLcd (12);
  mLcd->init();

  // init ps2 keyboard
  mPs2 = new cPs2 (mLcd);
  if (keyboard)
    mPs2->initKeyboard();
  else
    mPs2->initTouchpad();

  // init usb
  //{{{  hidClass
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
    usbGetConfigurationDescriptor,
    usbGetConfigurationDescriptor,
    usbGetConfigurationDescriptor,
    usbGetDeviceQualifierDescriptor,
    };
  //}}}
  //{{{  hidDescriptor
  USBD_DescriptorsTypeDef hidDescriptor = {
    deviceDescriptor,
    langIdStringDescriptor,
    manufacturerStringDescriptor,
    productStringDescriptor,
    serialStringDescriptor,
    configurationStringDescriptor,
    interfaceStringDescriptor,
    };
  //}}}
  USBD_Init (&gUsbDevice, &hidDescriptor, 0);
  USBD_RegisterClass (&gUsbDevice, &hidClass);
  USBD_Start (&gUsbDevice);

  while (true) {
    if (keyboard)
      pollTouch();
    else {
      int touch, x, y, z;
      mPs2->getTouch (touch, x, y, z);
      handleTouch (x || y, x, y, z);
      }

    mLcd->show (kVersion);
    if (keyboard)
      mPs2->showChars();
    else
      mPs2->showTouch();
    mPs2->showCodes();
    mPs2->showWave();
    mLcd->flip();

    if (keyboard) {
      while (mPs2->hasRawChar())
        mPs2->getRawChar();
      while (mPs2->hasChar()) {
        auto ch = mPs2->getChar();
        hidSendKeyboard (ch >> 8, ch & 0xFF);
        mLcd->debug (ch & 0x100 ? LCD_COLOR_GREEN : LCD_COLOR_YELLOW, "sendHid %02x:%02x", ch >> 8, ch & 0xFF);
        }
      }
    }
  }
//}}}
//{{{
void cApp::onProx (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,(uint8_t)x,(uint8_t)y,0 };
    // hidSendReport (&gUsbDevice, HID_Buffer);
    mLcd->debug (LCD_COLOR_MAGENTA, "onProx %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void cApp::onPress (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onPress %d %d", x, y);
  }
//}}}
//{{{
void cApp::onMove (int x, int y, int z) {

  if (x || y) {
    //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,(uint8_t)x,(uint8_t)y,0 };
    //hidSendReport (&gUsbDevice, HID_Buffer);
    mLcd->debug (LCD_COLOR_GREEN, "onMove %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void cApp::onScroll (int x, int y, int z) {
  mLcd->incScrollValue (y);
  }
//}}}
//{{{
void cApp::onRelease (int x, int y) {

  //uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,0,0,0 };
  //hidSendReport (&gUsbDevice, HID_Buffer);
  mLcd->debug (LCD_COLOR_GREEN, "onRelease %d %d", x, y);
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
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);

  gApp = new cApp (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  gApp->run (true);
  }
//}}}
