/*
 * Implements a Specific Driver from the Display Class, with Hardware and
 * Libraries for the ILI9341 Display Unit from Kingtech.
 */

#ifndef _CLASS_ILI9341_H
#define _CLASS_ILI9341_H

#include "Display.h"

#include <TouchScreen.h> // For Reading Data from the Resistive Touch Panel
#include <Adafruit_GFX.h>    // Adafruit Core graphics library

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// See SetupX_Template.h for all options available
#ifdef USING_ESP32
  #include <TFT_eSPI.h> // For Communicating with the ILI9341

  #define ESP32_PARALLEL

  #define ILI9341_DRIVER

  //#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
  #define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
  //#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
  //#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
  //#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
  //#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
  #define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

  #define SMOOTH_FONT
#else
  #include <Adafruit_TFTLCD.h> // For Communicating with the ILI9341
#endif // USING_ESP32

class ILI9341 : public Display{
    TFT_eSPI tft;
    TouchScreen ts;

    // These pins might be multiplexed (used for both input and output signals)
    // and switched for different purposes; so, the driver must be able to
    // change their I/O state:
    uint8_t xm;
    uint8_t yp;

    // Calibrated Boundary Positions of Touch Panel:
    uint8_t ts_minX;
    uint8_t ts_minY;
    uint8_t ts_maxX;
    uint8_t ts_maxY;

    ILI9341(
      lcd_cs_, lcd_cd_, lcd_wr_, lcd_rd_, lcd_rst_, // unused for the given library but maintained for future compatibility
      ts_xp_, ts_yp_, ts_xm_, ts_ym_, ts_resistX_,
      ts_minX_, ts_minY_, ts_maxX_, ts_maxY_
    ) :
    #ifdef USING_ESP32
    tft(),
    #else
    tft(lcd_cs_, lcd_cd_, lcd_wr_, lcd_rd_, lcd_rst_),
    #endif // USING_ESP32
    ts(ts_xp_, ts_yp_, ts_xm_, ts_ym_, ts_resistX_),
    xm(ts_xm_), yp(ts_yp_),
    ts_minX(ts_minX_), ts_minY(ts_minY_), ts_maxX(ts_maxX_), ts_maxY(ts_maxY_)
    {
      // tft.setRotation(1); // Set Rotation Here

      this->width = tft.width();
      this->height = tft.height();
    }

  private:
    // nothing
} // Class ILI9341

#include "ILI9341.cpp" // this is required for the compiler to realize the cpp exists.
#endif
