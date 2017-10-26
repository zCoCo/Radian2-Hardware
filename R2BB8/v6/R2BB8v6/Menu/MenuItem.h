/****
 * Defines the Abstract Functional and Visual Attributes of a Menu Item used in
 * a Menu in a Window.
****/
#ifndef _CLASS_MENUITEM_H
#define _CLASS_MENUITEM_H


class MenuItem {
  public:
    ScreenRegion* region;//   - Region of the Screen Occupied by this MenuItem
    GraphicsObject[] shapes*;// -GraphicsObjects which Define this MenuItem

    void (*action)();//       - Action to Perform when Clicked (externally settable)

    ////// TODO TODO TODO:
    /*
      - MenuItem CenterPosition will be defined by its ScreenRegion.
      - Create GraphicsObject Abs which Contains a Region and a draw function and a color or some other parameters like fill and outline
    */

    // Constructs an Null/Empty ScreenRegion
    MenuItem(void){ };

    // Draws this MenuItem
    void draw(){
      for(auto& shape : shapes){
        shape->draw();
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
