// debug.c
//{{{  includes
#include "debug.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
//}}}

const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
uint32_t SystemCoreClock = 16000000;

//{{{  static vars
#define DEBUG_DISPLAY_LINES  16
#define DEBUG_MAX_LINES      200

static int gTick = 0;
static int gLayer = 1;

static uint16_t gDebugLine = 0;
static char* gDebugStr[DEBUG_MAX_LINES];
static uint32_t gDebugTicks[DEBUG_MAX_LINES];
static uint32_t gDebugColour[DEBUG_MAX_LINES];

static TS_StateTypeDef gTsState;

enum eHit { eReleased, eProx, ePressed, eScroll };
static enum eHit gHit = eReleased;
static int gHitX = 0;
static int gHitY = 0;
static int gLastX = 0;
static int gLastY = 0;
static int gScroll = 0;
//}}}
//{{{  common handlers
void NMI_Handler() {}
void SVC_Handler() {}
void PendSV_Handler() {}
void DebugMon_Handler() {}

void BusFault_Handler() { while (1) {} }
void HardFault_Handler() { while (1) {} }
void MemManage_Handler() { while (1) {} }
void UsageFault_Handler() { while (1) {} }

void SysTick_Handler() { HAL_IncTick(); }
//}}}

//{{{
int getScrollScale() {
  return BSP_LCD_GetFont()->Height / 3;
  }
//}}}
//{{{
int getScrollLines() {
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
void initLcd() {

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
void showLcd (char* title, int showTouch) {

  BSP_LCD_SelectLayer (gLayer);
  BSP_LCD_Clear (LCD_COLOR_BLACK);

  uint32_t wait = 20 - (HAL_GetTick() % 20);
  HAL_Delay (wait);
  gTick = HAL_GetTick();

  char str1[40];
  sprintf (str1, "%s %d %d", title, (int)gTick, (int)wait);
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

  if (showTouch)
    for (int i = 0; i < gTsState.touchDetected; i++) {
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

//{{{
void SystemInit() {

  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

  /* Reset the RCC clock configuration to the default reset state Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */
  }
//}}}
//{{{
/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         (*) HSI_VALUE is a constant defined in stm32f7xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *         (**) HSE_VALUE is a constant defined in stm32f7xx.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  */
void SystemCoreClockUpdate() {
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp) {
    case 0x00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock source */
      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P */
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

      if (pllsource != 0) {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = pllvco/pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}
//}}}
