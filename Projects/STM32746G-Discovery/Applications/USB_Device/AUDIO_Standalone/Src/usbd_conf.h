#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USBD_MAX_NUM_INTERFACES    1
#define USBD_MAX_NUM_CONFIGURATION 1

#if (USBD_DEBUG_LEVEL > 0)
  #define  USBD_UsrLog(...) printf(__VA_ARGS__); printf("\n");
#else
 #define USBD_UsrLog(...)
# endif

#if (USBD_DEBUG_LEVEL > 1)
  #define  USBD_ErrLog(...) printf("ERROR: "); printf(__VA_ARGS__); printf("\n");
#else
  #define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
  #define  USBD_DbgLog(...) printf("DEBUG : "); printf(__VA_ARGS__); printf("\n");
#else
  #define USBD_DbgLog(...)
#endif
