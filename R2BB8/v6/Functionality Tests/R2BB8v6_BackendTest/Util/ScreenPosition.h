/****
 * Defines a Position on the Screen
****/
#ifndef _CLASS_SCREENPOSITION_H
#define _CLASS_SCREENPOSITION_H


class ScreenPosition {
  public:
    ScreenPosition(void){
      x = y = 0;
    }
    ScreenPosition(int16_t x_, int16_t y_){
      x = x_;
      y = y_;
    }
    ~ScreenPosition(void){
      delete& x;
      delete& y;
    }

    ScreenPosition operator+(ScreenPosition& p1){
      return ScreenPosition(x+p1.x, y+p1.y);
    }
    bool operator==(ScreenPosition& p1){
      return  ((p1.x == x) && (p1.y == y));
    }
    bool operator!=(ScreenPosition& p1){
      return  ((p1.x != x) || (p1.y != y));
    }

    int16_t x, y;
};


#endif //_CLASS_SCREENPOSITION_H
