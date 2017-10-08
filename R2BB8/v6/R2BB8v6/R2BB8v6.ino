// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include "./Graphics/Colors.h"

#include "BB8_Strings.h" // Localized Strings

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);// 300-Ohms Across X-Plate (~500 across Y)

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A7

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

#define NUM_TEST_STRS 2
const __FlashStringHelper* TEST_STRING;
const __FlashStringHelper* TEST_STRING_B;
const __FlashStringHelper* TEST_STRING_ARR[NUM_TEST_STRS];
const __FlashStringHelper* TEST_STRING_ARRUN[NUM_TEST_STRS];
byte idx = 0;

void populateTestStrings(){
  TEST_STRING = SS("TEST STRING");
  TEST_STRING_B = SS("TEST STRING B");
  TEST_STRING_ARR[0] = SS("TEST STRING A0");
  TEST_STRING_ARR[1] = SS("TEST STRING A1");
  TEST_STRING_ARRUN[0] = SS("TEST STRING AUN0");
}

const __FlashStringHelper* GetTestString(const __FlashStringHelper* arr[], byte lang){
  const __FlashStringHelper* ret_arr = arr[0];
  if(arr[lang] != nullptr){
    ret_arr = arr[lang];
  }
  return ret_arr;
}

void setup(void) {
  //Initialize Data Structures:
  populateStrings();


  delay(500); // -- DISPLAY DOESN'T WORK IF NOT GIVEN TIME TO INITIALIZE W/PWR BEFORE TALKING.
  Serial.begin(9600);

  Serial.println(F("Test Begins"));
  populateTestStrings();
  Serial.println(TEST_STRING);

  Serial.println("Array Index Test:");
  Serial.println(TEST_STRING_ARR[0]);
  Serial.println(TEST_STRING_ARR[1]);

  Serial.println("Array Variable Index Test:");
  Serial.println(TEST_STRING_ARR[idx]);
  idx = 1;
  Serial.println(TEST_STRING_ARR[1]);

  Serial.println("Array TestGet Function Test:");
  Serial.println(GetTestString(TEST_STRING_ARR,0));
  Serial.println(GetTestString(TEST_STRING_ARR,1));
  Serial.println("Array Get Function Test:");
  Serial.println(GetString(TEST_STRING_ARR,0));
  Serial.println(GetString(TEST_STRING_ARR,1));
  Serial.println("Array GS Function Test:");
  Serial.println(GS(TEST_STRING_ARR,0));
  Serial.println(GS(TEST_STRING_ARR,1));
  Serial.println("Array GS UN Function Test:");
  Serial.println(GS(TEST_STRING_ARRUN,0));
  Serial.println(GS(TEST_STRING_ARRUN,1));
  Serial.println("Array GS OB Function Test:");
  Serial.println(GS(TEST_STRING_ARR,0));
  Serial.println(GS(TEST_STRING_ARR,1));
  Serial.println(GS(TEST_STRING_ARR,2));
  Serial.println(GS(TEST_STRING_ARR,3));
  Serial.println(GS(TEST_STRING_ARR,4));
  Serial.println("Array GS LID Function Test:");
  Serial.println(GS(TEST_STRING_ARR,LID_ENGLISH));
  Serial.println(GS(TEST_STRING_ARR,LID_SPANISH));

  Serial.print("\nString Population Built: ");
  Serial.println(strings_populated);
  Serial.println(GS(STR__X_Position,LID_ENGLISH));
  Serial.println(GS(STR__X_Position,LID_SPANISH));
    Serial.println(GS(STR__Y_Position,LID_ENGLISH));
    Serial.println(GS(STR__Y_Position,LID_SPANISH));
      Serial.println(GS(STR__Z_Position,LID_ENGLISH));
      Serial.println(GS(STR__Z_Position,LID_SPANISH));

  Serial.println(GSC(STR__LANGUAGE_NAME));
  CURR_LANG = LID_SPANISH;
  Serial.println(GSC(STR__LANGUAGE_NAME));

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    return;
  }

  tft.begin(identifier);

  tft.fillScreen(BLACK);

  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  // tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, WHITE);

  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;


  tft.setCursor(50, 50);
  tft.setTextColor(GREEN);  tft.setTextSize(2);
  tft.println(F("End Test"));
  tft.setCursor(50, 90);
  tft.println(F("End Test 2"));
  tft.setCursor(50, 110);
  tft.println(TEST_STRING);


  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop() {
  TSPoint p = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    if (p.y < (TS_MINY-5)) {
      Serial.println("erase");
      // press the bottom of the screen to erase
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
    }
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    /*
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
    */
    if (p.y < BOXSIZE) {
       oldcolor = currentcolor;

       if (p.x < BOXSIZE) {
         currentcolor = RED;
         tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*2) {
         currentcolor = YELLOW;
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*3) {
         currentcolor = GREEN;
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*4) {
         currentcolor = CYAN;
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*5) {
         currentcolor = BLUE;
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*6) {
         currentcolor = MAGENTA;
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
       }

       if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
          if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
          if (oldcolor == GREEN) tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
          if (oldcolor == CYAN) tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
          if (oldcolor == BLUE) tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
          if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
       }
    }
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
  }
}
