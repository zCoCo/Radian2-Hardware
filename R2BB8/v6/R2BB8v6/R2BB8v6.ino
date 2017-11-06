// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include "./Util/UtilMacros.h"

#include <Adafruit_GFX.h>//         - Core graphics library
#include "./Graphics/Colors.h"//    - Predefined 16-Bit Color Macros
//
#include "./Hardware/HAL.h"//       - Hardware Abstraction Layer

#include "BB8_Strings.h" //         - Localized Strings

#include "./Menu/ButtonItem.h"//    - Buttons.

RectRegion* EraseButton = new RectRegion(0,HAL.HEIGHT-11, HAL.WIDTH, 11);
RectRegion* PaintRegion = new RectRegion(0,BOXSIZE, HAL.WIDTH, HAL.HEIGHT-BOXSIZE);

// MenuItem* buttons[] = {
//   new ButtonItem(new RectRegion(BOXSIZE*0,BOXSIZE, BOXSIZE, BOXSIZE), RED),
//   new ButtonItem(new RectRegion(BOXSIZE*1,BOXSIZE, BOXSIZE, BOXSIZE), YELLOW),
//   new ButtonItem(new RectRegion(BOXSIZE*2,BOXSIZE, BOXSIZE, BOXSIZE), GREEN),
//   new ButtonItem(new RectRegion(BOXSIZE*3,BOXSIZE, BOXSIZE, BOXSIZE), CYAN),
//   new ButtonItem(new RectRegion(BOXSIZE*4,BOXSIZE, BOXSIZE, BOXSIZE), BLUE),
//   new ButtonItem(new RectRegion(BOXSIZE*5,BOXSIZE, BOXSIZE, BOXSIZE), MAGENTA)
// };


void setup(void) {
  Serial.begin(9600);
  Serial.println("> Begin");
  //Initialize Data Structures:
  populateStrings();
                                                                                Serial.println("Strings Initialized.");

  init_HAL();
                                                                                Serial.println("HAL Initialized.");
                                                                                Serial.print("Screen Size:");
                                                                                Serial.print(HAL.tft.width());
                                                                                Serial.print("x");
                                                                                Serial.println(HAL.tft.height());

  HAL.tft.setCursor(50, 50);
  HAL.tft.setTextColor(GREEN);  HAL.tft.setTextSize(2);
  HAL.tft.println(GSC(STR__LANGUAGE_NAME));
                                                                                Serial.println("Language Named.");

  delay(1000);
}

void loop() {
  ScreenPosition touch = HAL.touchPosition();

  delay(1000);
  HAL.eraseRegion(PaintRegion);
  HAL.tft.setCursor(50, 50);
  HAL.tft.setTextColor(0xFFFF-BGCOLOR);  HAL.tft.setTextSize(2);
  HAL.tft.println(String(millis()));


  // if( EraseButton.contains(touch) ){
  //   HAL.eraseRegion(PaintRegion);
  // }

  // for(auto& button : buttons){
  //   button->update(touch);
  //   button->draw();
  // }

  //
  //
  // if (p.y < BOXSIZE) {
  //    oldcolor = currentcolor;
  //
  //    if (p.x < BOXSIZE) {
  //      currentcolor = RED;
  //      HAL.tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  //    } else if (p.x < BOXSIZE*2) {
  //      currentcolor = YELLOW;
  //      HAL.tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
  //    } else if (p.x < BOXSIZE*3) {
  //      currentcolor = GREEN;
  //      HAL.tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
  //    } else if (p.x < BOXSIZE*4) {
  //      currentcolor = CYAN;
  //      HAL.tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
  //    } else if (p.x < BOXSIZE*5) {
  //      currentcolor = BLUE;
  //      HAL.tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
  //    } else if (p.x < BOXSIZE*6) {
  //      currentcolor = MAGENTA;
  //      HAL.tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
  //    }
  //
  //    if (oldcolor != currentcolor) {
  //       if (oldcolor == RED) HAL.tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  //       if (oldcolor == YELLOW) HAL.tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  //       if (oldcolor == GREEN) HAL.tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  //       if (oldcolor == CYAN) HAL.tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  //       if (oldcolor == BLUE) HAL.tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  //       if (oldcolor == MAGENTA) HAL.tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  //    }
  // }
  // if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
  //   HAL.tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
  // }

}
