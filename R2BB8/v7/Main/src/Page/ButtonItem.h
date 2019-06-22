/****
 * Defines a Standard (themed) Interactive Button to Serve as a PageItem
****/
#ifndef _CLASS_BUTTONITEM_H
#define _CLASS_BUTTONITEM_H

#include "DrawablePageItem.h"
#include "../Graphics/Colors.h"
#include "../Graphics/GO_Rectangle.h"

class ButtonItem : public DrawablePageItem{
  public:
    // Constructs a Standard Interactive Button which Fills the Given Parent
    // Container.
    ButtonItem(ItemContainer* ic){
      this->parent = ic;
      this->region = this->parent->container_region;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, NO_COLOR, BLUE)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));
    };
    // Constructs a Standard Interactive Button which Fills the Given Parent
    // Container, with the special outline color oc, and fill color fc.
    ButtonItem(ItemContainer* ic, Color oc, Color fc){
      this->parent = ic;
      this->region = this->parent->container_region;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, oc, fc)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));
    };
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
    ButtonItem(RectRegion* reg_, Color fc){
      this->region = reg_;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, NO_COLOR, fc)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));
    };
    // Constructs a Interactive Button in the Given Region with special outline
    // color oc, and fill color fc.
    ButtonItem(RectRegion* reg_, Color oc, Color fc){
      this->region = reg_;
      GraphicsObject* shps[] = {
        new GO_Rectangle(reg_, oc, fc)
      };
      this->num_shapes = 1;
      memcpy(this->shapes, shps, sizeof(shps));
    };

    // Updates the Screen Region Containing this Menu Item if Touched.
    void update(ScreenPosition* touch_point){
      // Perform Base Update:
      PageItem::update(touch_point);

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
