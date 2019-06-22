/****
 * Defines the Abstract Functional and Visual Attributes of a Menu Item used in
 * a Menu in a Window.
****/
#ifndef _CLASS_MENUITEM_H
#define _CLASS_MENUITEM_H

#include "../Util/ScreenPosition.h"
#include "../Util/ScreenRegion.h"
#include "../Graphics/GraphicsObject.h"

#define _MENUITEM_MAX_NUM_SHAPES 4

class MenuItem {
  public:
    ScreenRegion* region;//   - Region of the Screen Occupied by this MenuItem (clickable region)
    // GraphicsObjects which Define this MenuItem:
    GraphicsObject* shapes[_MENUITEM_MAX_NUM_SHAPES];
    byte num_shapes = 0; ///  - Number of Shapes in this MenuItem

    bool being_touched = 0; // Whether this Item is Currently Being Touched

    void (*action)();//       - Action to Perform when Clicked (externally settable)

    // Constructs an Null/Empty ScreenRegion
    MenuItem(void){ };

    // Draws this MenuItem
    void draw(){
                                                                                Serial.println("MI Draw.");
      for(int i=0; i<num_shapes; i++){
                                                                                Serial.print("> Shape-");
                                                                                Serial.print(i);
                                                                                Serial.print(" of -");
                                                                                Serial.println(num_shapes);
                                                                                Serial.println("====");
                                                                                Serial.println( (shapes[i])->color_fill );
        (shapes[i])->draw();
      }
    } // #draw

    // Updates the Screen Region Containing this Menu Item if Touched.
    virtual void update(ScreenPosition* touch_point){
    // Override to do Other Things for Certain Items (like highlight button)

      if(this->region->contains(touch_point)){

        if(!being_touched){
          this->being_touched = 1;

          this->action();
        }

      } else{
        this->being_touched = 0;
      }
    } // #update
};

#endif //_CLASS_MENUITEM_H
