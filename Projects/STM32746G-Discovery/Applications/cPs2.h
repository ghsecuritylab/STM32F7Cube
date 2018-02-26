// cPs2.h
#pragma once
// keyboard brown  - touchpad red    - V - +5v
// keyboard yellow - touchpad yellow - C - A3 - PF8 - clock
// keyboard red    - touchpad green  - O - A2 - PF9 - data
// keyboard black  - touchpad black  - G - ground
//{{{  includes
#include "../../../system.h"
#include "../../../cLcd.h"
//}}}
//{{{  keycode, modifier defines
//{{{  modifier keys
#define KEY_MOD_LCTRL  0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT   0x04
#define KEY_MOD_LMETA  0x08
#define KEY_MOD_RCTRL  0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT   0x40
#define KEY_MOD_RMETA  0x80
//}}}
//{{{  keys
// Scan codes - last N slots in the HID report(usually 6) 0x00 if no key pressed.
//If more than N keys are pressed, the HID reports
// KEY_ERR_OVF in all slots to indicate this condition.

#define KEY_NONE 0x00 // No key pressed
#define KEY_ERR_OVF 0x01 //  Keyboard Error Roll Over - used for all slots if too many keys are pressed
// 0x02 //  Keyboard POST Fail
// 0x03 //  Keyboard Error Undefined
//}}}
//{{{  chars
#define KEY_A 0x04 // Keyboard a and A
#define KEY_B 0x05 // Keyboard b and B
#define KEY_C 0x06 // Keyboard c and C
#define KEY_D 0x07 // Keyboard d and D
#define KEY_E 0x08 // Keyboard e and E
#define KEY_F 0x09 // Keyboard f and F
#define KEY_G 0x0a // Keyboard g and G
#define KEY_H 0x0b // Keyboard h and H
#define KEY_I 0x0c // Keyboard i and I
#define KEY_J 0x0d // Keyboard j and J
#define KEY_K 0x0e // Keyboard k and K
#define KEY_L 0x0f // Keyboard l and L
#define KEY_M 0x10 // Keyboard m and M
#define KEY_N 0x11 // Keyboard n and N
#define KEY_O 0x12 // Keyboard o and O
#define KEY_P 0x13 // Keyboard p and P
#define KEY_Q 0x14 // Keyboard q and Q
#define KEY_R 0x15 // Keyboard r and R
#define KEY_S 0x16 // Keyboard s and S
#define KEY_T 0x17 // Keyboard t and T
#define KEY_U 0x18 // Keyboard u and U
#define KEY_V 0x19 // Keyboard v and V
#define KEY_W 0x1a // Keyboard w and W
#define KEY_X 0x1b // Keyboard x and X
#define KEY_Y 0x1c // Keyboard y and Y
#define KEY_Z 0x1d // Keyboard z and Z
//}}}
//{{{  number
#define KEY_1 0x1e // Keyboard 1 and !
#define KEY_2 0x1f // Keyboard 2 and @
#define KEY_3 0x20 // Keyboard 3 and #
#define KEY_4 0x21 // Keyboard 4 and $
#define KEY_5 0x22 // Keyboard 5 and %
#define KEY_6 0x23 // Keyboard 6 and ^
#define KEY_7 0x24 // Keyboard 7 and &
#define KEY_8 0x25 // Keyboard 8 and *
#define KEY_9 0x26 // Keyboard 9 and (
#define KEY_0 0x27 // Keyboard 0 and )
//}}}
//{{{  punctuation
#define KEY_ENTER      0x28 // Keyboard Return (ENTER)
#define KEY_ESC        0x29 // Keyboard ESCAPE
#define KEY_BACKSPACE  0x2a // Keyboard DELETE (Backspace)
#define KEY_TAB        0x2b // Keyboard Tab
#define KEY_SPACE      0x2c // Keyboard Spacebar
#define KEY_MINUS      0x2d // Keyboard - and _
#define KEY_EQUAL      0x2e // Keyboard = and +
#define KEY_LEFTBRACE  0x2f // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30 // Keyboard ] and }
#define KEY_BACKSLASH  0x31 // Keyboard \ and |
#define KEY_HASHTILDE  0x32 // Keyboard Non-US # and ~
#define KEY_SEMICOLON  0x33 // Keyboard ; and :
#define KEY_APOSTROPHE 0x34 // Keyboard ' and "
#define KEY_GRAVE      0x35 // Keyboard ` and ~
#define KEY_COMMA      0x36 // Keyboard , and <
#define KEY_DOT        0x37 // Keyboard . and >
#define KEY_SLASH      0x38 // Keyboard / and ?
#define KEY_CAPSLOCK   0x39 // Keyboard Caps Lock
//}}}
//{{{  F1-F12
#define KEY_F1         0x3a // Keyboard F1
#define KEY_F2         0x3b // Keyboard F2
#define KEY_F3         0x3c // Keyboard F3
#define KEY_F4         0x3d // Keyboard F4
#define KEY_F5         0x3e // Keyboard F5
#define KEY_F6         0x3f // Keyboard F6
#define KEY_F7         0x40 // Keyboard F7
#define KEY_F8         0x41 // Keyboard F8
#define KEY_F9         0x42 // Keyboard F9
#define KEY_F10        0x43 // Keyboard F10
#define KEY_F11        0x44 // Keyboard F11
#define KEY_F12        0x45 // Keyboard F12
//}}}
//{{{  mid arrows
#define KEY_SYSRQ      0x46 // Keyboard Print Screen
#define KEY_SCROLLLOCK 0x47 // Keyboard Scroll Lock
#define KEY_PAUSE      0x48 // Keyboard Pause
#define KEY_INSERT     0x49 // Keyboard Insert
#define KEY_HOME       0x4a // Keyboard Home
#define KEY_PAGEUP     0x4b // Keyboard Page Up
#define KEY_DELETE     0x4c // Keyboard Delete Forward
#define KEY_END        0x4d // Keyboard End
#define KEY_PAGEDOWN   0x4e // Keyboard Page Down
#define KEY_RIGHT      0x4f // Keyboard Right Arrow
#define KEY_LEFT       0x50 // Keyboard Left Arrow
#define KEY_DOWN       0x51 // Keyboard Down Arrow
#define KEY_UP         0x52 // Keyboard Up Arrow
//}}}
//{{{  keypad
#define KEY_NUMLOCK    0x53 // Keyboard Num Lock and Clear
#define KEY_KPSLASH    0x54 // Keypad /
#define KEY_KPASTERISK 0x55 // Keypad *
#define KEY_KPMINUS    0x56 // Keypad -
#define KEY_KPPLUS     0x57 // Keypad +
#define KEY_KPENTER    0x58 // Keypad ENTER
#define KEY_KP1        0x59 // Keypad 1 and End
#define KEY_KP2        0x5a // Keypad 2 and Down Arrow
#define KEY_KP3        0x5b // Keypad 3 and PageDn
#define KEY_KP4        0x5c // Keypad 4 and Left Arrow
#define KEY_KP5        0x5d // Keypad 5
#define KEY_KP6        0x5e // Keypad 6 and Right Arrow
#define KEY_KP7        0x5f // Keypad 7 and Home
#define KEY_KP8        0x60 // Keypad 8 and Up Arrow
#define KEY_KP9        0x61 // Keypad 9 and Page Up
#define KEY_KP0        0x62 // Keypad 0 and Insert
#define KEY_KPDOT      0x63 // Keypad . and Delete

#define KEY_102ND      0x64 // Keyboard Non-US \ and |
#define KEY_COMPOSE    0x65 // Keyboard Application
#define KEY_POWER      0x66 // Keyboard Power
#define KEY_KPEQUAL    0x67 // Keypad =
//}}}
//{{{  unusual keys
#define KEY_F13        0x68 // Keyboard F13
#define KEY_F14        0x69 // Keyboard F14
#define KEY_F15        0x6a // Keyboard F15
#define KEY_F16        0x6b // Keyboard F16
#define KEY_F17        0x6c // Keyboard F17
#define KEY_F18        0x6d // Keyboard F18
#define KEY_F19        0x6e // Keyboard F19
#define KEY_F20        0x6f // Keyboard F20
#define KEY_F21        0x70 // Keyboard F21
#define KEY_F22        0x71 // Keyboard F22
#define KEY_F23        0x72 // Keyboard F23
#define KEY_F24        0x73 // Keyboard F24

#define KEY_OPEN       0x74 // Keyboard Execute
#define KEY_HELP       0x75 // Keyboard Help
#define KEY_PROPS      0x76 // Keyboard Menu
#define KEY_FRONT      0x77 // Keyboard Select
#define KEY_STOP       0x78 // Keyboard Stop
#define KEY_AGAIN      0x79 // Keyboard Again
#define KEY_UNDO       0x7a // Keyboard Undo
#define KEY_CUT        0x7b // Keyboard Cut
#define KEY_COPY       0x7c // Keyboard Copy
#define KEY_PASTE      0x7d // Keyboard Paste
#define KEY_FIND       0x7e // Keyboard Find
#define KEY_MUTE       0x7f // Keyboard Mute
#define KEY_VOLUMEUP   0x80 // Keyboard Volume Up
#define KEY_VOLUMEDOWN 0x81 // Keyboard Volume Down
//}}}
//{{{  international key defines
// 0x82  Keyboard Locking Caps Lock
// 0x83  Keyboard Locking Num Lock
// 0x84  Keyboard Locking Scroll Lock
#define KEY_KPCOMMA    0x85 // Keypad Comma
// 0x86  Keypad Equal Sign
#define KEY_RO 0x87 // Keyboard International1
#define KEY_KATAKANAHIRAGANA 0x88 // Keyboard International2
#define KEY_YEN 0x89 // Keyboard International3
#define KEY_HENKAN 0x8a // Keyboard International4
#define KEY_MUHENKAN 0x8b // Keyboard International5
#define KEY_KPJPCOMMA 0x8c // Keyboard International6
// 0x8d  Keyboard International7
// 0x8e  Keyboard International8
// 0x8f  Keyboard International9
#define KEY_HANGEUL 0x90 // Keyboard LANG1
#define KEY_HANJA 0x91 // Keyboard LANG2
#define KEY_KATAKANA 0x92 // Keyboard LANG3
#define KEY_HIRAGANA 0x93 // Keyboard LANG4
#define KEY_ZENKAKUHANKAKU 0x94 // Keyboard LANG5
// 0x95  Keyboard LANG6
// 0x96  Keyboard LANG7
// 0x97  Keyboard LANG8
// 0x98  Keyboard LANG9
// 0x99  Keyboard Alternate Erase
// 0x9a  Keyboard SysReq/Attention
// 0x9b  Keyboard Cancel
// 0x9c  Keyboard Clear
// 0x9d  Keyboard Prior
// 0x9e  Keyboard Return
// 0x9f  Keyboard Separator
// 0xa0  Keyboard Out
// 0xa1  Keyboard Oper
// 0xa2  Keyboard Clear/Again
// 0xa3  Keyboard CrSel/Props
// 0xa4  Keyboard ExSel
//}}}
//{{{  keypad key defines
// 0xb0  Keypad 00
// 0xb1  Keypad 000
// 0xb2  Thousands Separator
// 0xb3  Decimal Separator
// 0xb4  Currency Unit
// 0xb5  Currency Sub-unit
#define KEY_KPLEFTPAREN    0xb6 // Keypad (
#define KEY_KPRIGHTPAREN   0xb7 // Keypad )
// 0xb8  Keypad {
// 0xb9  Keypad }
// 0xba  Keypad Tab
// 0xbb  Keypad Backspace
// 0xbc  Keypad A
// 0xbd  Keypad B
// 0xbe  Keypad C
// 0xbf  Keypad D
// 0xc0  Keypad E
// 0xc1  Keypad F
// 0xc2  Keypad XOR
// 0xc3  Keypad ^
// 0xc4  Keypad %
// 0xc5  Keypad <
// 0xc6  Keypad >
// 0xc7  Keypad &
// 0xc8  Keypad &&
// 0xc9  Keypad |
// 0xca  Keypad ||
// 0xcb  Keypad :
// 0xcc  Keypad #
// 0xcd  Keypad Space
// 0xce  Keypad @
// 0xcf  Keypad !
// 0xd0  Keypad Memory Store
// 0xd1  Keypad Memory Recall
// 0xd2  Keypad Memory Clear
// 0xd3  Keypad Memory Add
// 0xd4  Keypad Memory Subtract
// 0xd5  Keypad Memory Multiply
// 0xd6  Keypad Memory Divide
// 0xd7  Keypad +/-
// 0xd8  Keypad Clear
// 0xd9  Keypad Clear Entry
// 0xda  Keypad Binary
// 0xdb  Keypad Octal
// 0xdc  Keypad Decimal
// 0xdd  Keypad Hexadecimal
//}}}
//{{{  more modifier key defines
#define KEY_LEFTCTRL   0xe0 // Keyboard Left Control
#define KEY_LEFTSHIFT  0xe1 // Keyboard Left Shift
#define KEY_LEFTALT    0xe2 // Keyboard Left Alt
#define KEY_LEFTMETA   0xe3 // Keyboard Left GUI
#define KEY_RIGHTCTRL  0xe4 // Keyboard Right Control
#define KEY_RIGHTSHIFT 0xe5 // Keyboard Right Shift
#define KEY_RIGHTALT   0xe6 // Keyboard Right Alt
#define KEY_RIGHTMETA  0xe7 // Keyboard Right GUI
//}}}
//{{{  media key defines
#define KEY_MEDIA_PLAYPAUSE 0xe8
#define KEY_MEDIA_STOPCD 0xe9
#define KEY_MEDIA_PREVIOUSSONG 0xea
#define KEY_MEDIA_NEXTSONG 0xeb
#define KEY_MEDIA_EJECTCD 0xec
#define KEY_MEDIA_VOLUMEUP 0xed
#define KEY_MEDIA_VOLUMEDOWN 0xee
#define KEY_MEDIA_MUTE 0xef
#define KEY_MEDIA_WWW 0xf0
#define KEY_MEDIA_BACK 0xf1
#define KEY_MEDIA_FORWARD 0xf2
#define KEY_MEDIA_STOP 0xf3
#define KEY_MEDIA_FIND 0xf4
#define KEY_MEDIA_SCROLLUP 0xf5
#define KEY_MEDIA_SCROLLDOWN 0xf6
#define KEY_MEDIA_EDIT 0xf7
#define KEY_MEDIA_SLEEP 0xf8
#define KEY_MEDIA_COFFEE 0xf9
#define KEY_MEDIA_REFRESH 0xfa
#define KEY_MEDIA_CALC 0xfb
//}}}
//}}}
//{{{  ps2 keyboard defines
#define PS2_KEYMAP_SIZE     136

#define PS2_TAB        9
#define PS2_ENTER      13
#define PS2_BACKSPACE  127
#define PS2_ESC        27
#define PS2_INSERT     0
#define PS2_DELETE     127
#define PS2_HOME       0
#define PS2_END        0
#define PS2_PAGEUP     25
#define PS2_PAGEDOWN   26
#define PS2_UPARROW    11
#define PS2_LEFTARROW  8
#define PS2_DOWNARROW  10
#define PS2_RIGHTARROW 21

#define PS2_F1         0
#define PS2_F2         0
#define PS2_F3         0
#define PS2_F4         0
#define PS2_F5         0
#define PS2_F6         0
#define PS2_F7         0
#define PS2_F8         0
#define PS2_F9         0
#define PS2_F10        0
#define PS2_F11        0
#define PS2_F12        0
#define PS2_SCROLL     0

#define PS2_INVERTED_EXCLAMATION  161
#define PS2_CENT_SIGN       162
#define PS2_POUND_SIGN      163
#define PS2_CURRENCY_SIGN   164
#define PS2_YEN_SIGN        165
#define PS2_BROKEN_BAR      166
#define PS2_SECTION_SIGN    167
#define PS2_DIAERESIS       168
#define PS2_COPYRIGHT_SIGN  169
#define PS2_FEMININE_ORDINAL 170
#define PS2_LEFT_DOUBLE_ANGLE_QUOTE 171
#define PS2_NOT_SIGN        172
#define PS2_HYPHEN          173
#define PS2_REGISTERED_SIGN 174
#define PS2_MACRON          175
#define PS2_DEGREE_SIGN     176
#define PS2_PLUS_MINUS_SIGN 177
#define PS2_SUPERSCRIPT_TWO 178
#define PS2_SUPERSCRIPT_THREE 179
#define PS2_ACUTE_ACCENT    180
#define PS2_MICRO_SIGN      181
#define PS2_PILCROW_SIGN    182
#define PS2_MIDDLE_DOT      183
#define PS2_CEDILLA         184
#define PS2_SUPERSCRIPT_ONE 185
#define PS2_MASCULINE_ORDINAL 186
#define PS2_RIGHT_DOUBLE_ANGLE_QUOTE 187
#define PS2_FRACTION_ONE_QUARTER 188
#define PS2_FRACTION_ONE_HALF 189
#define PS2_FRACTION_THREE_QUARTERS 190
#define PS2_INVERTED_QUESTION MARK  191
#define PS2_A_GRAVE         192
#define PS2_A_ACUTE         193
#define PS2_A_CIRCUMFLEX    194
#define PS2_A_TILDE         195
#define PS2_A_DIAERESIS     196
#define PS2_A_RING_ABOVE    197
#define PS2_AE              198
#define PS2_C_CEDILLA       199
#define PS2_E_GRAVE         200
#define PS2_E_ACUTE         201
#define PS2_E_CIRCUMFLEX    202
#define PS2_E_DIAERESIS     203
#define PS2_I_GRAVE         204
#define PS2_I_ACUTE         205
#define PS2_I_CIRCUMFLEX    206
#define PS2_I_DIAERESIS     207
#define PS2_ETH             208
#define PS2_N_TILDE         209
#define PS2_O_GRAVE         210
#define PS2_O_ACUTE         211
#define PS2_O_CIRCUMFLEX    212
#define PS2_O_TILDE         213
#define PS2_O_DIAERESIS     214
#define PS2_MULTIPLICATION  215
#define PS2_O_STROKE        216
#define PS2_U_GRAVE         217
#define PS2_U_ACUTE         218
#define PS2_U_CIRCUMFLEX    219
#define PS2_U_DIAERESIS     220
#define PS2_Y_ACUTE         221
#define PS2_THORN           222
#define PS2_SHARP_S         223
#define PS2_a_GRAVE         224
#define PS2_a_ACUTE         225
#define PS2_a_CIRCUMFLEX    226
#define PS2_a_TILDE         227
#define PS2_a_DIAERESIS     228
#define PS2_a_RING_ABOVE    229
#define PS2_ae              230
#define PS2_c_CEDILLA       231
#define PS2_e_GRAVE         232
#define PS2_e_ACUTE         233
#define PS2_e_CIRCUMFLEX    234
#define PS2_e_DIAERESIS     235
#define PS2_i_GRAVE         236
#define PS2_i_ACUTE         237
#define PS2_i_CIRCUMFLEX    238
#define PS2_i_DIAERESIS     239
#define PS2_eth             240
#define PS2_n_TILDE         241
#define PS2_o_GRAVE         242
#define PS2_o_ACUTE         243
#define PS2_o_CIRCUMFLEX    244
#define PS2_o_TILDE         245
#define PS2_o_DIAERESIS     246
#define PS2_DIVISION        247
#define PS2_o_STROKE        248
#define PS2_u_GRAVE         249
#define PS2_u_ACUTE         250
#define PS2_u_CIRCUMFLEX    251
#define PS2_u_DIAERESIS     252
#define PS2_y_ACUTE         253
#define PS2_thorn           254
#define PS2_y_DIAERESIS     255
//}}}

//{{{
typedef struct tHidLookup {
  uint8_t mHidCode;
  uint8_t mModifierBits;
  uint8_t mExtendedHidCodes;
  uint8_t mExtendedModifierBits;
  uint8_t mAsciiCode;
  } tHidLookup;
//}}}
//{{{
const tHidLookup kPs2Key[0x83] = {
 //{{{  0x00
 {0,0,0,0,0},
 {0x42,0,0,0,0},   // F9
 {0,0,0,0,0},
 {0x3e,0,0,0,0},   // F5
 {0x3c,0,0,0,0},   // F3
 {0x3a,0,0,0,0},   // F1
 {0x3b,0,0,0,0},   // F2
 {0x45,0,0,0,0},   // F12

 {0,0,0,0,0},
 {0x43,0,0,0,0},   // F10
 {0x41,0,0,0,0},   // F8
 {0x3f,0,0,0,0},   // F6
 {0x3d,0,0,0,0},   // F4
 {0x2b,0,0,0,0},   // tab
 {0x35,0,0,0,0},   // ` ~
 {0,0,0,0,0},
 //}}}
 //{{{  0x10
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x20
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x30
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x40
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x50
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x60
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x70
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 //}}}
 //{{{  0x80
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0},
 {0,0,0,0,0}, // 0x83
 //}}}
 };
//}}}

class cPs2 {
public:
  //{{{
  cPs2 (cLcd* lcd) : mLcd(lcd) {
    initGpio();
    }
  //}}}

  //{{{
  void initKeyboard() {

    sendChar (0xFF);
    if (getRawChar() != 0xAA)
      mLcd->debug (LCD_COLOR_RED, "initPs2keyboard - missing 0xAA reset");

    // send getId
    sendChar (0x0F2);
    mLcd->debug (LCD_COLOR_YELLOW, "keyboard id %x %x", getRawChar(), getRawChar());

    resetChar();
    }
  //}}}
  //{{{
  void initTouchpad() {

    // reset
    sendChar (0xFF);

    // reset reply
    auto reply = getRawChar();
    if (reply != 0xAA)
      mLcd->debug (LCD_COLOR_RED, "initTouchpad - no 0xAA reply", reply);

    reply = getRawChar();
    if (reply != 0x00)
      mLcd->debug (LCD_COLOR_RED, "initTouchpad - missing 0x00 reset");
    mLcd->debug (LCD_COLOR_YELLOW, "initTouchpad - reset");

    sendTouchpadSpecialCommand (0x00);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto minor = getRawChar();
    getRawChar();  // 0x47
    auto major = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "Identify %x.%x.%x", major & 0x0F, minor, major >> 4);

    sendTouchpadSpecialCommand (0x02);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto capMsb = getRawChar();
    auto mid = getRawChar();  // 0x47
    auto capLsb = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "Capabilities %x %x", mid, (capMsb << 8) | capLsb);

    sendTouchpadSpecialCommand (0x03);
    sendChar (0xE9); // touchpad statusRequest prefixed by specialCommand
    auto modelId1 = getRawChar();
    auto modelId2 = getRawChar();
    auto modelId3 = getRawChar();
    mLcd->debug (LCD_COLOR_YELLOW, "ModelId %x %x %x", modelId1, modelId2, modelId3);

    sendTouchpadSpecialCommand (0x80);
    sendChar (0xF3); // touchpad setSampleRate prefixed by specialCommand
    sendChar (0x14); // - setSampleRate = 20
    sendChar (0xF4); // touchpad enable streaming

    mStream = true;
    }
  //}}}

  bool hasChar() { return mInPtr != mOutPtr; }
  bool hasRawChar() { return mInRawPtr != mOutRawPtr; }
  //{{{
  uint16_t getChar() {

    while (!hasChar()) { HAL_Delay (1); }
    uint16_t ch = mRxData[mOutPtr];
    mOutPtr = (mOutPtr + 1) % kMaxRing;
    return ch;
    }
  //}}}
  //{{{
  uint16_t getRawChar() {

    while (!hasRawChar()) { HAL_Delay (1); }
    uint16_t ch = mRxRawData[mOutRawPtr];
    mOutRawPtr = (mOutRawPtr + 1) % kMaxRing;
    return ch;
    }
  //}}}
  //{{{
  bool getTouch (int& touch, int& x, int& y, int& z) {

    touch = mTouchZ > 0;
    x = mTouchX;
    y = mTouchY;
    z = mTouchZ;
    return mTouchZ > 0;
    }
  //}}}

  //{{{
  void sendLeds (int i) {

    sendChar (0xED);
    sendChar (i);
    HAL_Delay (100);
    }
  //}}}
  //{{{
  void resetChar() {

    mInPtr = 0;
    mOutPtr = 0;

    mInRawPtr = 0;
    mOutRawPtr = 0;

    mCurCode = 0;
    mCurChar = 0;
    }
  //}}}

  //{{{
  void showTouch() {

    BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);

    char str[8];
    sprintf (str, "%d", mTouchX);
    BSP_LCD_DisplayStringAtLineColumn (13, 0, str);

    sprintf (str, "%d", mTouchY);
    BSP_LCD_DisplayStringAtLineColumn (13, 6, str);

    sprintf (str, "%d", mTouchZ);
    BSP_LCD_DisplayStringAtLineColumn (13, 12, str);

    sprintf (str, "%d", mStreamCode);
    BSP_LCD_DisplayStringAtLineColumn (13, 18, str);

    sprintf (str, "%x", mStreamCodes[0]);
    BSP_LCD_DisplayStringAtLineColumn (13, 22, str);

    sprintf (str, "%x", mStreamCodes[1]);
    BSP_LCD_DisplayStringAtLineColumn (13, 25, str);

    sprintf (str, "%x", mStreamCodes[2]);
    BSP_LCD_DisplayStringAtLineColumn (13, 28, str);

    sprintf (str, "%x", mStreamCodes[3]);
    BSP_LCD_DisplayStringAtLineColumn (13, 31, str);

    sprintf (str, "%x", mStreamCodes[4]);
    BSP_LCD_DisplayStringAtLineColumn (13, 34, str);

    sprintf (str, "%x", mStreamCodes[5]);
    BSP_LCD_DisplayStringAtLineColumn (13, 37, str);
    }
  //}}}
  //{{{
  void showChars() {

    auto ch = mCurChar - kMaxChars + 1;
    for (auto i = 0u; i < kMaxChars; i++) {
      if (ch > 0) {
        char str[2];
        BSP_LCD_SetTextColor (mChars[ch % kMaxChars] & 0x100 ? LCD_COLOR_GREEN : LCD_COLOR_YELLOW);
        str[0] = mChars[ch % kMaxChars] & 0xFF;
        str[1] = 0;
        BSP_LCD_DisplayStringAtLineColumn (13, i, str);
        }
      ch++;
      }
    }
  //}}}
  //{{{
  void showCodes() {

    BSP_LCD_SetTextColor (LCD_COLOR_YELLOW);

    auto code = mCurCode - kMaxCodes + 1;
    for (auto i = 0u; i < kMaxCodes; i++) {
      if (code > 0) {
        char str[4];
        sprintf (str, "%02x", mCodes[code % kMaxCodes]);
        BSP_LCD_DisplayStringAtLineColumn (14, i*3, str);
        }
      code++;
      }
    }
  //}}}
  //{{{
  void showWave() {

    int bitHeight = 12;
    int clockHeight = 12;
    int lineHeight = 16;
    auto waveY = BSP_LCD_GetYSize() - 2*lineHeight;

    bool lastBit = false;
    auto sample = mSample - kMaxSamples + 1;
    for (auto i = 0u; i < kMaxSamples; i++) {
      if (sample > 0) {
        bool bit =  mBitArray[sample % kMaxSamples];
        int bitPos = mBitPosArray[sample % kMaxSamples];
        sample++;

        if (bit != lastBit) {
          // draw edge
          BSP_LCD_SetTextColor (bitPos == -1 ? LCD_COLOR_RED : LCD_COLOR_WHITE);
          BSP_LCD_FillRect (i*kBitWidth, waveY, 1, bitHeight);
          lastBit = bit;
          }

        // draw bit
        BSP_LCD_SetTextColor ((bitPos == -1) || (bitPos == 9) ? LCD_COLOR_RED : LCD_COLOR_WHITE);
        BSP_LCD_FillRect (i*kBitWidth, waveY + (bit ? 0 : bitHeight-2), kBitWidth, 2);
        }

      BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth/4), waveY + lineHeight, 1, clockHeight);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth/4), waveY + lineHeight , kBitWidth/2, 1);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth*3/4), waveY + lineHeight, 1, clockHeight);
      BSP_LCD_FillRect (i*kBitWidth + (kBitWidth*3/4), waveY + lineHeight + clockHeight, kBitWidth/2, 1);
      }
    }
  //}}}

  //{{{
  void onIrq() {

    if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_8) != RESET) {
      __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_8);

      if (mRx) {
        bool bit = (GPIOF->IDR & GPIO_PIN_9) != 0;

        mSample++;
        mBitArray[mSample % kMaxSamples] = bit;
        mBitPosArray[mSample % kMaxSamples] = mBitPos;

        switch (mBitPos) {
          case -1:
            //{{{  wait for lo start bit
            if (!bit) { // lo start bit
              mBitPos = 0;
              mCode = 0;
              }
            break;
            //}}}
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
            //{{{  bits 0..7
            mCode = mCode | (bit << mBitPos);
            mBitPos++;
            break;
            //}}}
          case 8:
            //{{{  parity bit
            mBitPos++;

            // add to codes debug buffer
            mCurCode++;
            mCodes[mCurCode % kMaxCodes] = mCode;

            if (mStream) {
              //{{{  add to touchpad stream
              if (mStreamCode == -1) {
                if ((mCode & 0xC0) == 0x80) {
                  mStreamCode = 0;
                  mStreamCodes[mStreamCode] = mCode;
                  }
                }

              else {
                mStreamCode++;
                if ((mStreamCode == 3) && ((mCode & 0xc0) != 0xc0))
                  mStreamCode = -1;
                else {
                  mStreamCodes[mStreamCode] = mCode;
                  if (mStreamCode == 5) {
                    mTouchX = ((mStreamCodes[3] & 0x10) << 8) | ((mStreamCodes[1] & 0x0F) << 8) | mStreamCodes[4];
                    mTouchY = ((mStreamCodes[3] & 0x20) << 7) | ((mStreamCodes[1] & 0xF0) << 4) | mStreamCodes[5];
                    mTouchZ = mStreamCodes[2];
                    mStreamCode = -1;
                    }
                  }
                }
              }
              //}}}
            else {
              //{{{  add char code
              mRxRawData[mInRawPtr] = mCode;
              mInRawPtr = (mInRawPtr + 1) % kMaxRing;

              if (mCode == 0xE0)
                mRxExpandCode = true;
              else if (mCode == 0xE1)
                mPauseCode = true;
              else if (mCode == 0xF0)
                mRxReleaseCode = true;
              else if (mCode == 0x12) // SHIFT_L;
                mShiftedL = !mRxReleaseCode;
              else if (mCode == 0x1F) // WINDOWS_L;
                mWindowsL = !mRxReleaseCode;
              else if (mCode == 0x27) // WINDOWS_R;
                mWindowsR = !mRxReleaseCode;
              else if (mCode == 0x59) // SHIFT_R;
                mShiftedR = !mRxReleaseCode;
              else if (mCode == 0x14) // CTRL_L
                mControlled = !mRxReleaseCode;
              else {
                if (mRxExpandCode) {
                  //{{{  expandCode
                  if (mCode == 0x4A)
                    mCode = '/';
                  else if (mCode == 0x5A)
                    mCode = PS2_ENTER;
                  else if (mCode == 0x69)
                    mCode = PS2_END;
                  else if (mCode == 0x6B)
                    mCode = PS2_LEFTARROW;
                  else if (mCode == 0x6C)
                    mCode = PS2_HOME;
                  else if (mCode == 0x70)
                    mCode = PS2_INSERT;
                  else if (mCode == 0x71)
                    mCode = PS2_DELETE;
                  else if (mCode == 0x72)
                    mCode = PS2_DOWNARROW;
                  else if (mCode == 0x74)
                    mCode = PS2_RIGHTARROW;
                  else if (mCode == 0x75)
                    mCode = PS2_UPARROW;
                  else if (mCode == 0x7D)
                    mCode = PS2_PAGEUP;
                  else if (mCode == 0x7A)
                    mCode = PS2_PAGEDOWN;
                  else
                    mCode |= 0x200;
                  }
                  //}}}
                else if (mShiftedL || mShiftedR)
                  mCode = kPs2Keymap.shift[mCode];
                else
                  mCode = kPs2Keymap.noshift[mCode];

                if (!mRxReleaseCode) {
                  // add to decoded chars debug buffer
                  mCurChar++;
                  mChars[mCurChar % kMaxChars] = mCode;
                  }

                // add to partially decoded char ring buffer
                mRxData[mInPtr] = mCode | (mRxReleaseCode * 0x100);
                mInPtr = (mInPtr + 1) % kMaxRing;

                mRxExpandCode = false;
                mRxReleaseCode = false;
                mPauseCode = false;
                }
              }
              //}}}
            break;
            //}}}
          case 9:
            //{{{  hi stop bit
            mBitPos = -1;
            if (!bit)
              mLcd->debug (LCD_COLOR_RED, "lo stop bit");
            break;
            //}}}
          }
        }
      }
    }
  //}}}

private:
  static const int kBitWidth = 8;
  static const int kMaxSamples = 480 / kBitWidth;
  static const int kMaxCodes = 14;
  static const int kMaxChars = 43;
  static const int kMaxRing = 32;
  //{{{
  void initGpio() {

    __HAL_RCC_GPIOF_CLK_ENABLE();

    // init PS2 clock
    GPIO_InitTypeDef GPIO_Init_Structure;
    GPIO_Init_Structure.Mode = GPIO_MODE_IT_RISING;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Pin = GPIO_PIN_8;
    HAL_GPIO_Init (GPIOF, &GPIO_Init_Structure);

    // init PS2 data
    GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Pin = GPIO_PIN_9;
    HAL_GPIO_Init (GPIOF, &GPIO_Init_Structure);

    // enable and set EXTI line 8 Interrupt to the lowest priority
    HAL_NVIC_SetPriority (EXTI9_5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ (EXTI9_5_IRQn);

    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay (100);
    }
  //}}}

  //{{{
  void sendChar (uint8_t value)  {

    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_RESET); // set clock lo, release inhibit, if necessary
    HAL_Delay (2); // Wait out any final clock pulse, 100us

    mRx = false;
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_RESET); // set data lo, start bit
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_8, GPIO_PIN_SET);   // set clock hi, float

    uint8_t parity = 1;
    for (int bit = 0; bit < 8; bit++) {
      while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
      HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, (value & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // set data to i’th data bit
      parity = parity + value;  // Accumulate parity
      value = value >> 1;       // Shift right to get next bit
      while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge
      }

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, (parity & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // set data to parity bit
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    HAL_GPIO_WritePin (GPIOF, GPIO_PIN_9, GPIO_PIN_SET); // set data hi, stop bit
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge

    while (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for rising edge
    //if (HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8) == true)
    //  lcd->info ("sendChar - missing line control bit");
    while (!HAL_GPIO_ReadPin (GPIOF, GPIO_PIN_8)) {} // wait for falling edge
    mRx = true;

    if (getRawChar() != 0xFA)
      mLcd->debug (LCD_COLOR_RED, "send - no 0xFA ack");
    }
  //}}}
  //{{{
  void sendTouchpadSpecialCommand (uint8_t arg) {
  // send touchpad special command sequence

    for (auto i = 0u; i < 4; i++) {
      sendChar (0xE8);
      sendChar ((arg >> (6-2*i)) & 3);
      }
    }
  //}}}

  //{{{  struct ps2Keymap_t
  typedef struct {
    uint8_t noshift[PS2_KEYMAP_SIZE];
    uint8_t shift[PS2_KEYMAP_SIZE];
    uint8_t uses_altgr;
    uint8_t altgr[PS2_KEYMAP_SIZE];
    } ps2Keymap_t;
  //}}}
  //{{{
  const ps2Keymap_t kPs2Keymap = {
    // without shift
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
     0, 0, 'z', 's', 'a', 'w', '2', 0,
     0, 'c', 'x', 'd', 'e', '4', '3', 0,
     0, ' ', 'v', 'f', 't', 'r', '5', 0,
     0, 'n', 'b', 'h', 'g', 'y', '6', 0,
     0, 0, 'm', 'j', 'u', '7', '8', 0,
     0, ',', 'k', 'i', 'o', '0', '9', 0,
     0, '.', '/', 'l', ';', 'p', '-', 0,
     0, 0, '\'', 0, '[', '=', 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
     0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },

    // with shift
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
     0, 0, 'Z', 'S', 'A', 'W', '@', 0,
     0, 'C', 'X', 'D', 'E', '$', '#', 0,
     0, ' ', 'V', 'F', 'T', 'R', '%', 0,
     0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
     0, 0, 'M', 'J', 'U', '&', '*', 0,
     0, '<', 'K', 'I', 'O', ')', '(', 0,
     0, '>', '?', 'L', ':', 'P', '_', 0,
     0, 0, '"', 0, '{', '+', 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
     0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },

    0
    };
  //}}}
  //{{{  vars
  cLcd* mLcd;

  // bits
  bool mRx = true;
  int mBitPos = -1;
  uint16_t mCode = 0;

  // keyboard
  volatile int mInRawPtr = 0;
  volatile int mOutRawPtr = 0;
  int mRxRawData[kMaxRing];

  volatile int mInPtr = 0;
  volatile int mOutPtr = 0;
  int mRxData[kMaxRing];

  int mCurCode = 0;
  int mCodes[kMaxCodes];
  int mCurChar = 0;
  int mChars[kMaxChars];

  bool mRxExpandCode = false;
  bool mRxReleaseCode = false;
  bool mPauseCode = false;
  bool mShiftedL = false;
  bool mShiftedR = false;
  bool mControlled = false;
  bool mWindowsL = false;
  bool mWindowsR = false;

  // touchpad
  bool mStream = false;
  int mStreamCode = -1;
  uint8_t mStreamCodes[6];
  int mTouchX = 0;
  int mTouchY = 0;
  int mTouchZ = 0;

  // waveform
  uint16_t mSample = 0;
  bool mBitArray[kMaxSamples];
  int mBitPosArray[kMaxSamples];
  //}}}
  };
