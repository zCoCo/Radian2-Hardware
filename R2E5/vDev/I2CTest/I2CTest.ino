#include <Arduino.h>

#include "HAL.h"
#include "Comm.h"
#include "Motion.h"

void setup(){
  init_HAL();
  init_comm();
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
