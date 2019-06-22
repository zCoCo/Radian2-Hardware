// Hardware Abstraction Layer
// Hardware: Essentially, an Arduino Nano soldered directly to an A4988 Step Stick.
#ifndef _HAL_H
#define _HAL_H

  // Physical Specs:

  // Ratio of Number of Turns on the Motor Shaft to Number of Turns on the Output
  // 69.60:15.60 = 4.4615384615 <- from Pitch Diameter
  // 58:13 = 4.4615384615 <- from Number of Teeth
  #define GEAR_RATIO (15.0f * 4.4615384615f)

  // Motor Specs:
  #define STEPS_PER_REV 200
  #define MICROSTEPS 2

  // Board GPIO Pinout:
  #define EN 12
  #define MS1 11
  #define MS2 10
  #define MS3 9
  #define RST 8
  #define SLP 7
  #define STP 6
  #define DIR 5

  #define TARGET_RPM 150

  #include "A4988.h"
  A4988 stepper(STEPS_PER_REV, DIR, STP, EN, MS1, MS2, MS3);

  // Initialize Hardware Components:
  void init_HAL(){
    digitalWrite(SLP, HIGH);
    digitalWrite(RST, HIGH);
    stepper.begin(TARGET_RPM, MICROSTEPS);
    stepper.enable();
  } // #init_HAL

#endif //_HAL_H
