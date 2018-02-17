// This driver implements the following aspects of the specification:
//             - The Boot Interface Subclass
//             - The Mouse protocol
//             - Usage Page : Generic Desktop
//             - Usage : Joystick
//             - Collection : Application
#include "usbd_hid.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

//{{{
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END = {
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/

  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,

  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  } ;
//}}}
//{{{
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END = {
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  };
//}}}
//{{{
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
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
__ALIGN_BEGIN static const uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END = {
  0x05,   0x01,
  0x09,   0x02,
  0xA1,   0x01,
  0x09,   0x01,

  0xA1,   0x00,
  0x05,   0x09,
  0x19,   0x01,
  0x29,   0x03,

  0x15,   0x00,
  0x25,   0x01,
  0x95,   0x03,
  0x75,   0x01,

  0x81,   0x02,
  0x95,   0x01,
  0x75,   0x05,
  0x81,   0x01,

  0x05,   0x01,
  0x09,   0x30,
  0x09,   0x31,
  0x09,   0x38,

  0x15,   0x81,
  0x25,   0x7F,
  0x75,   0x08,
  0x95,   0x03,

  0x81,   0x06,
  0xC0,   0x09,
  0x3c,   0x05,
  0xff,   0x09,

  0x01,   0x15,
  0x00,   0x25,
  0x01,   0x75,
  0x01,   0x95,

  0x02,   0xb1,
  0x22,   0x75,
  0x06,   0x95,
  0x01,   0xb1,

  0x01,   0xc0
  };
//}}}

//{{{
static uint8_t USBD_HID_Init (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  uint8_t ret = 0;

  /* Open EP IN */
  USBD_LL_OpenEP(device, HID_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);

  device->pClassData = USBD_malloc(sizeof (USBD_HID_HandleTypeDef));

  if (device->pClassData == NULL)
    ret = 1;
  else
    ((USBD_HID_HandleTypeDef *)device->pClassData)->state = HID_IDLE;
  return ret;
  }
//}}}
//{{{
static uint8_t USBD_HID_DeInit (USBD_HandleTypeDef* device, uint8_t cfgidx) {

  /* Close HID EPs */
  USBD_LL_CloseEP(device, HID_EPIN_ADDR);

  /* FRee allocated memory */
  if(device->pClassData != NULL) {
    USBD_free(device->pClassData);
    device->pClassData = NULL;
    }

  return USBD_OK;
  }
//}}}
//{{{
static uint8_t USBD_HID_Setup (USBD_HandleTypeDef* device, USBD_SetupReqTypedef* req) {

  uint16_t len = 0;
  uint8_t* pbuf = NULL;
  USBD_HID_HandleTypeDef* hhid = (USBD_HID_HandleTypeDef*) device->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest) {
        case HID_REQ_SET_PROTOCOL: hhid->Protocol = (uint8_t)(req->wValue); break;
        case HID_REQ_GET_PROTOCOL: USBD_CtlSendData (device, (uint8_t *)&hhid->Protocol, 1); break;
        case HID_REQ_SET_IDLE: hhid->IdleState = (uint8_t)(req->wValue >> 8); break;
        case HID_REQ_GET_IDLE: USBD_CtlSendData (device, (uint8_t *)&hhid->IdleState, 1); break;
        default: USBD_CtlError (device, req); return USBD_FAIL;
        }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
          if( req->wValue >> 8 == HID_REPORT_DESC) {
            len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
            pbuf = (uint8_t*)HID_MOUSE_ReportDesc;
            }
          else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE) {
            pbuf = (uint8_t*)USBD_HID_Desc;
            len = MIN(USB_HID_DESC_SIZ , req->wLength);
            }
          USBD_CtlSendData (device, pbuf, len);
          break;
        case USB_REQ_GET_INTERFACE : USBD_CtlSendData (device, (uint8_t *)&hhid->AltSetting, 1); break;
        case USB_REQ_SET_INTERFACE : hhid->AltSetting = (uint8_t)(req->wValue); break;
        }
      }

  return USBD_OK;
  }
//}}}

//{{{
uint8_t USBD_HID_SendReport (USBD_HandleTypeDef* device, uint8_t* report, uint16_t len) {

  USBD_HID_HandleTypeDef* hhid = (USBD_HID_HandleTypeDef*)device->pClassData;
  if (device->dev_state == USBD_STATE_CONFIGURED ) {
    if(hhid->state == HID_IDLE) {
      hhid->state = HID_BUSY;
      USBD_LL_Transmit (device, HID_EPIN_ADDR, report, len);
    }
  }
  return USBD_OK;
}
//}}}
//{{{
uint32_t USBD_HID_GetPollingInterval (USBD_HandleTypeDef* device)
{
  uint32_t polling_interval = 0;

  /* HIGH-speed endpoints */
  if(device->dev_speed == USBD_SPEED_HIGH)
   /* Sets the data transfer polling interval for high speed transfers.
    Values between 1..16 are allowed. Values correspond to interval
    of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1 <<(HID_HS_BINTERVAL - 1)))/8);
  else   /* LOW and FULL-speed endpoints */
    /* Sets the data transfer polling interval for low and full
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;

  return ((uint32_t)(polling_interval));
  }
//}}}

//{{{
static uint8_t* USBD_HID_GetCfgDesc (uint16_t* length) {
  *length = sizeof (USBD_HID_CfgDesc);
  return (uint8_t*)USBD_HID_CfgDesc;
  }
//}}}
//{{{
static uint8_t USBD_HID_DataIn (USBD_HandleTypeDef* device, uint8_t epnum) {
  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_HID_HandleTypeDef *)device->pClassData)->state = HID_IDLE;
  return USBD_OK;
  }
//}}}
//{{{
static uint8_t* USBD_HID_GetDeviceQualifierDesc (uint16_t* length) {
  *length = sizeof (USBD_HID_DeviceQualifierDesc);
  return (uint8_t*)USBD_HID_DeviceQualifierDesc;
  }
//}}}

USBD_ClassTypeDef USBD_HID = {
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  NULL, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_HID_GetCfgDesc,
  USBD_HID_GetCfgDesc,
  USBD_HID_GetCfgDesc,
  USBD_HID_GetDeviceQualifierDesc,
  };
