// ============================================================
// File:     UCCD Informative device system
// ============================================================
// Author:      Amir
// Last update: 26.02.2022
// License:     MIT
// ============================================================
// Description:
// This is a system for the UCCD Informative device which is used
// to let visitors know about the center and it's services.
// It uses U8g2Lib to run the GLCD,
//  SoftwareSerial & DFRobotDFPlayerMini to communicate with the DF Player module
// ============================================================

// *********************************************************************
// special settings
// *********************************************************************
// enable this line when you are not using a standard arduino
// for example when your chip is an ESP or a STM or SAM or something else
//#define _LCDML_cfg_use_ram

// include libs
#include <Arduino.h>
#include <U8g2lib.h>
#include <LCDMenuLib2.h>
#include <SoftwareSerial.h>
#include <DFPlayer.h>
#include <EEPROM.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// *********************************************************************
// U8GLIB
// *********************************************************************
// U8g2 Constructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/13, /* data=*/11, /* CS=*/10, /* reset=*/8);

// BLA pin for Backlit control
#define BLA 9

// Buzzer pin & freq
#define buzpin 12
#define buzfreq 2000
#define buzdur 100

// DFPlayer Serial
#define MP3_RX_PIN 7            // GPIO5/D1
#define MP3_TX_PIN 8            // GPIO4/D2
#define MP3_SERIAL_SPEED 9600   // DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_TIMEOUT 100  // average DFPlayer response timeout 100msec..200msec

//EEPROM Addresses
#define a_soundLvl 0
#define a_screensaverTimeout 1

//EEPROM VARs
uint8_t g_soundLvl = EEPROM.read(a_soundLvl);
uint8_t g_screensaverTimeout = EEPROM.read(a_screensaverTimeout);

// settings for u8g lib and LCD
#define _LCDML_DISP_w 128  // LCD width
#define _LCDML_DISP_h 64   // LCD height
// font settings
#define _LCDML_DISP_font u8g2_font_unifont_t_arabic  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_DISP_font_w 16                        // font width
#define _LCDML_DISP_font_h 16                        // font height
// cursor settings
#define _LCDML_DISP_cursor_char "<"      // cursor char
#define _LCDML_DISP_cur_space_before 10  // cursor space between
#define _LCDML_DISP_cur_space_behind -6  // cursor space between
// menu position and size
#define _LCDML_DISP_box_x0 0      // start point (x0, y0)
#define _LCDML_DISP_box_y0 0      // start point (x0, y0)
#define _LCDML_DISP_box_x1 128    // width x  (x0 + width)
#define _LCDML_DISP_box_y1 64     // hight y  (y0 + height)
#define _LCDML_DISP_draw_frame 1  // draw a box around the menu
// scrollbar width
#define _LCDML_DISP_scrollbar_w 4  // scrollbar width (if this value is < 3, the scrollbar is disabled)

// nothing change here
#define _LCDML_DISP_cols_max ((_LCDML_DISP_box_x1 - _LCDML_DISP_box_x0) / _LCDML_DISP_font_w)
#define _LCDML_DISP_rows_max ((_LCDML_DISP_box_y1 - _LCDML_DISP_box_y0 - ((_LCDML_DISP_box_y1 - _LCDML_DISP_box_y0) / _LCDML_DISP_font_h)) / _LCDML_DISP_font_h)

// rows and cols
// when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// the program needs more ram with this changes
#define _LCDML_DISP_rows _LCDML_DISP_rows_max  // max rows
#define _LCDML_DISP_cols 40                    // max cols

// *********************************************************************
// Prototypes
// *********************************************************************
void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();

// *********************************************************************
// Objects
// *********************************************************************
LCDMenuLib2_menu LCDML_0(255, 0, 0, NULL, NULL);  // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
// LCDML_0        => layer 0
// LCDML_0_X      => layer 1
// LCDML_0_X_X    => layer 2
// LCDML_0_X_X_X  => layer 3
// LCDML_0_...      => layer ...
// For beginners
// LCDML_add(id, prev_layer, new_num, lang_char_array, callback_function)
LCDML_add(0, LCDML_0, 1, "ﻦﺤﻧ ﻦﻣ", NULL);            // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(1, LCDML_0, 2, "ﺎﻨﺗﺎﻣﺪﺧ", NULL);           // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(2, LCDML_0_2, 1, "ﺔﻳﺩﺮﻓ ﺕﺎﺴﻠﺟ", NULL);     // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(3, LCDML_0_2, 2, "ﻞﻤﻋ ﺵﺭﻭ", NULL);         // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(4, LCDML_0_2, 3, "ﺔﻴﻔﻴﻇﻭ ﺕﺍﺀﺎﻘﻟ", NULL);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(5, LCDML_0_2, 4, "ﻉﻮﺟﺮﻟﺍ", mFunc_back);    // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(6, LCDML_0, 3, "ﺔﻳﺅﺮﻟﺍ", NULL);            // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(7, LCDML_0, 4, "ﺔﻟﺎﺳﺮﻟﺍ", NULL);           // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(8, LCDML_0, 5, "ﺐﻳﺭﺪﺘﻟﺍ ﺞﻣﺍﺮﺑ", NULL);     // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(9, LCDML_0_5, 1, "ﺔﻴﻨﻘﺗ ﺕﺍﺭﺎﻬﻣ", NULL);    // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(10, LCDML_0_5, 2, "ﺔﻴﺼﺨﺷ ﺕﺍﺭﺎﻬﻣ", NULL);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(11, LCDML_0_5, 3, "ﻉﻮﺟﺮﻟﺍ", mFunc_back);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(12, LCDML_0, 6, "ﻥﻭﺪﻴﻔﺘﺴﻤﻟﺍ", NULL);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(13, LCDML_0_6, 1, "ﺏﻼﻄﻟﺍ", NULL);          // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(14, LCDML_0_6, 2, "ﻥﻮﻔﻇﻮﻤﻟﺍ", NULL);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(15, LCDML_0_6, 3, "ﻝﺎﻤﻋﻷﺍ ﺏﺎﺤﺻﺃ", NULL);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(16, LCDML_0_6, 4, "ﻉﻮﺟﺮﻟﺍ", mFunc_back);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(17, LCDML_0, 7, "ﻞﻤﻌﻟﺍ ﻖﻳﺮﻓ", NULL);       // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(18, LCDML_0, 8, "ﻦﻴﻋﻮﻄﺘﻤﻟﺍ ﻖﻳﺮﻓ", NULL);   // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(19, LCDML_0, 9, "ﻞﺻﺍﻮﺘﻠﻟ", NULL);          // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_add(20, LCDML_0, 10, "ﻡﺎﻈﻨﻟﺍ ﺕﺍﺩﺍﺪﻋﺇ", NULL);  // this menu function can be found on "LCDML_display_menuFunction" tab

// Advanced menu part with more settings
// Example for one function and different parameters
// It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
// the parameter change can also be released with dynParams on the next example
// LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
LCDML_addAdvanced(21, LCDML_0_10, 1, NULL, "ﺕﻮﺼﻟﺍ ﺔﻐﻟ", NULL, 0, _LCDML_TYPE_default);         // this menu function can be found on "LCDML_display_menuFunction" tab
LCDML_addAdvanced(22, LCDML_0_10_1, 1, NULL, "ﻰﺤﺼﻓ", mFunc_lang, 10, _LCDML_TYPE_default);     // NULL = no menu function
LCDML_addAdvanced(23, LCDML_0_10_1, 2, NULL, "ﺔﻴﻣﺎﻋ", mFunc_lang, 20, _LCDML_TYPE_default);    // NULL = no menu function
LCDML_addAdvanced(24, LCDML_0_10_1, 3, NULL, "English", mFunc_lang, 30, _LCDML_TYPE_default);  // NULL = no menu function
LCDML_add(25, LCDML_0_10_1, 4, "ﻉﻮﺟﺮﻟﺍ", mFunc_back);                                          // this menu function can be found on "LCDML_display_menuFunction" tab

// dynamic content
// 1. set the string to ""
// 2. use type  _LCDML_TYPE_dynParam   instead of    _LCDML_TYPE_default
// this function type can not be used in combination with different parameters
// LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
LCDML_addAdvanced(26, LCDML_0_10, 2, NULL, "", mSound_Lvl, 0, _LCDML_TYPE_dynParam);
LCDML_addAdvanced(27, LCDML_0_10, 3, NULL, "", mScreensaver_Timeout, 0, _LCDML_TYPE_dynParam);
LCDML_add(28, LCDML_0_10, 4, "ﻉﻮﺟﺮﻟﺍ", mFunc_back);  // this menu function can be found on "LCDML_display_menuFunction" tab

// Example for conditions (for example for a screensaver)
// 1. define a condition as a function of a boolean type -> return false = not displayed, return true = displayed
// 2. set the function name as callback (remove the braces '()' it gives bad errors)
// LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
LCDML_addAdvanced(29, LCDML_0, 11, COND_hide, "screensaver", mFunc_screensaver, 0, _LCDML_TYPE_default);  // this menu function can be found on "LCDML_display_menuFunction" tab

// ***TIP*** Try to update _LCDML_DISP_cnt when you add a menu element.

// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt 29

// create menu
LCDML_createMenu(_LCDML_DISP_cnt);

// Initiating Connection to DFPlayer
SoftwareSerial mp3Serial(MP3_RX_PIN, MP3_TX_PIN);
DFPlayer mp3;

// *********************************************************************
// UCCD Logo
// *********************************************************************
#define UCCD_BMPWIDTH 73
#define UCCD_BMPHEIGHT 62

const unsigned char uccd_logo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x07, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0E, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E,
  0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xFC, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0xFC, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xF8, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xF8, 0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
  0x03, 0x00, 0x06, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00,
  0x07, 0x00, 0x80, 0xFF, 0x03, 0x00, 0x00, 0xE0, 0x1F, 0xC0, 0x07, 0x00,
  0xF0, 0xFF, 0x07, 0x00, 0x00, 0xF0, 0xFF, 0xF8, 0x07, 0x00, 0xFE, 0xFF,
  0x1F, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x07, 0x80, 0xFF, 0xFF, 0x3F, 0x00,
  0x00, 0xFC, 0xFF, 0xFF, 0x03, 0xF0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0xFB,
  0xFF, 0xFF, 0x03, 0xFC, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFB, 0xFF, 0xFF,
  0x81, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x80, 0xFB, 0xFF, 0xFF, 0xE0, 0xFF,
  0xFF, 0xFF, 0x7F, 0x00, 0x80, 0xF3, 0x7F, 0x0A, 0xFC, 0xFF, 0xFF, 0xFF,
  0xFF, 0x00, 0xC0, 0xE7, 0x7F, 0x80, 0x9F, 0xFF, 0xFF, 0xFF, 0xEF, 0x00,
  0xC0, 0x8F, 0xFF, 0xF1, 0x03, 0xFF, 0xFF, 0x1F, 0x8E, 0x00, 0xE0, 0x1F,
  0xF0, 0xFF, 0x33, 0xFE, 0xFF, 0xC3, 0x7F, 0x00, 0xE0, 0xFF, 0xF5, 0x3F,
  0x63, 0xFE, 0xFF, 0xF8, 0x7F, 0x00, 0xE0, 0xFF, 0xFF, 0x07, 0x67, 0xFC,
  0x3F, 0xFF, 0x7F, 0x00, 0xE0, 0xFF, 0xFF, 0x67, 0xE6, 0xFC, 0xCF, 0xFF,
  0x3F, 0x00, 0xF0, 0x67, 0x0E, 0xE6, 0xCF, 0xF8, 0xF3, 0xFF, 0x1F, 0x00,
  0xF0, 0xC7, 0xCC, 0xE4, 0xCF, 0xF9, 0xFC, 0xFF, 0x0F, 0x00, 0xE0, 0xE7,
  0xCC, 0xCC, 0x8F, 0x3C, 0xFE, 0xFF, 0x01, 0x00, 0xE0, 0xCF, 0xCC, 0xCF,
  0x19, 0x8E, 0xE7, 0x1F, 0x00, 0x00, 0xE0, 0xCF, 0xCC, 0xCF, 0x99, 0xA3,
  0xE0, 0x03, 0x00, 0x00, 0xE0, 0xCF, 0x8C, 0x8F, 0xF9, 0x01, 0xF0, 0x00,
  0x00, 0x00, 0xC0, 0xCF, 0x98, 0x19, 0xFC, 0x03, 0x70, 0x00, 0x00, 0x00,
  0x80, 0xCF, 0x99, 0x79, 0xFF, 0x1F, 0x38, 0x00, 0x00, 0x00, 0x80, 0x8F,
  0x18, 0xF8, 0xFF, 0x1F, 0x30, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3C, 0xFC,
  0xF8, 0x3F, 0x20, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x3F, 0xE0, 0x3F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x07, 0x80, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// *********************************************************************
// SETUP
// *********************************************************************
void setup() {

  u8g2.begin();
  // Turning on Backlit
  pinMode(BLA, OUTPUT);
  digitalWrite(BLA, HIGH);

  do {
    u8g2.drawXBMP((_LCDML_DISP_box_x1 - UCCD_BMPWIDTH) / 2, _LCDML_DISP_box_x0, UCCD_BMPWIDTH, UCCD_BMPHEIGHT, uccd_logo);
  } while (u8g2.nextPage());
  ;

  // DFPlayer init
  mp3Serial.begin(MP3_SERIAL_SPEED);  // false=signal not inverted, 0=ISR/RX buffer size (shared with serial TX buffer)

  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, DFPLAYER_MINI, false);  // DFPLAYER_MINI see NOTE, false=no response from module after the command

  mp3.stop();   // if player was runing during ESP8266 reboot
  mp3.reset();  // reset all setting to default

  mp3.setSource(2);  // 1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash

  mp3.setEQ(0);       // 0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(20);  // 0..30, module persists volume on power failure

  mp3.sleep();  // inter sleep mode, 24mA

  // serial init; only be needed if serial control is used
  // Serial.begin(9600);                // start serial

  // LCDMenuLib Setup
  LCDML_setup(_LCDML_DISP_cnt);

  // Enable Menu Rollover
  LCDML.MENU_enRollover();

  // Enable Screensaver (screensaver menu function, time to activate in ms)
  if (g_screensaverTimeout == 0) {
    LCDML.SCREEN_disable();
  }
  else {
    LCDML.SCREEN_enable(mFunc_screensaver, g_screensaverTimeout * 1000);
  }
  // LCDML.SCREEN_disable();

  // Some needful methods

  // You can jump to a menu function from anywhere with
  // LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name
}

// *********************************************************************
// LOOP
// *********************************************************************
void loop() {
  // this function must called here, do not delete it
  LCDML.loop();
}

// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
#if (_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
#error _LCDML_glcd_tft_box_x1 is to big
#endif

#if (_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
#error _LCDML_glcd_tft_box_y1 is to big
#endif
