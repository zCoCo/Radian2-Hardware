/*
 Variables and Functions Necessary for Setting Up the R2BB8 Display Hardware
 Abstraction Layer
*/
#ifndef _HAL_H
#define _HAL_H

#include "Arduino.h"
#include <Adafruit_TFTLCD.h> // For Communicating with the ILI9341
#include <TouchScreen.h> // For Reading Resistive Touchscreen Data
#include "../Hardware/R2BB8_HAL.h"

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A7

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts(XP, YP, XM, YM, 300); // 300-Ohms Across X-Plate (~500 across Y)
R2BB8_HAL HAL(&tft, &ts);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

/*
 Method for Initializing HAL (to be called during setup) and Sets it Up for Use.
*/
void init_HAL(){

  delay(500); // -- DISPLAY DOESN'T WORK IF NOT GIVEN TIME TO INITIALIZE W/PWR BEFORE TALKING.
  
  HAL.init();
  HAL.erase();

  HAL.tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  HAL.tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  HAL.tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  HAL.tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  HAL.tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  HAL.tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);

  HAL.tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;
}


#endif //HAL_H
