/*
 * A Basic ItemContainer for Cells in a Grid. Allows for Consistent Calling of
 * "parent->width()", etc. for generated code.
 */
 #ifndef _CLASS_GRIDCELL_H
 #define _CLASS_GRIDCELL_H

  #include "Grid.h"
  #include "ItemContainer.h"
  #include "RectRegion.h"

  class GridCell : public ItemContainer{
  public:
    uint8_t index; //       - Linear Index of this Cell within the Grid (store index not r,c to save memory)

    GridCell(Grid* g, uint8_t idx){
      this->index = idx;
      // Determine Position in Grid, 0-indexed:
      uint8_t c = idx % g->clickableRegion->n_horiz - 1; // Row
      uint8_t r = idx / g->clickableRegion->n_horiz; // Column
      this->container_region = new RectRegion(
        c * g->clickableRegion->width() / g->n_horiz,
        r * g->clickableRegion->height() / g->n_vert,
        g->clickableRegion->width() / g->n_horiz,
        g->clickableRegion->height() / g->n_vert
      );
    } // ctor

    ~GridCell(){
      delete contents;
      delete region;
    } // dtor

    uint16_t width(){
      return this->region->width;
    } // #width

    uint16_t height(){
      return this->region->height;
    } // #height

    void setContents(PageItem* c){
      this->contents = c;
      // Inform the PageItem of its Parent:
      this->contents->parent = static_cast<ItemContainer*>(this);
    } // #setContents
    PageItem* getContents(){
      return this->contents;
    } // #getContents

  private:
  PageItem* contents; //  - Contents of this GridCell

  }; // Class GridCell
 #endif // _CLASS_GRIDCELL_H
