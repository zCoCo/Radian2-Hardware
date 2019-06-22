/****
 * Defines the Abstract Functional Attributes of an Item used in a Page.
****/
#ifndef _CLASS_PAGEITEM_H
#define _CLASS_PAGEITEM_H

#include "../Util/ScreenPosition.h"
#include "../Util/ScreenRegion.h"

#include "ItemContainer.h"

typedef void (*Action)(void);

class PageItem{
public:
  const ScreenRegion* clickableRegion; // Region of Screen which Will Generate a Click for this Object

  bool being_touched = 0; //  - Whether this Item is Currently being Touched
  Action action;//            - Action to Perform when Clicked (externally settable)

  ItemContainer* parent = &ItemContainer::NULL_CONTAINER; // Default Container. Is fine to use this since all functions are called by parent.

  // Constructs a Null/Empty PageItem:
  PageItem(){ };

  virtual ~PageItem(){ }; // Ensure Proper Destruction Behaviour of Derived Classes

  virtual void draw() = 0;

  // Updates the Screen Region Containing this PageItem if Touched.
  virtual void update(ScreenPosition* touch_point){
    // Override to do Other Things for Certain Items (like highlight button)

    if(this->clickableRegion->contains(touch_point)){

      if(!being_touched){
        this->being_touched = 1;

        this->action();
      }

    } else{
      this->being_touched = 0;
    }

  } // #update

}; // Class PageItem

#endif // _CLASS_PAGEITEM_H
