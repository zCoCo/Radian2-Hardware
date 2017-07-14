// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper_Mini.h>


//PDO
#define STP 0
//PD1
#define DIR 1
//PD2
#define _EN_ 2
AccelStepper_Mini stepper(STP, DIR);

//PB0
#define LED_A 8
//PB1
#define LED_K 9

void setup()
{
  pinMode(_EN_, OUTPUT);
   digitalWrite(_EN_, LOW);

  pinMode(LED_A, OUTPUT);
   digitalWrite(LED_A, HIGH);
  pinMode(LED_K, OUTPUT);
   digitalWrite(LED_K, LOW);

   stepper.setMaxSpeed(1000);
   stepper.setSpeed(50);
}

void loop()
{
   stepper.runSpeed();
}
