/*
 HAL Class Defining the Parts and Functions of the R2BB8 board.
*/
#ifndef _CLASS_R2BB8_HAL_H
#define _CLASS_R2BB8_HAL_H


#include <Adafruit_TFTLCD.h> // For Communicating with the ILI9341
#include <TouchScreen.h> // For Reading Resistive Touchscreen Data

#include "../Util/ScreenPosition.h"
#include "../Util/RectRegion.h"

#define MINPRESSURE 10
#define MAXPRESSURE 1000

class R2BB8_HAL{
  public:
    Adafruit_TFTLCD tft;
    TouchScreen ts;

    // Store these so this hardware data can be changed without changing all
    // code using this data.
    int16_t WIDTH;//           - Width of Screen
    int16_t HEIGHT;//          - Height of Screen

    byte NUM_GRID_X = 4;//    - Number of Grid Cells in X Direction
    float NUM_GRID_Y = 3.5;// - Number of Grid Cells in Y Direction
    float GRID_WIDTH;//       - Width of A Grid Cell
    float GRID_HEIGHT;//      - Height of A Grid Cell

    // ScreenPosition Returned when No Touch is Registered by the Screen or there
    // is an Invalid Position.
    ScreenPosition NotTouching = ScreenPosition(-1,-1);

    R2BB8_HAL(Adafruit_TFTLCD* tft_, TouchScreen* ts_) : tft(*tft_), ts(*ts_){
      // tft.setRotation(1); // Set Rotation Here
      WIDTH = tft.width();
      HEIGHT = tft.height();
      GRID_WIDTH = float(tft.width()) / float(NUM_GRID_X);
      GRID_HEIGHT = float(tft.height()) / float(NUM_GRID_Y);
    }
    ~R2BB8_HAL(){
      delete &tft;
      delete &ts;
    }

    // Initializes the Hardware Interface.
    void              init();

    // Erases the Display
    void              erase();
    // Erases a Region of the Display with Upper Left Corner (x_ul,y_ul),
    // Width w and Height h.
    void              eraseRegion(int16_t x_ul, int16_t y_ul,  int16_t w, int16_t h);
    // Erases the Given Region of the Display
    void              eraseRegion(RectRegion* reg);

    // Returns the Screen Position of the Current Touch Point
    ScreenPosition*    touchPosition();

  private:

};

#include "R2BB8_HAL.cpp" // this is required for the compiler to realize the cpp exists.
#endif //_CLASS_R2BB8_HAL_H
