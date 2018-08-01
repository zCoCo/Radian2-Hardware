// Basic Low-Level Motion Control
#ifndef _MOTION_H
#define _MOTION_H

  #include "HAL.h"

  float current_angle = 0.0f; // Current Joint Angle in Degrees

  // Home the Joint
  void home(){
    // @TODO
  } // #home

  // Go to the Given Angle [in degrees], where 0deg is pointing straight forward
  // and the positive direction is counter-clockwise.
  void goTo(float ang){
    float Dth = (ang-current_angle);
    current_angle = ang; // Update this /before/ the move so positioning interrupts
                        // can update angle as move occurs if necessary.
    stepper.rotate(-GEAR_RATIO*Dth);
  } // #goTo

#endif //_MOTION_H
