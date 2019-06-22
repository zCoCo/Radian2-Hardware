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
    void update(ScreenPosition* touch_point){
      // Perform Base Update:
      MenuItem::update(touch_point);

      // Toggle Outline Color While Being Clicked:
      if(this->being_touched){

        if(this->shapes[0]->color_outline != WHITE){
          // Touched for First Time in Series
          this->shapes[0]->color_outline = WHITE;
          this->draw(); // Send One Draw Update to Make this Change
        }

      } else{

        if(this->shapes[0]->color_outline != NO_COLOR){
          // Touched for First Time in Series
          this->shapes[0]->color_outline = NO_COLOR;
          this->draw(); // Send One Draw Update to Make this Change
        }

      } // being_touched?
    } // #update
};

#endif //_CLASS_BUTTONITEM_H
