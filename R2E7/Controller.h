#ifndef _CLASS_CONTROLLER_H
#define _CLASS_CONTROLLER_H

#include <stdlib.h>
#include <Arduino.h>

// These defs cause trouble on some versions of Arduino
#undef round

typedef int gpio;

class Controller{
public:
  // Motor Driver Pins:
  gpio pSTP;
  gpio pDIR;
  gpio pENA;

  // Settings:
  int dirCCW = 0; // Direction Signal (1 or 0) used for indicating CCW rotation.
  unsigned int minPulseWidth = 1; // Minimum length of STEP pulse.
  unsigned long stepInterval = 0; // Time between Steps. 0 if not stepping.

  long jerk;
  long peakAccel;
  long peakSpeed;
  long long targetPos;

  // State:
  float steps_per_sec = 0; // Speed in Steps per Second
  int direction = 0; // Current Direction of Rotation.
  unsigned long t_prev = 0; // Micros time of previous step.
  long long currPos = 0; // Step Position where +ve is CCW.

  long steps; // Step counter

  // Sends a Step Pulse Signal to the Motor Driver
  void step();

private:

};

#endif // _CLASS_CONTROLLER_H
