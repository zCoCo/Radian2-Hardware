/****
 * Defines a Grid of PageItems
****/
#ifndef _CLASS_GRID_H
#define _CLASS_GRID_H

#include "PageItem.h"
#include "../Graphics/Colors.h"
#include "../Graphics/GO_Rectangle.h"

class Grid : public PageItem{
  public:
    GridCell** cells;
    uint8_t n_horiz; // Number of Horizontal PageItems
    uint8_t n_vert; // Number of Vertical PageItems

    // Constructs a Grid which Fills the Given Parent Container with the Given
    // Number of Horizontal and Vertical Cells.
    Grid(ItemContainer* ic, byte nh, byte nv) : Grid(ic->container_region, nh,nv){
      this->parent = ic;
    } // ctor
    // Constructs a Grid Occupying the Given ScreenRegion with the Given Number
    // of Horizontal and Vertical Cells.
    Grid(RectRegion* reg_, byte nh, byte nv){
      this->region = reg_;
      this->n_horiz = nh;
      this->n_vert = nv;
      this->cells = new GridCell*[nh*nv];
      // Generate GridCells:
      for(int i=0; i<nh*nv; i++){
        this->cells[i] = new GridCell(this, i);
      }
    } // ctor

    ~Grid(){
      for (int i=0; i<nh*nv; i++){
        delete this->cells[i];
      }
      delete[] this->cells;
    } // dtor

    void setItems(PageItem** _items){
      this->items = _items;
      // TODO: Loop through each item and add parent.
    } // #setItems

    void draw(){
      // **TODO**
    }

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
  private:
    //Don't make this. Better to pass in completed array (allocated as one big block of memory) to minimize heap fragmentation than to have a #setItem function (since, hypothetically, these objects could be deleted if large menu structures are loaded and unloaded from SPIFFS)).
    void setItem(){ /* Don't do it. */ }
};

#endif //_CLASS_GRID_H
