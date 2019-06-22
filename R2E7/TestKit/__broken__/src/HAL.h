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
  // Ratio of Number of Turns on the Motor Shaft to Number of Turns on the Output
  // 69.60:15.60 = 4.4615384615 <- from Pitch Diameter
  // 58:13 = 4.4615384615 <- from Number of Teeth
  // Cycloid Drive Reduction: 20 ring pins -> 19x reduction
  #define GEAR_RATIO 1.0f //(19.0f * 4.4615384615f)
  #define MICROSTEPS 2

  // Motor Specs:
  #define STEPS_PER_REV 200
  #define MAX_RPM 10 //150
  #define ACCEL 100

  constexpr float STEPS_PER_DEG = 200.0f / 360.0f; // 200 stp/rev / 360 deg/rev

  #ifdef R2E7v1
    #include "../HALs/R2E7v1.h"
  #endif

  AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, STP, DIR);

  void init_stepper(){
    stepper.setMaxSpeed(180 * STEPS_PER_DEG); // 180deg/s (TODO: verify units of fnc)
    stepper.setAcceleration(1000 * STEPS_PER_DEG); // 100deg/s^2 (TODO: verify units of fnc)
    stepper.setEnablePin(ENA);
    stepper.setPinsInverted(0,0,1);
    stepper.enableOutputs();
  } // #init_stepper

#endif //_HAL_H
