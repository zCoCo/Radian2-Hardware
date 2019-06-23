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

  #include "HAL.h"

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

  // struct {
  //     uint8_t blank_time = 24;        // [16, 24, 36, 54]
  //     uint8_t off_time = 3;           // [1..15]
  //     uint8_t hysteresis_start = 1;   // [1..8]
  //     int8_t hysteresis_end = 12;     // [-3..12]
  // } config;
  // void initPins();

  // Initialize Hardware Components:
  void init_HAL(){
    pinMode(STP, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, LOW);

    SPI.begin();
    Serial.begin(9600);
    while(!Serial); // Wait for UART connection
    Serial.println("Start . . . ");

    pinMode(CS_DRV, OUTPUT);
    digitalWrite(CS_DRV, HIGH);
    driver.begin();             // Initiate pins and registeries
    driver.rms_current(600);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    //driver.stealthChop(1);      // Enable extremely quiet stepping
    //driver.stealth_autoscale(1);
    driver.microsteps(MICROSTEPS);
  } // #init_HAL

#endif // _R2E7v1_HAL_H
