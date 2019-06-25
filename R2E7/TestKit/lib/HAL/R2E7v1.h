/****
 * Hardware Abstraction Layer for R2E7v1 Motor Module Driver:
 *
 * Board Electronics:
 * uC: STM32-F103C8(T6) (Bluepill layout)
 * Driver: Trinamic TMC2130-TA
****/
#ifndef _R2E7v1_HAL_H
#define _R2E7v1_HAL_H

  #include <SPI.h>
  #include <TMCStepper.h>
  #include "TMC2130_REGDEFS.h"

  #include <HAL.h>

  // Board GPIO Pinout:
  #define TXD     PA9
  #define RXD     PA10
  #define SCL     PB8
  #define SDA     PB9
  #define SDA_IN  PB11 // 5V line
  #define SCL_IN  PB10 // 5V line
  #define MOSI    PA7
  #define MISO    PA6
  #define SCK     PA5
  #define CS_DRV  PA4
  #define DIAG1   PA3
  #define STP     PA2
  #define DIR     PA1
  #define ENA     PA0
  #define SLP -1
  #define SER1    PB0
  #define SENSD   PA15
  #define SENSC   PB3
  #define SENSB   PB4
  #define SENSA   PB5

  #define R_SENSE 0.11 // SilentStepStick

  // Software Setup:
  // Macros expected by TMC Library:
  #define EN_PIN   ENA // Enable
  #define DIR_PIN  DIR // Direction
  #define STEP_PIN STP // Step
  #define CS_PIN   CS_DRV // Chip select
  TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE);
  //TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE, MOSI, MISO, SCK); // bit-bangs SW-SPI

  // Initialize Hardware Components:
  void init_HAL(){
    pinMode(SENSA, INPUT);
    pinMode(SENSB, INPUT);
    pinMode(SENSC, INPUT);
    pinMode(SENSD, INPUT);

    pinMode(STP, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, LOW);

    pinMode(CS_DRV, OUTPUT);
    digitalWrite(CS_DRV, HIGH);
    SPI.begin();
    pinMode(MISO, INPUT_PULLUP);

    driver.begin();             // Initiate pins and registeries
    driver.rms_current(600);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    //driver.stealthChop(1);      // Enable extremely quiet stepping
    //driver.stealth_autoscale(1);
    driver.microsteps(MICROSTEPS);
    driver.diag1_stall(1);
    driver.diag1_pushpull(1);

  	driver.push(); // Push shadow registers
      driver.tbl(1);
      driver.TPOWERDOWN(255);
  	driver.toff(4);

    // coolStep Threshold:
    driver.TCOOLTHRS(0xFFFFF); // 20bit max
    driver.THIGH(0);

    // stallGuard2:
    driver.semin(5);
    driver.semax(2);
    driver.sedn(0b01);
    driver.sgt(15); // STALL_VALUE:[-64..63] - higher->less sensitive

  	// Effective hysteresis = 0
  	driver.hstrt(0);
  	driver.hend(2);

  	// stealthChop PWM:
  	driver.en_pwm_mode(true);
  	driver.pwm_freq(1);
  	driver.pwm_autoscale(true);
  	driver.pwm_ampl(180);
  	driver.pwm_grad(1);

    // Ensure DIAG1 only reports on motor stall:
    pinMode(DIAG1, INPUT_PULLUP); // rated as weak pull-up at 30-50kOhm, DIAG1 is spec'd for 47kOhm max.
    driver.diag1_pushpull(0); // Driver will pull up to 5V if enabled and DIAG1 is not on 5V tolerant pin
    driver.diag1_stall(1);
    driver.diag1_index(0);
    driver.diag1_onstate(0);

  	digitalWrite(EN_PIN, LOW); // Re-nable
  } // #init_HAL


  // DIAGNOTICS:

  // Whether the motor is currently in stand-still:
  bool motor_still(){
    return driver.stst(); // TODO: Add encoder feedback checking
  }

  // Detect if a motor stall event has been reported on DIAG1 (which is active low).
  bool motor_stalled(){
    // Check both in case one diagnostic path becomes broken:
    return !digitalRead(DIAG1) || driver.stallguard();
  }

  // Detect if either motor coil is shorted to ground:
  bool motor_shorted(){
    // Check both in case one diagnostic path becomes broken:
    return driver.s2ga() | driver.s2gb();
  }

  /*
   * Detect if either motor coil circuit is reporting an open loop (broken).
   * NOTE: False postives can be triggered by:
      - Low motor voltage
      - High motor velocities
      - No motor velocity (stand-still)
      - Motor short
      - High driver temperatures
    * Therefore, only perform this test while running at low velocity.
    */
  bool motor_open(){
    // Either coil A or B reports open loop AND driver has not been sitting in standstill (for 2^20 clocks):
    return (driver.ola() | driver.olb()) & !motor_still();
  }

  // Whether the motor driver is near an over-temperature shutdown.
  bool driver_warm(){
    return driver.otpw(); // over-temperature pre-warning flag
  }

  /*
   * Whether the driver has shutdown due to thermal overload.
   * NOTE: Once this threshold has been crossed, it will remain in shutdown
   * driver temperature drops below optw.
   */
  bool driver_overloaded(){
    return driver.ot(); // over-temperature flag
  }

#endif // _R2E7v1_HAL_H
