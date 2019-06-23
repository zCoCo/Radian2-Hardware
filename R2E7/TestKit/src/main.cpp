// Compile using PlatformIO. Latest version tested: 3.6.3

#include <Arduino.h>
#include "SPI.h" // Platformio LDF has trouble knowing core libraries like this exist unless included here.

#include <HAL.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while(!Serial); // Wait for UART connection
  Serial.println("Start . . . ");

  init_HAL();
  init_stepper();
}

void blink(uint32_t t){
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  delay(t/2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(t/2);
}

unsigned long long last_time = 0;
void loop(){
  if (millis() - last_time > 250){
    Serial.println(stepper.distanceToGo());
    last_time = millis();
  }
  if(stepper.distanceToGo() == 0){
    Serial.println("Blorp");
    stepper.disableOutputs();
    delay(100);
    stepper.move(180*STEPS_PER_DEG); // Move 100deg
    stepper.enableOutputs();
  }
  stepper.run();
} // #loop

void loop2(){
  Serial.println("Blink");
  blink(250);
  blink(250);
  blink(500);
  blink(1000);
  Serial.println("Blonk");
  blink(500);
  blink(500);
  blink(1000);
  blink(1500);
}

/* Scheduler Notes: *look into migrating to FreeRTOS instead of finishing the scheduler*
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
