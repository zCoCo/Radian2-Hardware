/*
 Variables and Functions Necessary for Setting Up the R2BB8 Display Hardware
 Abstraction Layer
*/
#ifndef _HAL_H
#define _HAL_H

#include "Arduino.h"

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

 // #### #### ####
// ESP32 pins used for the parallel interface TFT
#define TFT_CS   33  // Chip select control pin
#define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
#define TFT_RST  32  // Reset pin

#define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
#define TFT_RD    2

#define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
#define TFT_D1   13  // so a single register write sets/clears all bits
#define TFT_D2   26
#define TFT_D3   25
#define TFT_D4   17
#define TFT_D5   16
#define TFT_D6   27
#define TFT_D7   14

#include "ILI9341.h"

Display display = new ILI9341(
  TFT_CS, TFT_DC, TFT_WR, TFT_RD, TFT_RST,
  XP, YP, XM, YM, 300, // 300-Ohms Across X-Plate (~500 across Y),
  TS_MINX, TS_MINY, TS_MAXX, TS_MAXY
);

/*
 Method for Initializing HAL (to be called during setup) and Sets it Up for Use.
*/
void init_HAL(){

  display.init();

  // HAL.tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  // HAL.tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  // HAL.tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  // HAL.tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  // HAL.tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  // HAL.tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  //
  // HAL.tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
}

#endif //HAL_H
