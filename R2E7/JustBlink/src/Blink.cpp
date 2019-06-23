/*
 * Blink
 * Really basic test to verify that board is alive and programmable (based
 * heavily on platformio ststm32 examples)
 */

#include <Arduino.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

void setup(){
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void blink(uint32_t t){
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  delay(t);
  digitalWrite(LED_BUILTIN, LOW);
  delay(t);
}

void loop(){
  blink(1000);
  blink(500);
  blink(250);
  blink(250);
}
