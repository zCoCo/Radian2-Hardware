/****
 * Defines the Abstract Functional and Visual Attributes of a Page Item used in
 * a Page which constains self-defined visual attributes which can be drawn
 * (that is, it can be drawn from an unchanging list of basic shapes).
****/
#ifndef _CLASS_PAGEITEM_H
#define _CLASS_PAGEITEM_H

#include "PageItem.h"

#include "../Graphics/GraphicsObject.h"

#define _PAGEITEM_MAX_NUM_SHAPES 4

class DrawablePageItem : public PageItem{
public:
  const GraphicsObject* shapes[_PAGEITEM_MAX_NUM_SHAPES];
  const byte num_shapes; // Number of Shapes in this PageItem

  // Constructs a Null/Empty PageItem:
  DrawablePageItem(){ };

  ~DrawablePageItem(){
    delete* clickableRegion;
    delete[] shapes;
  } // dtor

  void draw(){
    for(byte i=0; i<num_shapes; i++){
      (shapes[i])->draw();
    }
  } // #draw

} // Class DrawablePageItem

#endif // _CLASS_PAGEITEM_H
