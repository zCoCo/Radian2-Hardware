class MachineState {
/****
 * Static Struct MachineState: container class for properties such as
 * Nozzle_Temperature on the machine which get updated over SPI from the Main
 * Board.
****/
#ifndef _STRUCT_MACHINESTATE_H
#define _STRUCT_MACHINESTATE_H


#include "./Util/Property.h"
#include "BB8_Strings.h"

struct MachineState {
  public:
    Effector_Position_X = Property<int>('X Position',0);
    Effector_Position_Y = Property<int>('Y Position',0);
    Effector_Position_Z = Property<int>('Z Position',0);
  private:
} Ottia_MS;

#endif //_STRUCT_MACHINESTATE_H
