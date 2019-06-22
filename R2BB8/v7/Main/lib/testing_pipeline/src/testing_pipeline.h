#include <unity.h>

void UNITY_SETUP(){
}

#ifdef ARDUINO

#include <Arduino.h>

void setup(){
  // Wait 2000ms if board doesn't support software reset via Serial.DTR/RTS:
  delay(2000);
  UNITY_BEGIN();
    test();
  UNITY_END();
} // #setup

void loop(){
  // nothing
} // #loop

#else

int main(int argc, char **argv){
  UNITY_BEGIN();
    test();
  UNITY_END();
  return 0;
} //#main

#endif
