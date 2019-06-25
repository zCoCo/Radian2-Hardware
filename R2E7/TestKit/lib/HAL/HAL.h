/****
 * Hardware Abstraction Layer:
 *
 * Target System Hardware:
 * Stepper Motor connected to a strain-wave geared reduction system.x
****/
#ifndef _HAL_H
#define _HAL_H

  #include <AccelStepper.h>

  // Physical Specs:
  #define MICROSTEPS 64

  // Motor Specs:
  constexpr float FULL_STEPS_PER_REV = 200.0f; // ignoring microstepping
  #define MAX_RPM 90 // [rev/min] Empirical
  #define ACCEL 500 // [deg/s^2]
  #define MAX_RMS_CURRENT 1000 // [mA] - Max. RMS motor coil current

  constexpr float STEPS_PER_REV = MICROSTEPS * FULL_STEPS_PER_REV;
  constexpr float STEPS_PER_DEG = STEPS_PER_REV / 360.0f;

  constexpr float MAX_STEP_SPEED = MAX_RPM * 360.0f * STEPS_PER_DEG / 60.0f; // [stp/sec]

  #ifdef R2E7v1
    #include "R2E7v1.h"
  #endif

  AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, STP, DIR);

  void init_stepper(){
    stepper.setMaxSpeed(MAX_STEP_SPEED);
    stepper.setAcceleration(ACCEL * STEPS_PER_DEG);
    stepper.setEnablePin(ENA);
    stepper.setPinsInverted(0,0,1);
    stepper.enableOutputs();
  } // #init_stepper

#endif //_HAL_H
