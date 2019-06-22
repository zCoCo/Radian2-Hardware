/*
 * Defines All Basic Characteristics (Interface) for an Object which Contains
 * At Least One PageItem Must Have to be able to be Called as a Parent.
 */

 #ifndef _CLASS_ITEMCONTAINER_H
 #define _CLASS_ITEMCONTAINER_H

 #include "../Util/RectRegion.h"

 class ItemContainer{
 public:
   RectRegion* container_region;

   static const ItemContainer NULL_CONTAINER = ItemContainer();

   ~ItemContainer(){
     delete container_region;
   } // dtor

   uint16_t width(){
     return this->region->width;
   } // #width
   uint16_t height(){
     return this->region->height;
   } // #height

 private:
   // Creates an Empty ItemContainer:
   ItemContainer(){
     this->container_region = new RectRegion(0,0,0,0);
   } // ctor

 }; // Class ItemContainer

 #endif // _CLASS_ITEMCONTAINER_H
