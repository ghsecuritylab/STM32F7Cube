// cTouch.h
#pragma once
//{{{  includes
#include "../../../cLcd.h"
#include "stm32746g_discovery_ts.h"
//}}}
//{{{
#ifdef __cplusplus
  extern "C" {
#endif
//}}}

class cTouch {
public:
  //{{{
  cTouch (int x, int y) {
    BSP_TS_Init (x, y);
    }
  //}}}
  //{{{
  void pollTouch() {

    BSP_TS_GetState (&mTsState);

    if (mTsState.touchDetected) {
      //lcd->debug (LCD_COLOR_YELLOW, "%d x:%d y:%d w:%d e:%d a:%d g:%d",
      //       mTsState.touchDetected, mTsState.touchX[0],mTsState.touchY[0], mTsState.touchWeight[0],
      //       mTsState.touchEventId[0], mTsState.touchArea[0], mTsState.gestureId);

      // pressed
      if (mTsState.touchDetected > 1) {
        mHit = eScroll;
        onScroll (mTsState.touchX[0] - mLastX, mTsState.touchY[0] - mLastY, mTsState.touchWeight[0]);
        }
      else if (mHit == ePressed)
        onMove (mTsState.touchX[0] - mLastX, mTsState.touchY[0] - mLastY, mTsState.touchWeight[0]);
      else if ((mHit == eReleased) && (mTsState.touchWeight[0] > 50)) {
        // press
        mHitX = mTsState.touchX[0];
        mHitY = mTsState.touchY[0];
        onPress (mHitX, mHitY);
        mHit = ePressed;
        }
      else if (mHit == eProx)
        onProx (mTsState.touchX[0] - mLastX, mTsState.touchY[0] - mLastY, mTsState.touchWeight[0]);
      else
        mHit = eProx;
      mLastX = mTsState.touchX[0];
      mLastY = mTsState.touchY[0];
      }
    else {
      // release
      if (mHit == ePressed)
        onRelease (mLastX, mLastY);
      mHit = eReleased;
      }
    }
  //}}}
  //{{{
  void show() {
    for (unsigned int i = 0u; i < mTsState.touchDetected; i++) {
      BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);
      BSP_LCD_FillCircle (mTsState.touchX[i], mTsState.touchY[i],
                          mTsState.touchWeight[i] ? mTsState.touchWeight[i] : 1);
      }
    }
  //}}}

protected:
  virtual void onProx (int x, int y, int z) {}
  virtual void onPress (int x, int y) {}
  virtual void onMove (int x, int y, int z) {}
  virtual void onScroll (int x, int y, int z) {}
  virtual void onRelease (int x, int y) {}

  enum eHit { eReleased, eProx, ePressed, eScroll };
  enum eHit mHit = eReleased;

  int mHitX = 0;
  int mHitY = 0;
  int mLastX = 0;
  int mLastY = 0;

  TS_StateTypeDef mTsState;
  };

//{{{
#ifdef __cplusplus
}
#endif
//}}}
