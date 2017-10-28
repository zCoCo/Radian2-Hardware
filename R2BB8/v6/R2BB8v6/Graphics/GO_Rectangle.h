/****
 * Defines a Rectangle Graphics Object.
****/
#ifndef _CLASS_GO_RECTANGLE_H
#define _CLASS_GO_RECTANGLE_H

#include "GraphicsObject.h"
#include "../Hardware/HAL.h"
#include "Colors.h"


class GO_Rectangle : public GraphicsObject{
  public:
    // Constructs a Rectangle Graphics Object with Shape and Position defined by
    // the Given RectRegion, Outline defined by co, and Fill Color by cf.
    GO_Rectangle(RectRegion* s, uint16_t co, uint16_t cf){
      this->shape = s;
      this->color_outline = co;
      this->color_fill = cf;
    }

    // Draw the Graphics Object onto the Screen
    void draw(){
      if(color_fill != NO_COLOR){
        HAL.tft.fillRect(
          shape->upperLeft.x, shape->upperLeft.y,
          shape->width, shape->height,
          color_fill
        );
      }
      if(color_outline != NO_COLOR){
        HAL.tft.drawRect(
          shape->upperLeft.x, shape->upperLeft.y,
          shape->width, shape->height,
          color_outline
        );
      }
    }
};


#endif //_CLASS_GO_RECTANGLE_H
