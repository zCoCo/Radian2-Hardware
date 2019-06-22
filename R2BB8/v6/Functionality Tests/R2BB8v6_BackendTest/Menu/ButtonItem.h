/****
 * Defines a Standard (themed) Interactive Button to Serve as a MenuItem
****/
#ifndef _CLASS_BUTTONITEM_H
#define _CLASS_BUTTONITEM_H

#include "MenuItem.h"
#include "../Graphics/Colors.h"
#include "../Graphics/GO_Rectangle.h"

class ButtonItem : public MenuItem{
  public:
    void (*action)();//       - Action to Perform when Clicked (externally settable)

    // Constructs a Standard Interactive Button in the Given Region
    ButtonItem(RectRegion* reg_){
      this->region = reg_;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, NO_COLOR, BLUE)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));

    };
    // Constructs a Interactive Button in the Given Region with special fill
    // color fc.
    ButtonItem(RectRegion* reg_, uint16_t fc){
      this->region = reg_;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, NO_COLOR, fc)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));
    };

    // Updates the Screen Region Containing this Menu Item if Touched.
    void update(ScreenPosition& touch_point){
      if(this->region->contains(touch_point)){
        // Toggle Outline Color When Clicked:
        if(this->shapes[0]->color_outline == NO_COLOR){
          this->shapes[0]->color_outline = WHITE;
        } else{
          this->shapes[0]->color_outline = NO_COLOR;
        }

        // Perform Associated Action:
        this->action();
      }
    }
};

#endif //_CLASS_BUTTONITEM_H
