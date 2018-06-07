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

  // Physical Specs:

  // Ratio of Number of Turns on the Motor Shaft to Number of Turns on the Output
  // 69.60:15.60 = 4.4615384615 <- from Pitch Diameter
  // 58:13 = 4.4615384615 <- from Number of Teeth
  // Cycloid Drive Reduction: 20 ring pins -> 19x reduction
  #define GEAR_RATIO (19.0f * 4.4615384615f)

  // Motor Specs:
  #define STEPS_PER_REV 200
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

  #define TARGET_RPM 150

  #include "./src/StepperDriver/src/A5984.h"
  A5984 stepper(STEPS_PER_REV, DIR, STP, MS1, MS2, MS3);

  // Initialize Hardware Components:
  void init_HAL(){
    //pinMode(SLP, OUTPUT);
    //pinMode(RST, OUTPUT);
    pinMode(EN, OUTPUT);
    //digitalWrite(SLP, HIGH);
    //digitalWrite(RST, HIGH);
    digitalWrite(EN, LOW);

    stepper.begin(TARGET_RPM, MICROSTEPS);
  } // #init_HAL

#endif //_HAL_H
