/****
 * Hardware Abstraction Layer:
 *
 * Board Electronics:
 * uC: Attiny48 (at 5V, 8MHz)
 * Driver: Allegro A5984
 *
 * System Hardware:
 * Stepper Motor connected to a geared speed-reduction system.
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

  // Motor Specs:
  #define STEPS_PER_REV 200
  #define MAX_RPM 10 //150
  #define ACCEL 100

  #ifdef R2E5v1
    #include "../HALs/R2E5v1.h"
  #endif
  #ifdef R2E5vdev
    #include "../HALs/R2E5vdev.h"
  #endif
  #ifdef R2E6v1
    #include "../HALs/R2E6v1.h"
  #endif

  AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, STP, DIR);

  void init_stepper(){
    stepper.setMaxSpeed(MAX_RPM);
    stepper.setAcceleration(ACCEL);
  } // #init_stepper

#endif //_HAL_H
