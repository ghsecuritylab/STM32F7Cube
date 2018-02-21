// utils.cpp
//{{{  includes
#include <vector>
#include "utils.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//}}}

// lcd
#define DEBUG_DISPLAY_LINES  16
#define DEBUG_MAX_LINES      200
int gTick = 0;
int gLayer = 1;

//{{{
class cDebugItem {
public:
  cDebugItem() {
    mStr = (char*)malloc (40);
    }
  ~cDebugItem() { free (mStr); }

  char* mStr = nullptr;
  int mStrSize = 40;
  uint32_t mTicks = 0;
  uint32_t mColour = 0;
  };
//}}}
cDebugItem gLines[DEBUG_MAX_LINES];
unsigned gDebugLine = 0;

std::vector<cDebugItem> mLines;
unsigned mMaxLine = 0;

// touch
TS_StateTypeDef gTsState;
enum eHit { eReleased, eProx, ePressed, eScroll };
enum eHit gHit = eReleased;
int gHitX = 0;
int gHitY = 0;
int gLastX = 0;
int gLastY = 0;
int gScroll = 0;

//{{{
int getScrollScale() {
  return BSP_LCD_GetFont()->Height / 3;
  }
//}}}
//{{{
unsigned getScrollLines() {
  return gScroll / getScrollScale();
  }
//}}}
//{{{
void incScrollValue (int inc) {

  gScroll += inc;

  if (gScroll < 0)
    gScroll = 0;
  else if (getScrollLines() >  gDebugLine - DEBUG_DISPLAY_LINES)
    gScroll = (gDebugLine - DEBUG_DISPLAY_LINES) * getScrollScale();
  }
//}}}

//{{{
void initUtils() {

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

  BSP_LCD_Init();
  BSP_TS_Init (BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_PB_Init (BUTTON_KEY, BUTTON_MODE_GPIO);
  BSP_LED_Init (LED1);

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
  }
//}}}
//{{{
void showLcd (const std::string& title, bool showTouch) {

  BSP_LCD_SelectLayer (gLayer);
  BSP_LCD_Clear (LCD_COLOR_BLACK);

  uint32_t wait = 20 - (HAL_GetTick() % 20);
  HAL_Delay (wait);
  gTick = HAL_GetTick();

  char str1[40];
  sprintf (str1, "%s %d %d", title.c_str(), (int)gTick, (int)wait);
  BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
  BSP_LCD_DisplayStringAtLine (0, str1);

  if (!BSP_PB_GetState (BUTTON_KEY))
    for (auto displayLine = 0u; (displayLine < gDebugLine) && (displayLine < DEBUG_DISPLAY_LINES); displayLine++) {
      int debugLine = (gDebugLine <= DEBUG_DISPLAY_LINES) ?
                        displayLine : gDebugLine - DEBUG_DISPLAY_LINES + displayLine - getScrollLines();
      debugLine = debugLine % DEBUG_MAX_LINES;
      BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
      char tickStr[20];
      sprintf (tickStr, "%2d.%03d", (int)gLines[debugLine].mTicks / 1000, (int)gLines[debugLine].mTicks % 1000);
      BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 0, tickStr);
      BSP_LCD_SetTextColor (gLines[debugLine].mColour);
      BSP_LCD_DisplayStringAtLineColumn (1+displayLine, 7, gLines[debugLine].mStr);
      }

  if (showTouch)
    for (unsigned int i = 0u; i < gTsState.touchDetected; i++) {
      BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);
      BSP_LCD_FillCircle (gTsState.touchX[i], gTsState.touchY[i],
                          gTsState.touchWeight[i] ? gTsState.touchWeight[i] : 1);
      }
  }
//}}}
//{{{
void flipLcd() {

  BSP_LCD_SetTransparency (gLayer, 255);
  gLayer = !gLayer;
  BSP_LCD_SetTransparency (gLayer, 0);
  }
//}}}

//{{{
void debug (uint32_t colour, const char* format, ... ) {

  va_list args;
  va_start (args, format);
  vsnprintf (gLines[gDebugLine].mStr, gLines[gDebugLine].mStrSize, format, args);
  va_end (args);

  gLines[gDebugLine].mTicks = HAL_GetTick();
  gLines[gDebugLine].mColour = colour;

  gDebugLine = (gDebugLine+1) % DEBUG_MAX_LINES;
  }
//}}}

//{{{
void touch() {

  BSP_TS_GetState (&gTsState);

  //if (gTsState.touchDetected)
  //  debug (LCD_COLOR_YELLOW, "%d x:%d y:%d w:%d e:%d a:%d g:%d",
  //         gTsState.touchDetected, gTsState.touchX[0],gTsState.touchY[0], gTsState.touchWeight[0],
  //         gTsState.touchEventId[0], gTsState.touchArea[0], gTsState.gestureId);

  if (gTsState.touchDetected) {
    // pressed
    if (gTsState.touchDetected > 1) {
      gHit = eScroll;
      onScroll (gTsState.touchX[0] - gLastX, gTsState.touchY[0] - gLastY, gTsState.touchWeight[0]);
      }
    else if (gHit == ePressed)
      onMove (gTsState.touchX[0] - gLastX, gTsState.touchY[0] - gLastY, gTsState.touchWeight[0]);
    else if ((gHit == eReleased) && (gTsState.touchWeight[0] > 50)) {
      // press
      gHitX = gTsState.touchX[0];
      gHitY = gTsState.touchY[0];
      onPress (gHitX, gHitY);
      gHit = ePressed;
      }
    else if (gHit == eProx)
      onProx (gTsState.touchX[0] - gLastX, gTsState.touchY[0] - gLastY, gTsState.touchWeight[0]);
    else
      gHit = eProx;
    gLastX = gTsState.touchX[0];
    gLastY = gTsState.touchY[0];
    }
  else {
    // release
    if (gHit == ePressed)
      onRelease (gLastX, gLastY);
    gHit = eReleased;
    }
  }
//}}}
