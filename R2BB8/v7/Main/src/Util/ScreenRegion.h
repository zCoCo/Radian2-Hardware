/****
 * Defines Abstract Features of Any Region on the Screen
****/
#ifndef _CLASS_SCREENREGION_H
#define _CLASS_SCREENREGION_H

#include "ScreenPosition.h"
#include "RectRegion.h"

class ScreenRegion {
  public:
    RectRegion* bounding_box; // Bounding Box of the Shape of this Screen Region

    // Constructs an Null/Empty ScreenRegion
    ScreenRegion(void){ };

    virtual ~ScreenRegion(void){
      delete this->bounding_box;
    } // dtor

    // Returns the Position of the Center of this Region.
    const ScreenPosition centerPosition(){
      return ScreenPosition(
        this->bounding_box->upperLeft.x + this->bounding_box->width/2,
        this->bounding_box->upperLeft.y + this->bounding_box->height/2
      );
    } // #centerPosition

    // Translate the Given Screen Region by Dx, Dy
    virtual void translate(int16_t Dx, int16_t Dy) = 0;
      // Translate the Given Screen Region by the Amount Stored in Dp
    void translate(ScreenPosition* Dp){
      this->translate(Dp->x,Dp->y);
    }

    // Returns Whether the given ScreenPosition is Contained within this Region.
    const bool contains(ScreenPosition* p){
      return contains(p->x, p->y);
    }

    // Returns Whether the given Point (x,y) is Contained within this Region.
    virtual const bool contains(int16_t x, int16_t y) = 0;
    // Returns Whether the given X-Position is Contained within this Region.
    virtual const bool containsX(int16_t x) = 0;
    // Returns Whether the given Y-Position is Contained within this Region.
    virtual const bool containsY(int16_t y) = 0;

    // Returns the Width of the Bounding Box of This Shape:
    const uint16_t width(){ return this->bounding_box->width; }
    // Returns the Height of the Bounding Box of This Shape:
    const uint16_t height(){ return this->bounding_box->height; }
};


#endif //_CLASS_SCREENREGION_H
