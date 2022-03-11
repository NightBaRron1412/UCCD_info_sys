/* ===================================================================== *
 *                                                                       *
   Dynamic content
 *                                                                       *
   =====================================================================
*/


//uint8_t g_soundLvl = 20; // when this value comes from an EEPROM, load it in setup
//uint8_t g_screensaverTimeout = 10; // when this value comes from an EEPROM, load it in setup

// at the moment here is no setup function (To-Do)
void mSound_Lvl(uint8_t line)
// *********************************************************************
{
  // check if this function is active (cursor stands on this line)
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if (LCDML.BT_checkAny())
    {
      if (LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if (LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...

        LCDML.BT_resetEnter();
      }

      // This check have only an effect when MENU_disScroll is set
      if (LCDML.BT_checkUp())
      {
        if (g_soundLvl != 30) {
          g_soundLvl += 5;
        }
        LCDML.BT_resetUp();
      }

      // This check have only an effect when MENU_disScroll is set
      if (LCDML.BT_checkDown())
      {
        if (g_soundLvl != 0) {
          g_soundLvl -= 5;
        }
        LCDML.BT_resetDown();
      }


      if (LCDML.BT_checkLeft())
      {
        if (g_soundLvl != 30) {
          g_soundLvl += 5;
        }
        LCDML.BT_resetLeft();
      }

      if (LCDML.BT_checkRight())
      {
        if (g_soundLvl != 0) {
          g_soundLvl -= 5;
        }
        LCDML.BT_resetRight();
      }


    }
  }

  char buf[40];
  sprintf (buf, "%d:ﺕﻮﺼﻟﺍ ﻯﻮﺘﺴﻣ", g_soundLvl);

  // setup function
  u8g2.drawUTF8( _LCDML_DISP_box_x1 - u8g2.getUTF8Width(buf) - _LCDML_DISP_font_w - _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1 + line)), buf); // the value can be changed with left or right
  EEPROM.update(a_soundLvl, g_soundLvl);
  mp3.setVolume(g_soundLvl);

}

void mScreensaver_Timeout(uint8_t line)
// *********************************************************************
{
  // check if this function is active (cursor stands on this line)
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if (LCDML.BT_checkAny())
    {
      if (LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if (LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...

        LCDML.BT_resetEnter();
      }

      // This check have only an effect when MENU_disScroll is set
      if (LCDML.BT_checkUp())
      {
        if (g_screensaverTimeout != 120) {
          g_screensaverTimeout += 10;
        }
        LCDML.BT_resetUp();
      }

      // This check have only an effect when MENU_disScroll is set
      if (LCDML.BT_checkDown())
      {
        if (g_screensaverTimeout != 0) {
          g_screensaverTimeout -= 10;
        }
        LCDML.BT_resetDown();
      }


      if (LCDML.BT_checkLeft())
      {
        if (g_screensaverTimeout != 120) {
          g_screensaverTimeout += 10;
        }
        LCDML.BT_resetLeft();
      }

      if (LCDML.BT_checkRight())
      {
        if (g_screensaverTimeout != 0) {
          g_screensaverTimeout -= 10;
        }
        LCDML.BT_resetRight();
      }


    }
  }

  char buf[40];
  sprintf (buf, "%d:ﻒﻗﻮﺘﻟﺍ ﻦﻣﺯ", g_screensaverTimeout);

  // setup function
  u8g2.drawUTF8( _LCDML_DISP_box_x1 - u8g2.getUTF8Width(buf) - _LCDML_DISP_font_w - _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1 + line)), buf); // the value can be changed with left or right
  EEPROM.update(a_screensaverTimeout, g_screensaverTimeout);
  if (g_screensaverTimeout == 0) {
    LCDML.SCREEN_disable();
  }
  else {
    LCDML.SCREEN_enable(mFunc_screensaver, g_screensaverTimeout * 1000);
  }

}
