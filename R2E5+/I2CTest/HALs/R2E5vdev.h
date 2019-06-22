// Hardware Abstraction Layer
// Hardware: Essentially, an Arduino Nano soldered directly to an A4988 Step Stick.
#ifndef _R2E5vdev_HAL_H
#define _R2E5vdev_HAL_H

  #include "Arduino.h"
  #include "../src/HAL.h"

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

  //#include "A4988.h"
  //A4988 stepper(STEPS_PER_REV, DIR, STP, EN, MS1, MS2, MS3);
  #include "../src/src/StepperDriver/src/A5984.h"
  A5984 stepper(STEPS_PER_REV, DIR, STP, MS1, MS2, MS3);
  #warning "Stepper Function Deprecated in R2E5vdev Library."

  // Initialize Hardware Components:
  void init_HAL(){
    digitalWrite(SLP, HIGH);
    digitalWrite(RST, HIGH);
    //stepper.begin(TARGET_RPM, MICROSTEPS);
    //stepper.enable();
  } // #init_HAL

#endif // _R2E5vdev_HAL_H
