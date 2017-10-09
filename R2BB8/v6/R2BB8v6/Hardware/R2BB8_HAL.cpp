/*
 HAL Class Defining the Parts and Functions of the R2BB8 board.
*/

#include "Arduino.h"
#include "R2BB8_HAL.h"

void R2BB8_HAL::init(void){
  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341"));
  } else {
    Serial.print(F("Unknown LCD driver: "));
    Serial.println(identifier, HEX);
    return;
  }

  tft.begin(identifier);
}

void R2BB8_HAL::erase(void){
  tft.fillScreen(BLACK);
}
