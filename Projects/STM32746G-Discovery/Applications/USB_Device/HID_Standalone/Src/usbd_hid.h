#pragma once
//{{{
#ifdef __cplusplus
 extern "C" {
#endif
//}}}
#include  "usbd_ioreq.h"

#define HID_EPIN_ADDR                0x81
#define HID_EPIN_SIZE                0x04

#define USB_HID_CONFIG_DESC_SIZ      34
#define USB_HID_DESC_SIZ             9
#define HID_MOUSE_REPORT_DESC_SIZE   74

#define HID_DESCRIPTOR_TYPE          0x21
#define HID_REPORT_DESC              0x22
#define HID_HS_BINTERVAL             0x07
#define HID_FS_BINTERVAL             0x0A
#define HID_POLLING_INTERVAL         0x0A
#define HID_REQ_SET_PROTOCOL         0x0B
#define HID_REQ_GET_PROTOCOL         0x03
#define HID_REQ_SET_IDLE             0x0A
#define HID_REQ_GET_IDLE             0x02
#define HID_REQ_SET_REPORT           0x09
#define HID_REQ_GET_REPORT           0x01

typedef enum { HID_IDLE = 0, HID_BUSY, } HID_StateTypeDef;


typedef struct {
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  HID_StateTypeDef     state;
  } USBD_HID_HandleTypeDef;

extern USBD_ClassTypeDef  USBD_HID;
#define USBD_HID_CLASS    &USBD_HID

uint8_t USBD_HID_SendReport (USBD_HandleTypeDef* pdev, uint8_t* report, uint16_t len);
uint32_t USBD_HID_GetPollingInterval (USBD_HandleTypeDef* pdev);

//{{{
#ifdef __cplusplus
}
#endif
//}}}
