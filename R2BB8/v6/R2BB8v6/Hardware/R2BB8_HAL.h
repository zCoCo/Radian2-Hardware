/*
 HAL Class Defining the Parts and Functions of the R2BB8 board.
*/
#ifndef _CLASS_R2BB8_HAL_H
#define _CLASS_R2BB8_HAL_H


#include <Adafruit_TFTLCD.h> // For Communicating with the ILI9341
#include <TouchScreen.h> // For Reading Resistive Touchscreen Data

#define MINPRESSURE 10
#define MAXPRESSURE 1000

class R2BB8_HAL{
  public:
    Adafruit_TFTLCD tft;
    TouchScreen ts;

    byte NUM_GRID_X = 4;//    - Number of Grid Cells in X Direction
    float NUM_GRID_Y = 3.5;// - Number of Grid Cells in Y Direction
    float GRID_WIDTH;//       - Width of A Grid Cell
    float GRID_HEIGHT;//      - Height of A Grid Cell

    R2BB8_HAL(Adafruit_TFTLCD* tft_, TouchScreen* ts_) : tft(*tft_), ts(*ts_){
      GRID_WIDTH = float(tft.width()) / float(NUM_GRID_X);
      GRID_HEIGHT = float(tft.height()) / float(NUM_GRID_Y);
    }
    ~R2BB8_HAL(){
      delete &tft;
      delete &ts;
    }

    // Initializes the Hardware Interface.
    void    init();

    // Erases the Display
    void    erase();

  private:

};

#include "R2BB8_HAL.cpp" // this is required for the compiler to realize the cpp exists.
#endif //_CLASS_R2BB8_HAL_H
