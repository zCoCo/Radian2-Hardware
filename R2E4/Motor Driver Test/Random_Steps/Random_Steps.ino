// Random.pde
// -*- mode: C++ -*-
//
// Make a single stepper perform random changes in speed, position and acceleration
//
// Copyright (C) 2009 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

//#include <AccelStepper.h>

// R2E4 v5:
/*
  STP - PD0 - 0
  DIR - PD1 - 1
  EN  - PD2 - 2
  MS1 - PD5 - 5
  MS2 - PB7 - 15
  MS3 - PB6 - 14
*/
#define STP 0
#define DIR 1
#define EN 2

#define MS1 5
#define MS2 15
#define MS3 14

// Define a stepper and the pins it will use
//AccelStepper stepper(1, STP, DIR); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setup(){
  // Initialize Microstepping State:
  /* Allegro A5984 Step Settings:
    321 | Fraction
    000 | 1/1
    001 | 1/2
    010 | 1/16
    011 | 1/32
    100 | 1/1 (70% pwr)
    101 | 1/2 (70% pwr)
    110 | 1/4
    111 | 1/8
  */
  pinMode(MS1, OUTPUT);
    digitalWrite(MS1, 1);
  pinMode(MS2, OUTPUT);
    digitalWrite(MS2, 1);
  pinMode(MS3, OUTPUT);
    digitalWrite(MS3, 1);

  // Activate Driver:
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EN, OUTPUT);
    digitalWrite(EN, 1);

  // Set Direction:
  digitalWrite(DIR, 1);
}

void loop()
{
  delay(1);
  digitalWrite(STP, 1);
  delay(1);
  digitalWrite(STP, 0);
}
