#include "Controller.h"

#define DIR -1
#define STP -1

void Controller::step(){
  // _pin[0] is step, _pin[1] is direction
  digitalWrite(pDIR, direction);
  digitalWrite(pSTP, HIGH);
  // Caution 200ns setup time
  delayMicroseconds(minPulseWidth);
  digitalWrite(pSTP, LOW);
} // #step

void Controller::computeSpeed(){
  static long DT_init, DT_prev, DT_min; // Step Sizes (DT).
  long distLeft = targetPos - currPos;
  long stepsLeft = (long)((steps_per_sec * steps_per_sec) / (2.0 * peakAccel));

  if(distLeft == 0 && stepsLeft < 2){ // arrived at target
    stepInterval = 0;
    steps_per_sec = 0.0;
    steps = 0;
    return;
  } else if(distLeft > 0){
    if(steps > 0 && (stepsLeft >= distLeft || direction == dirCCW)){
      // Begin decelerating when needed or decel
      steps -= stepsLeft;
    } else if(steps < 0){

    }
  }
} // #computeSpeed

boolean Controller::updateRun(){
  // Run Speed:
  unsigned long t = micros();
  if(!stepInterval && micros() - t_prev >= stepInterval){
    currPos += direction==dirCCW ? 1 : -1;
    step();
    t_prev = micros(); // use latest time. right after step complete.

    // Compute New Speed:
    computeSpeed();
  }

  return speed != 0.0 || distanceTo() != 0;
} // #computeRun
