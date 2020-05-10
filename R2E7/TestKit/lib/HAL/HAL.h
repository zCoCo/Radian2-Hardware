/****
 * Hardware Abstraction Layer:
 *
 * Target System Hardware:
 * Stepper Motor connected to a strain-wave geared reduction system.
****/
#ifndef _M1_HAL_H
#define _M1_HAL_H

  #include <AccelStepper.h>
/*
//sch->EVERY(100)->DO(updateRegisterX); // <-this is dumb and prone to backups. Just call for a reg to be reupdated when the prev update is processed.

  Main
  <-Scheduler
  <-Comm
  <-Motion

  Scheduler

  System
    <-Registers
    <-HAL

  Comm
  <-HAL
  <-Motion

  Motion
  <- HAL.stepper

  HAL
  - Stepper
*/
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

  // Expectations:
  struct HAL_t{
    // The Unique ID of this Device on the Chain:
    const uint16_t deviceID;

    // Sub-modules:
    AccelStepper stepper;

    // Basic State Checks:
    bool in_programming_mode();
    bool last_device_in_chain();
    bool plugged_in_backwards();

    // CONTROL SIGNALS:

    // Sets the Fan Power Level to the Given Value between 0 and 100;
    void set_fan(uint8_t pwr);

    // DIAGNOTICS:

    // Returns the Voltage of the Board and Actuator's Power Supply, VM, in
    // decivolts (eg. 11.9V = 119)
    uint8_t power_voltage();
    // Whether the motor is currently in stand-still:
    bool motor_still();
    // Detect if a motor stall event has been reported on DIAG1 (which is active low).
    bool motor_stalled();
    // Detect if either motor coil is shorted to ground:
    bool motor_shorted();
    // Detect if there is an open motor coil
    bool motor_open();
    // Whether the motor driver is near an over-temperature shutdown.
    bool driver_warm();
    // Whether the driver has shutdown due to thermal overload
    bool driver_overloaded();

    // Initialize Hardware Components
    void init_core();
    void init_stepper();

    /*
     * Initialize Hardware Functionality at Runtime. Not called in constructor
     * since other functionality may be desired to be run first.
     */
    void begin(){
      init_core();
      init_stepper();
    }
  } HAL; // basically a low budget singleton

  // Hardware Specific Implemenations:
  #ifdef R2E7v1
    #include "R2E7v1.h"
  #elif M1C1v2
    #include "M1C1v2.h"
  #endif

  HAL_t::stepper = AccelStepper(AccelStepper::DRIVER, STP, DIR);

  void HAL_t::init_stepper(){
    stepper.setMaxSpeed(MAX_STEP_SPEED);
    stepper.setAcceleration(ACCEL * STEPS_PER_DEG);
    stepper.setEnablePin(ENA);
    stepper.setPinsInverted(0,0,1);
    stepper.enableOutputs();
  } // #init_stepper

#endif //_M1_HAL_H
