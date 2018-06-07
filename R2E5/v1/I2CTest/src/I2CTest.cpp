// Compile using PlatformIO. Latest version tested: 3.6.0a1
#include <Arduino.h>
//#include "ArduinoAttinyCompatibilityPatch.h"

#include "HAL.h"
//#include "Comm.h"
#include "Motion.h"

void setup(){
  init_HAL();
  //init_comm();
  //home();
} // #setup

void loop(){
  // TEST:
  goTo(360.0);
  delay(250);
  goTo(180.0);
  delay(250);
  goTo(-180.0);
  delay(250);
  goTo(-360.0);
  delay(250);
} // #loop
