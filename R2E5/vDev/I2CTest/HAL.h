#ifndef _HAL_H
#define _HAL_H

  // Motor Specs:
  #define STEPS_PER_REV 200
  #define MICROSTEPS 16

  // Board GPIO Pinout:
  #define EN 12
  #define MS1 11
  #define MS2 10
  #define MS3 9
  #define RST 8
  #define SLP 7
  #define STP 6
  #define DIR 5

  #define TARGET_RPM 20

  #include "A4988.h"
  A4988 stepper(STEPS_PER_REV, DIR, STP, EN, MS1, MS2, MS3);

  // Initialize Hardware Components:
  void init_HAL(){
    digitalWrite(SLP, HIGH);
    digitalWrite(RST, HIGH);
    stepper.begin(TARGET_RPM, MICROSTEPS);
    stepper.enable();
  }

#endif //_HAL_H
