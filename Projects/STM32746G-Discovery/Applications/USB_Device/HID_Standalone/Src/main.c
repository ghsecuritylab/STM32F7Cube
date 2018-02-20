// main.c
char* kVersion = "USB HID 20/2/18";
//{{{  includes
#include "../../../utils.h"
#include "../../../usbd.h"
//}}}
#define HID_IN_ENDPOINT       0x81
#define HID_IN_ENDPOINT_SIZE  4

//{{{  hid configuration descriptor
#define VID                      0x0483
#define PID                      0x5710

// USB HID device Configuration Descriptor
__ALIGN_BEGIN static const uint8_t kHidConfigurationDescriptor[34] __ALIGN_END = {
  0x09, USB_DESC_TYPE_CONFIGURATION,
  34,00  // wTotalLength: Bytes returned
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
  0x01,  // bInterfaceSubClass : 1 = BOOT, 0 = no boot
  0x02,  // nInterfaceProtocol : 0 = none, 1 = keyboard, 2 = mouse
  0,     // iInterface: Index of string descriptor

  // Joystick Mouse HID Descriptor
  0x09, 0x21, // bDescriptorType: HID
  0x11,01  // bcdHID: HID Class Spec release number
  0x00,    // bCountryCode: Hardware target country
  0x01,    // bNumDescriptors: Number of HID class descriptors to follow
  0x22,    // bDescriptorType
  74,0,    // wItemLength: Total length of Report descriptor

  // Mouse endpoint Descriptor
  0x07, USB_DESC_TYPE_ENDPOINT,
  HID_IN_ENDPOINT,  // bEndpointAddress: Endpoint Address (IN)
  0x03,           // bmAttributes: Interrupt endpoint
  HID_IN_ENDPOINT_SIZE,  // wMaxPacketSize: 4 Byte max
  0x00,
  10,             // bInterval: Polling Interval (10 ms)
  };
//}}}
//{{{
// USB HID device Configuration Descriptor
__ALIGN_BEGIN static const uint8_t kHidDescriptor[9] __ALIGN_END = {
  0x09, 0x21,
  0x11,1, // bcdHID: HID Class Spec release number
  0x00,   // bCountryCode: Hardware target country
  0x01,   // bNumDescriptors: Number of HID class descriptors to follow
  0x22,   // bDescriptorType
  74,0,   // wItemLength: Total length of Report descriptor
  };
//}}}
//{{{
__ALIGN_BEGIN static const uint8_t kHidMouseReportDescriptor[74] __ALIGN_END = {
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
//{{{  device qualifier descriptor
__ALIGN_BEGIN static const uint8_t kHidDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC, USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00, 0x02,  // bcdUSb
  0x00,        // bDeviceClass
  0x00,        // bDeviceSubClass
  0x00,        // bDeviceProtocol
  0x40,        // bMaxPacketSize0
  0x01,        // bNumConfigurations
  0x00,        // bReserved
  };
//}}}
//{{{  device descriptor
__ALIGN_BEGIN static const uint8_t kDeviceDescriptor[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12, USB_DESC_TYPE_DEVICE,
  0,2,                       // bcdUSB
  0x00,                      // bDeviceClass
  0x00,                      // bDeviceSubClass
  0x00,                      // bDeviceProtocol
  USB_MAX_EP0_SIZE,          // bMaxPacketSize
  LOBYTE(VID), HIBYTE(VID),  // idVendor
  LOBYTE(PID), HIBYTE(PID),  // idVendor
  0, 2,                      // bcdDevice rel. 2.00
  USBD_IDX_MFC_STR,          // Index of manufacturer string
  USBD_IDX_PRODUCT_STR,      // Index of product string
  USBD_IDX_SERIAL_STR,       // Index of serial number string
  USBD_MAX_NUM_CONFIGURATION // bNumConfigurations
  };

static uint8_t* deviceDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kDeviceDescriptor);
  return (uint8_t*)kDeviceDescriptor;
  }
//}}}
//{{{  language id descriptor
#define LANGID_STRING            0x409
// USB Standard Device Descriptor
__ALIGN_BEGIN static const uint8_t kLangIdDescriptor[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(LANGID_STRING), HIBYTE(LANGID_STRING),
  };

static uint8_t* langIdStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = sizeof(kLangIdDescriptor);
  return (uint8_t*)kLangIdDescriptor;
  }
//}}}
//{{{  product string descriptor
#define PRODUCT_FS_STRING        "fs hid mouse"

static uint8_t* productStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)PRODUCT_FS_STRING, stringDescriptor, length);
  return stringDescriptor;
  }
//}}}
//{{{  manufacturer string descriptor
#define MANUFACTURER_STRING      "Colin"

static uint8_t* manufacturerStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)MANUFACTURER_STRING, stringDescriptor, length);
  return stringDescriptor;
  }
//}}}
//{{{  configuration string descriptor
#define CONFIGURATION_FS_STRING  "hid fs config"

static uint8_t* configurationStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)CONFIGURATION_FS_STRING, stringDescriptor, length);
  return stringDescriptor;
  }
//}}}
//{{{  interface string descriptor
#define INTERFACE_FS_STRING      "hid fs interface"

static uint8_t* interfaceStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  USBD_GetString ((uint8_t*)INTERFACE_FS_STRING, stringDescriptor, length);
  return stringDescriptor;
  }

//}}}
//{{{  serial string descriptor
//{{{
__ALIGN_BEGIN static uint8_t stringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END = {
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
  };
//}}}
__ALIGN_BEGIN uint8_t stringDescriptor[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

#define DEVICE_ID1               (0x1FFF7A10)
#define DEVICE_ID2               (0x1FFF7A14)
#define DEVICE_ID3               (0x1FFF7A18)
#define USB_SIZ_STRING_SERIAL    0x1A
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

static uint8_t* serialStringDescriptor (USBD_SpeedTypeDef speed, uint16_t* length) {
  *length = USB_SIZ_STRING_SERIAL;
  getSerialNum();
  return (uint8_t*)stringSerial;
  }
//}}}

//{{{  hidDescriptor handler
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
  USBD_LL_OpenEP (device, HID_IN_ENDPOINT, USBD_EP_TYPE_INTR, HID_IN_ENDPOINT_SIZE);
  device->pClassData = malloc (sizeof(tHidData));
  ((tHidData*)device->pClassData)->mState = HID_IDLE;
  return 0;
  }
//}}}
//{{{
static uint8_t usbDeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  // Close HID EPs
  USBD_LL_CloseEP (device, HID_IN_ENDPOINT);
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
            USBD_CtlSendData (device, (uint8_t*)kHidMouseReportDescriptor, 74);
            }
          else if (req->wValue >> 8 == 0x21) { // hidDescriptor
            debug (LCD_COLOR_GREEN, "- getDescriptor hid");
            USBD_CtlSendData (device, (uint8_t*)kHidDescriptor, 9);
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
static uint8_t* usbGetConfigurationDescriptor (uint16_t* length) {
  *length = sizeof(kHidConfigurationDescriptor);
  return (uint8_t*)kHidConfigurationDescriptor;
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
static uint8_t* usbGetDeviceQualifierDescriptor (uint16_t* length) {
  *length = sizeof (kHidDeviceQualifierDescriptor);
  return (uint8_t*)kHidDeviceQualifierDescriptor;
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
  usbGetConfigurationDescriptor,
  usbGetConfigurationDescriptor,
  usbGetConfigurationDescriptor,
  usbGetDeviceQualifierDescriptor,
  };
//}}}
//{{{
static uint8_t hidSendReport (USBD_HandleTypeDef* device, uint8_t* report) {

  tHidData* hidData = (tHidData*)device->pClassData;
  if (device->dev_state == USBD_STATE_CONFIGURED) {
    if (hidData->mState == HID_IDLE) {
      hidData->mState = HID_BUSY;
      USBD_LL_Transmit (device, HID_IN_ENDPOINT, report, HID_IN_ENDPOINT_SIZE);
      }
    }

  return USBD_OK;
  }
//}}}
//{{{
//static uint32_t hidGetPollingInterval (USBD_HandleTypeDef* device) {

  //if (device->dev_speed == USBD_SPEED_HIGH)
    //// high speed endpoints, values between 1..16 are allowed. Values correspond to interval/ of 2 ^ (bInterval-1).
    //return (uint32_t)(((1 << (7 - 1)))/8);
  //else
    //// low, full speed endpoints
    //return (uint32_t)10;
  //}
//}}}

//{{{
void onProx (int x, int y, int z) {

  if (x || y) {
    uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,x,y,0 };
    hidSendReport (&gUsbDevice, HID_Buffer);
    debug (LCD_COLOR_MAGENTA, "onProx %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void onPress (int x, int y) {

  uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,0,0,0 };
  hidSendReport (&gUsbDevice, HID_Buffer);

  debug (LCD_COLOR_GREEN, "onPress %d %d", x, y);
  }
//}}}
//{{{
void onMove (int x, int y, int z) {

  if (x || y) {
    uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 1,x,y,0 };
    hidSendReport (&gUsbDevice, HID_Buffer);
    debug (LCD_COLOR_GREEN, "onMove %d %d %d", x, y, z);
    }
  }
//}}}
//{{{
void onScroll (int x, int y, int z) {
  incScrollValue (y);
  }
//}}}
//{{{
void onRelease (int x, int y) {

  uint8_t HID_Buffer[HID_IN_ENDPOINT_SIZE] = { 0,0,0,0 };
  hidSendReport (&gUsbDevice, HID_Buffer);

  debug (LCD_COLOR_GREEN, "onRelease %d %d", x, y);
  }
//}}}

//{{{
int main() {

  initUtils();

  USBD_Init (&gUsbDevice, &hidDescriptor, 0);
  USBD_RegisterClass (&gUsbDevice, &hidClass);
  USBD_Start (&gUsbDevice);

  while (1) {
    touch();
    showLcd (kVersion, 0);
    flipLcd();
    }
  }
//}}}
