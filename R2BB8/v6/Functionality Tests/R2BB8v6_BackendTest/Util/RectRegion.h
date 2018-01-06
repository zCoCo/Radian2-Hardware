/****
 * Defines a Rectangular Region on the Screen
****/
#ifndef _CLASS_RECTREGION_H
#define _CLASS_RECTREGION_H

#include "ScreenRegion.h"
#include "ScreenPosition.h"

class RectRegion : public ScreenRegion{
  public:
    ScreenPosition upperLeft;
    int16_t width;
    int16_t height;

    // Constructs an Null/Empty Rectangular Region
    RectRegion(void)
    : upperLeft(0,0)
    {
      width = 0;
      height = 0;
    }

    // Constructs a Rectangular Region of the Screen from the Position of its
    // UpperLeft Corner, width and height.
    RectRegion(int16_t x_ul, int16_t y_ul, int16_t w, int16_t h)
    : upperLeft(x_ul, y_ul)
    {
      width = w;
      height = h;
    }

    // Constructs a Region of the Screen from the Position of its Center Position,
    // width and height.
    static RectRegion* constructFromCenter(int16_t xc, int16_t yc, int16_t w, int16_t h){
      return new RectRegion((xc - w/2), (yc - h/2), w, h);
    }
    static RectRegion* constructFromCenter(ScreenPosition& cp, int16_t w, int16_t h){
      return new RectRegion((cp.x - w/2), (cp.y - h/2), w, h);
    }

    // Returns the Position of the Center of this Region.
    ScreenPosition* centerPosition(){
      return new ScreenPosition(upperLeft.x + width/2, upperLeft.y + height/2);
    }

    // Translate the Given Screen Region by Dx, Dy
    virtual void translate(int16_t Dx, int16_t Dy){
      upperLeft = ScreenPosition(upperLeft.x+Dx, upperLeft.y+Dy);
    }

    using ScreenRegion::contains;

    // Returns Whether the given Point (x,y) is Contained within this Region.
    bool contains(int16_t x, int16_t y){
      return ( containsX(x) && containsY(y) );
    }

    // Returns Whether the given X-Position is Contained within this Region.
    bool containsX(int16_t x){
      return ( (x >= upperLeft.x) && (x <= (upperLeft.x+width)) );
    }
    // Returns Whether the given Y-Position is Contained within this Region.
    bool containsY(int16_t y){
      return ( (y >= upperLeft.y) && (y <= (upperLeft.y+height)) );
    }

    bool operator==(RectRegion& r1){
      return  ( (r1.upperLeft == upperLeft) &&
                (r1.width == width) &&
                (r1.height == height) );
    }
    bool operator!=(RectRegion& r1){
      return  ( (r1.upperLeft != upperLeft) ||
                (r1.width != width) ||
                (r1.height != height) );
    }
};


#endif //_CLASS_RECTREGION_H
