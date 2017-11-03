/****
 * Defines the Abstract Functional and Visual Attributes of a Menu Item used in
 * a Menu in a Window.
****/
#ifndef _CLASS_MENUITEM_H
#define _CLASS_MENUITEM_H

#include "../Util/ScreenPosition.h"
#include "../Util/ScreenRegion.h"
#include "../Graphics/GraphicsObject.h"

class MenuItem {
  public:
    ScreenRegion* region;//   - Region of the Screen Occupied by this MenuItem (clickable region)
    GraphicsObject** shapes;//- GraphicsObjects which Define this MenuItem
    byte num_shapes = 0; ///  - Number of Shapes in this MenuItem

    void (*action)();//       - Action to Perform when Clicked (externally settable)

    // Constructs an Null/Empty ScreenRegion
    MenuItem(void){ };

    // Draws this MenuItem
    void draw(){
      for(int i=0; i<num_shapes; i++){
        shapes[i]->draw();
      }
    }

    // Updates the Screen Region Containing this Menu Item if Touched.
    virtual void update(ScreenPosition& touch_point){
      if(this->region->contains(touch_point)){
        // Override to do Other Things for Certain Items (like highlight button)
        this->action();
      }
    }
};

#endif //_CLASS_MENUITEM_H
