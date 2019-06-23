// Compile using PlatformIO. Latest version tested: 3.6.3

#include <Arduino.h>
#include "SPI.h" // Platformio LDF has trouble knowing core libraries like this exist unless included here.

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

void setup(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void blink(uint32_t t){
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  delay(t/2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(t/2);
}

void loop(){
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
