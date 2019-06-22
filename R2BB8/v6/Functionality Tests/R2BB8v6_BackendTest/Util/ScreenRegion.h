/****
 * Defines Abstract Features of Any Region on the Screen
****/
#ifndef _CLASS_SCREENREGION_H
#define _CLASS_SCREENREGION_H

#include "ScreenPosition.h"

class ScreenRegion {
  public:
    // Constructs an Null/Empty ScreenRegion
    ScreenRegion(void){ };

    // Returns the Position of the Center of this Region.
    virtual ScreenPosition* centerPosition() = 0;

    // Translate the Given Screen Region by Dx, Dy
    virtual void translate(int16_t Dx, int16_t Dy) = 0;
        // Translate the Given Screen Region by the Amount Stored in Dp
    void translate(ScreenPosition Dp){
      this->translate(Dp.x,Dp.y);
    }

    // Returns Whether the given ScreenPoint is Contained within this Region.
    bool contains(ScreenPosition& p){
      return contains(p.x, p.y);
    }
    bool contains(ScreenPosition* p){
      return contains(p->x, p->y);
    }
    // Returns Whether the given Point (x,y) is Contained within this Region.
    virtual bool contains(int16_t x, int16_t y) = 0;
    // Returns Whether the given X-Position is Contained within this Region.
    virtual bool containsX(int16_t x) = 0;
    // Returns Whether the given Y-Position is Contained within this Region.
    virtual bool containsY(int16_t y) = 0;
};


#endif //_CLASS_SCREENREGION_H
