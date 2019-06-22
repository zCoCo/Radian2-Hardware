// Compile using PlatformIO. Latest version tested: 3.6.0a1
#include <Arduino.h>
#define LED PC13

void setup() {
  // put your setup code here, to run once:
  pinMode(PC13, OUTPUT);
}

void blink(int t){
  digitalWrite(LED, HIGH);
  delay(t);
  digitalWrite(LED, LOW);
  delay(t);
}

void loop() {
  blink(1000);
  blink(500);
  blink(500);
}
