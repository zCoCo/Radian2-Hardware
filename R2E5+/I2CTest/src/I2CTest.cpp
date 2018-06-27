// Compile using PlatformIO. Latest version tested: 3.6.0a1
#include <Arduino.h>
//#include "ArduinoAttinyCompatibilityPatch.h"

#include "HAL.h"
#include "Comm.h"
#include "Motion.h"

void setup(){
  init_HAL();
  init_stepper();
  //init_comm();
  //home();
} // #setup

void loop(){
  // TEST:
  goTo(360);
  delay(250);
  goTo(180);
  delay(250);
  goTo(-180);
  delay(250);
  goTo(-360);
  delay(250);
} // #loop

/* Scheduler Notes:
Sch.run() called as frequently as possible.
Sch.concurrent = [void (*fcn)(void)] to be called on every #run event
Contains RingBuffer of (timeToBeExecuted, (*fcn)(void)) events.
Sch::run:
  -find nearest non-concurrent event
  -call each item in concurrent array
    - after each call, ensure next non-concurrent even hasn't passed.
      - if it has passed or is about to, call it and find next nearest n-c event.
  - loop through non-concurrent event schedule buffer and call any events whose
    time has come.
*/
