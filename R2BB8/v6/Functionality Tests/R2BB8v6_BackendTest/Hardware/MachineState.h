/****
 * Static Struct MachineState: container class for properties such as
 * Hotend_Temperature on the machine which get updated over SPI from the Main
 * Board.
****/
#ifndef _STRUCT_MACHINESTATE_H
#define _STRUCT_MACHINESTATE_H


#include "./Util/Property.h"
#include "BB8_Strings.h"

struct MachineState {
  public:
  ///////////////////////////////////////////////////////////// - POSITIONING
    Effector_Position_X = Property<int>(GSC(STR__X_Position),0);
    Effector_Position_Y = Property<int>(GSC(STR__Y_Position),0);
    Effector_Position_Z = Property<int>(GSC(STR__Z_Position),0);

  ///////////////////////////////////////////////////////////// - PROJECT DATA
    Job_Percentage = Property<int>(GSC(STR__Job_Percentage),0);
    Time_Remaining = Property<int>(GSC(STR__Time_Remaining),0);

  ///////////////////////////////////////////////////////////// - 3D PRINTING
    Hotend_Temp = Property<int>(GSC(STR__Hotend_Temp),0);
  private:
} Ottia_MS;

#endif //_STRUCT_MACHINESTATE_H
