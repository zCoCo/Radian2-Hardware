/****
 * Hardware Abstraction Layer:
 *
 * Board Electronics:
 * uC: Attiny48 or Attiny88 (v1b) at 5V, 8MHz
 * Driver: Allegro A5984
 *
 * System Hardware:
 * Stepper Motor connected to a geared speed-reduction system.
****/
#ifndef _R2E5v1_HAL_H
#define _R2E5v1_HAL_H

  #include "../src/HAL.h"

  #define MICROSTEPS 2

  // Board GPIO Pinout:
  #define EN 8//PIN_B0
  #define MS1 5//PIN_D5
  #define MS2 15//PIN_B7
  #define MS3 14//PIN_B6
  #define RST -1
  #define SLP -1
  #define STP 10//PIN_B2
  #define DIR 9//PIN_B1

  #include "../src/src/StepperDriver/src/A5984.h"
  A5984 stepper(STEPS_PER_REV, DIR, STP, MS1, MS2, MS3);

  // Initialize Hardware Components:
  void init_HAL(){
    //pinMode(SLP, OUTPUT);
    //pinMode(RST, OUTPUT);
    pinMode(STP, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(EN, OUTPUT);
    //digitalWrite(SLP, HIGH);
    //digitalWrite(RST, HIGH);
    digitalWrite(EN, LOW);

    stepper.begin(TARGET_RPM, MICROSTEPS);
  } // #init_HAL

#endif // _R2E5v1_HAL_H
