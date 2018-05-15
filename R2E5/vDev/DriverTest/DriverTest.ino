#include <Arduino.h>

#include "HAL.h"

void setup(){
  init_HAL();
} // #setup

void loop(){
  stepper.rotate(360);
  delay(250);
  stepper.rotate(180);
  delay(250);
  stepper.rotate(-180);
  delay(250);
  stepper.rotate(-360);
  delay(250);
} // #loop
