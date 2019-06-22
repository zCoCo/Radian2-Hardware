/*
 HAL Class Defining the Control and Functions for the Display Unit on the Board.
 Contains All Abstract Functionality which Can be Expected from Any Display Unit
 throughout the Rest of the Application. Allows for Future Upgrading of the
 Display without Changes to the Rest of the Code.
*/
#ifndef _CLASS_DISPLAY_H
#define _CLASS_DISPLAY_H

#include "../Util/ScreenPosition.h"
#include "../Util/RectRegion.h"
#include "../Graphics/Colors.h"

class Display{
  public:
    // Store these so this hardware data can be changed without changing all
    // code using this data.
    int16_t width;//           - Width of Screen
    int16_t height;//          - Height of Screen

    // ScreenPosition Returned when No Touch is Registered by the Screen or there
    // is an Invalid Position.
    ScreenPosition NotTouching = ScreenPosition(-1,-1);

    // BASIC FUNCTIONALITY:

    // Initializes the Hardware Interface:
    virtual void            init() = 0;

    // Erases the Entire Display:
    virtual void            erase() = 0;
    // Erases a Region of the Display with Upper Left Corner (x_ul,y_ul),
    // Width w and Height h.
    virtual void            eraseRegion(int16_t x_ul, int16_t y_ul,  int16_t w, int16_t h) = 0;
    // Erases the Given Region of the Display
    virtual void            eraseRegion(RectRegion* reg) = 0;

    // Returns the Screen Position of the Current Touch Point
    virtual ScreenPosition  touchPosition() = 0;

    // Returns Whether the Display is Being Touched
    bool isTouching() = 0;

    // DRAWING FUNCTIONALITY:

    // Draws a Pixel at the Given Location (x,y) in Color c:
    virtual void  drawPixel(int16_t x, int16_t y, Color c) = 0;

    // Returns the Color of the Pixel at the Given Location (x,y):
    virtual Color readPixel(int16_t x, int16_t y) = 0;

    // Fills the Screen with One Color:
    virtual void  fillScreen(Color c) = 0;


    // Draws a Filled Rectangle with Upper-Left Corner at (x0,y0) with Dimensions (w,h):
    virtual void  fillRect(uint16_t x0, int16_t y0, int16_t w, int16_t h, Color c) = 0;
    // Draws the Outline of a Rectangle with Upper-Left Corner at (x0,y0) with Dimensions (w,h):
    virtual void  drawRect(uint16_t x0, int16_t y0, int16_t w, int16_t h, Color c) = 0;

    // Draws the Outline of a Rounded Rectangle with an Upper-Left Corner at
    // (x0,y0), Dimensions (w,h), Corner Radius r, and Color c
    virtual void  drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t r, Color c) = 0;
    // Draws a Filled of a Rounded Rectangle with an Upper-Left Corner at
    // (x0,y0), Dimensions (w,h), Corner Radius r, and Color c
    virtual void  fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t r, Color c) = 0;

    // Draws the Outline of a Circle Centered at (x0,y0) with Radius r, Color c
    virtual void  drawCircle(int16_t x0, int16_t y0, int16_t r, Color c) = 0;
    // Draws a Filled of a Circle Centered at (x0,y0) with Radius r, Color c
    virtual void  fillCircle(int16_t x0, int16_t y0, int16_t r, Color c) = 0;

    // Draws the Outline of a Triangle with the Given (x,y) Vertices of Color c
    virtual void  drawTriangle(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2,
        Color c
    ) = 0;
    // Draws a Filled of a Triangle with the Given (x,y) Vertices of Color c
    virtual void  fillTriangle(
      int16_t x0, int16_t y0,
      int16_t x1, int16_t y1,
      int16_t x2, int16_t y2,
      Color c
    ) = 0;

  private:
    Display(void){ /* Hidden */ }
};

#endif //_CLASS_DISPLAY_H
