/*
 HAL Class Defining the Parts and Functions of the R2BB8 board.
*/

#include "Arduino.h"
#include "R2BB8_HAL.h"

#include "HAL.h"

void R2BB8_HAL::init(void){
  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341"));
  } else {
    Serial.print(F("Unknown LCD driver: "));
    Serial.println(identifier, HEX);
    return;
  }

  tft.begin(identifier);
}

void R2BB8_HAL::erase(void){
  tft.fillScreen(BLACK);
}

// Erases a Region of the Display with Upper Left Corner (x_ul,y_ul),
// Width w and Height h.
void R2BB8_HAL::eraseRegion(int16_t x_ul, int16_t y_ul,  int16_t w, int16_t h){
  tft.fillRect(x_ul, y_ul, w, h, BLACK);
}
// Erases the Given Region of the Display
void R2BB8_HAL::eraseRegion(ScreenRegion reg){
  R2BB8_HAL::eraseRegion(reg.upperLeft.x, reg.upperLeft.y, reg.width, reg.height);
}

ScreenPosition R2BB8_HAL::touchPosition(void){
  TSPoint p = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if(p.z > MINPRESSURE && p.z < MAXPRESSURE){
    // scale from ~0->1023 to tft size
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    return ScreenPosition(p.x,p.y);
  } else{
    return R2BB8_HAL::NotTouching;  // Touch is Invalid if insufficient or too
                                   //  much Pressure
  }
}
