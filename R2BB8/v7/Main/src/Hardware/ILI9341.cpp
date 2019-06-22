/*
 HAL Class Defining the Parts and Functions of the R2BB8 board.
*/

#include "Arduino.h"
#include "ILI9341.h"
#include "../Graphics/Colors.h"
#include "../Util/StringUtils.h"

void ILI9341::init(void){
  delay(500); // -- DISPLAY DOESN'T WORK IF NOT GIVEN TIME TO INITIALIZE W/PWR BEFORE TALKING.

  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier == 0x9341) {
    Serial.println(SS("Found ILI9341"));
  } else {
    Serial.print(SS("Unknown LCD driver: "));
    Serial.println(identifier, HEX);
    return;
  }

  tft.begin(identifier);

  this->erase();
} // #init

void ILI9341::erase(void){
  tft.fillScreen(BGCOLOR);
} // #erase

// Erases a Region of the Display with Upper Left Corner (x_ul,y_ul),
// Width w and Height h.
void ILI9341::eraseRegion(int16_t x_ul, int16_t y_ul,  int16_t w, int16_t h){
  tft.fillRect(x_ul, y_ul, w, h, BGCOLOR);
} // #eraseRegion
// Erases the Given Region of the Display
void ILI9341::eraseRegion(RectRegion* reg){
  ILI9341::eraseRegion(reg->upperLeft.x, reg->upperLeft.y, reg->width, reg->height);
} // #eraseRegion

ScreenPosition ILI9341::touchPosition(void){
  //digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  //digitalWrite(13,LOW);

  pinMode(xm, OUTPUT);
  pinMode(yp, OUTPUT);

  if(p.z > MINPRESSURE && p.z < MAXPRESSURE){
    // scale from ~0->1023 to tft size
    p.x = map(p.x, ts_minX, ts_maxX, tft.width(), 0);
    p.y = map(p.y, ts_minY, ts_maxY, tft.height(), 0);

    return ScreenPosition(p.x,p.y);
  } else{
    return &NotTouching;  // Touch is Invalid if insufficient or too
                          // much Pressure
  }
} // #touchPosition

// Returns Whether the Display is Being Touched
bool ILI9341::isTouching(){
  return ts.isTouching();
} // #isTouching

// Draws a Filled Rectangle with Upper-Left Corner at (x0,y0) with Dimensions (w,h):
void ILI9341::fillRect(uint16_t x0, int16_t y0, int16_t w, int16_t h, Color c){
  tft.fillRect(x0,y0, w,h, c);
} // #fillRect

// Draws the Outline of a Rectangle with Upper-Left Corner at (x0,y0) with Dimensions (w,h):
void ILI9341::drawRect(uint16_t x0, int16_t y0, int16_t w, int16_t h, Color c){
  tft.drawRect(x0,y0, w,h, c);
} // #drawRect


// Draws a Pixel at the Given Location (x,y) in Color c:
void  ILI9341::drawPixel(int16_t x, int16_t y, Color c){
  tft.drawPixel(x,y, c);
} // #drawPixel

// Returns the Color of the Pixel at the Given Location (x,y):
Color ILI9341::readPixel(int16_t x, int16_t y){
  return tft.readPixel(x,y);
} // #readPixel

// Fills the Screen with One Color:
void  ILI9341::fillScreen(Color c){
  tft.fillScreen(c);
} // #fillScreen

// Draws the Outline of a Rounded Rectangle with an Upper-Left Corner at
// (x0,y0), Dimensions (w,h), Corner Radius r, and Color c
void  ILI9341::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t r, Color c){
  tft.drawRoundRect(x0,y0, w,h, r, c);
} // #drawRoundRect
// Draws a Filled of a Rounded Rectangle with an Upper-Left Corner at
// (x0,y0), Dimensions (w,h), Corner Radius r, and Color c
void  ILI9341::fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t r, Color c){
  tft.fillRoundRect(x0,y0, w,h, r, c);
} // #fillRoundRect

// Draws the Outline of a Circle Centered at (x0,y0) with Radius r, Color c
void  ILI9341::drawCircle(int16_t x0, int16_t y0, int16_t r, Color c){
  tft.drawCircle(x0,y0, r, c);
} // #drawCircle
// Draws a Filled of a Circle Centered at (x0,y0) with Radius r, Color c
void  ILI9341::fillCircle(int16_t x0, int16_t y0, int16_t r, Color c){
  tft.fillCircle(x0,y0, r, c);
} // #fillCircle

// Draws the Outline of a Triangle with the Given (x,y) Vertices of Color c
void  ILI9341::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, Color c){
  tft.drawTriangle(x0,y0, x1,y1, x2,y2, c);
} // #drawTriangle
// Draws a Filled of a Triangle with the Given (x,y) Vertices of Color c
void  ILI9341::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, Color c){
  tft.fillTriangle(x0,y0, x1,y1, x2,y2, c);
} // #fillTriangle
