/****
 * Hardware Abstraction Layer:
 *
 * Board Electronics:
 * uC: Atmega168p at 5V, 16MHz
 * Driver: Trinamic TMC2130
 *
 * System Hardware:
 * Stepper Motor connected to a geared speed-reduction system.
****/
#ifndef _R2E6v1_HAL_H
#define _R2E6v1_HAL_H

  #include <TMC2130Stepper.h>
  #include <SPI.h>
  #include "../src/HAL.h"

  #define MICROSTEPS 2

  // Board GPIO Pinout:
  #define EN      2//PIN_D2
  #define STP     9//PIN_B1
  #define DIR     6//PIN_D6
  #define DIAG1   5//PIN_D5
  #define CS_TMC  A6//ADC6/PE2

  TMC2130Stepper driver = TMC2130Stepper(EN, DIR, STP, CS_TMC);

  /*
    Set Supply RMS Current in mA Using Internal RDSon for Current Sensing and Rref in kOhms
    to provide a Current Reference, Iref.
    Calculations:
    Iref, mA = 5/(1+Rref)
    Ipeak, vsense=0, mA = 3000*Iref
    Ipeak, vsense=1, mA = 0.55*Ipeak, vsense=0

    Irms = Ipeak / sqrt(2)
    Isup, mA = (CS+1)/32 * Irms

    Isup, vsense=0, mA = (CS+1)/32 * 3000 * 5 / (1+Rref) /sqrt(2)
    Isup, vsense=1, mA = (CS+1)/32 * 0.55 * 3000 * 5 / (1+Rref) / sqrt(2)

    ->CS, vsense=0 = 4 * sqrt(2) * (Rref+1) * Isup / 1875 - 1
    ->CS, vsense=1 = 4 * sqrt(2) * (Rref+1) * Isup / 1031.25 - 1

    N.B.: Standard IHOLD multiplier is 0.5
  */
  void int_current(uint16_t Isup, float multiplier, float Rref) {
    driver.internal_Rsense(1); // Init.
    uint8_t CS = 4.0 * 1.41421 * (Rref+1.0) * Isup / 1875.0 - 1;
    // If Current Scale is too low, turn on high sensitivity R_sense and calculate again
    if (CS < 16) {
      driver.vsense(true);
      CS = 4.0 * 1.41421 * (Rref+1.0) * Isup / 1031.25 - 1;
    } else if(driver.vsense()) { // If CS >= 16, turn off high Vfs sensitivity mode if it's currently ON.
      driver.vsense(false); // Vfs = 0.32V
    }
    driver.irun(CS);
    driver.ihold(CS*multiplier); // Steady-state holding current.
  } // #int_current

  // Initialize Hardware Components:
  void init_HAL(){
    SPI.begin();
    pinMode(MISO, INPUT_PULLUP);
    driver.begin(); 			// Initiate pins and registeries
    int_current(1080, 0.5, 6.98); // Set Running Motor RMS Coil Current to 1080mA, with 0.5 IHOLD multiplier, using a Reference Resistor of 6k98Ohm

    digitalWrite(EN, LOW);
    digitalWrite(CS_TMC, LOW);
  } // #init_HAL

#endif // _R2E6v1_HAL_H
