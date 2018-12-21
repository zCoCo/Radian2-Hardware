/****
 * Defines the Abstract Functional and Visual Attributes of a Page Item used in
 * a Page.
****/
#ifndef _CLASS_PAGEITEM_H
#define _CLASS_PAGEITEM_H

#include "../Util/ScreenPosition.h"
#include "../Util/ScreenRegion.h"
#include "../Graphics/GraphicsObject.h"

#define _PAGEITEM_MAX_NUM_SHAPES 4

typedef (*Action)();

class PageItem{
public:
  const ScreenRegion* clickableRegion; // Region of Screen which Will Generate a Click for this Object
  const GraphicsObject* shapes[_PAGEITEM_MAX_NUM_SHAPES];
  const byte num_shapes; // Number of Shapes in this PageItem

  bool being_touched = 0; // Whether this Item is Currently being Touched
  Action action;//       - Action to Perform when Clicked (externally settable)

  // Constructs a Null/Empty PageItem:
  PageItem(void){ };

  ~PageItem(void){
    delete* clickableRegion;
    delete[] shapes;
  } // dtor

  void draw(){
    for(byte i=0; i<num_shapes; i++){
      (shapes[i])->draw();
    }
  } // #draw

  // Updates the Screen Region Containing this PageItem if Touched.
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

} // Class PageItem

#endif // _CLASS_PAGEITEM_H
