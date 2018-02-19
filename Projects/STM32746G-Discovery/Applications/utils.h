#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

int getScrollScale();
int getScrollLines();
void incScrollValue (int inc);

void initUtils();
void showLcd (char* title, int showTouch);
void flipLcd();

void debug (uint32_t colour, const char* format, ... );

void onProx (int x, int y, int z);
void onPress (int x, int y);
void onMove (int x, int y, int z);
void onScroll (int x, int y, int z);
void onRelease (int x, int y);

void touch();
