/****
 * Defines Abstract Features of Any Region on the Screen
****/
#ifndef _CLASS_REGION_H
#define _CLASS_REGION_H


class Region {
  public:
    // Constructs an Null/Empty ScreenRegion
    ScreenRegion(void){ };

    // Returns the Position of the Center of this Region.
    virtual ScreenPosition* centerPosition() = 0;

    // Returns Whether the given ScreenPoint is Contained within this Region.
    bool contains(ScreenPosition& p){
      return contains(p.x, p.y);
    }
    // Returns Whether the given Point (x,y) is Contained within this Region.
    virtual bool contains(int16_t x, int16_t y) = 0;
    // Returns Whether the given X-Position is Contained within this Region.
    virtual bool containsX(int16_t x) = 0;
    // Returns Whether the given Y-Position is Contained within this Region.
    virtual bool containsY(int16_t y) = 0;

    virtual bool operator==(ScreenRegion& r1) = 0;
    virtual bool operator!=(ScreenRegion& r1) = 0;
};


#endif //_CLASS_EGION_H
