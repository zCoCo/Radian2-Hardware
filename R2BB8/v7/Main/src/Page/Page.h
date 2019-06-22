#ifndef _CLASS_PAGE_H
#define _CLASS_PAGE_H

#include "../Util/ScreenPosition.h"
#include "PageItem.h"
#include "ItemContainer.h"

#include "../Hardware/HAL.h"

class Page : public ItemContainer{
public:
  PageItem** items;
  const uint8_t num_items;

  Page(const uint8_t n) : num_items(n){ // Items List Must Be Populated after Ctor b/c Filled Items Require Parent Ptr
    this->num_items = n;
    this->items = new PageItem[this->num_items];
    // Define ItemContainer Boundary Region of Page:
    this->container_region = new RectRegion(0,0, display->width, display->height);

    // Tell Each Item who their Parent is:
    for(uint8_t i=0; i<this->num_items; i++){
      (this->items[i])->parent = static_cast<ItemContainer*>(this);
    }
  } // ctor

  ~Page(){
    for(uint8_t i=0; i<this->num_items; i++){
      delete this->items[i];
    }
    delete [] this->items;
  } // dtor

  void draw(){
    for(uint8_t i=0; i<this->num_items; i++){
      (this->items[i])->draw();
    }
  } // #draw

  void registerTouch(ScreenPosition* tp){
    for(uint8_t i=0; i<this->num_items; i++){
      if( this->items[i]->contains(tp) ){
        (this->items[i])->action();
      }
    }
  } // #registerTouch

private:

}; // Class Page

#endif // _CLASS_PAGE_H
