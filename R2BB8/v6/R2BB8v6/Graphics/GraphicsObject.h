/****
 * Defines Abstract Features of a Graphics Object (shape), it's Region on the
 * Screen, and Associated Drawing Functions.
****/
#ifndef _CLASS_GRAPHICSOBJECT_H
#define _CLASS_GRAPHICSOBJECT_H

#include "../Util/ScreenPosition.h"
#include "../Hardware/HAL.h"


class GraphicsObject {
  public:
    ScreenRegion* shape; // Contains the Pertinent Information about the Shape
                        //  of this Graphics Object

    // If either of the following is set to NO_COLOR as defined in Colors.h, the 
    // feature should not be drawn.
    uint16_t color_outline; //  - Color of GraphicsObject Outline
    uint16_t color_fill;//      - Color of GraphicsObject Fill

    // Constructs an Null/Empty GraphicsObject
    GraphicsObject(void){ };

    // Draw the Graphics Object onto the Screen
    virtual void draw() = 0;
};


#endif //_CLASS_GRAPHICSOBJECT_H
