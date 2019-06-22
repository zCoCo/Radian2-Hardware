// Compile using PlatformIO. Latest version tested: 3.6.0a1
#include <Arduino.h>
//#include "ArduinoAttinyCompatibilityPatch.h"

#include <Servo.h>

//#include "HAL.h"
//#include "Comm.h"
//#include "Motion.h"

void setup(){
  Serial.begin(9600);
  // while(!Serial);
  // Serial.println("Start...");
  // init_HAL();
  // init_stepper();
  // init_comm();
  // home();
} // #setup

void loop(){
  Serial.println("Blink");
  delay(500);
  Serial.println("Blonk");
  // Serial.println("Beat");
  // if(stepper.distanceToGo() == 0){
  //   stepper.disableOutputs();
  //   delay(100);
  //   stepper.move(180*STEPS_PER_DEG); // Move 100deg
  //   stepper.enableOutputs();
  // }
  // stepper.run();
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
