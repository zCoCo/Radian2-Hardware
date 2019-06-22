// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper_Mini.h>
#include <I2C.h>
#include "t48_Pinout.h"

//Potentiometer Base Address
#define POT_ADDR 0b0101000
//Potentiometer Address Setting Pins
#define POT_ADDR0 PD6
#define POT_ADDR1 PC7
#define POT_ADDR2 PC3

#define STP PD0
#define DIR PD1
#define _EN_ PD2
#define MS1 PD5
#define MS2 PB7
#define MS3 PB6

AccelStepper_Mini stepper(STP, DIR);

//PB0
#define LED_A 8
//PB1
#define LED_K 9

void setup() {
  I2c.begin(); // join i2c bus
   //Set Potentiometer Address:
   pinMode(POT_ADDR0, OUTPUT);
    digitalWrite(POT_ADDR0, POT_ADDR & 1);
   pinMode(POT_ADDR1, OUTPUT);
    digitalWrite(POT_ADDR1, (POT_ADDR >> 1) & 1);
   pinMode(POT_ADDR2, OUTPUT);
    digitalWrite(POT_ADDR2, (POT_ADDR >> 2) & 1);
   // Set Potentiometer Current:
   I2c.write(POT_ADDR, 0x00, 127); //Pot Value Byte (2 Amps)

  // Setup Stepper Driver:
  pinMode(MS1, OUTPUT);
   digitalWrite(MS1, LOW);
  pinMode(MS2, OUTPUT);
   digitalWrite(MS2, LOW);
  pinMode(MS3, OUTPUT);
   digitalWrite(MS3, LOW);
  pinMode(_EN_, OUTPUT);
   digitalWrite(_EN_, LOW);

   // Setup Stepping Characteristics:
   stepper.setMaxSpeed(1000);
   stepper.setSpeed(800);

   //Indicator LED:
   pinMode(LED_A, OUTPUT);
    digitalWrite(LED_A, HIGH);
   pinMode(LED_K, OUTPUT);
    digitalWrite(LED_K, LOW);
}

void loop() {
  stepper.runSpeed();
}
