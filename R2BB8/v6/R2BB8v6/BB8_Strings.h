/****
  * Flash Storage of All User-Facing Strings, across Multiple Languages.
  *
  * The purpose of the following structure is to maximize the ability to
  * localize strings on the fly while minimizing MCU memory consumption,
  * namely RAM but also program space.
****/
#ifndef _BB8_STRINGS_H
#define _BB8_STRINGS_H


#include <WString.h>

//Number of Languages:
#define NUM_LANG 2
//Language Indices for the Subsequent String Arrays.
#define LID_ENGLISH 0
#define LID_SPANISH 1

byte CURR_LANG = 0;// Variable for Current Language

/*
 Safely get a String from the desired Array in the Given Language, lang,
 returning the English result if no string is defined for the desired language.
 Example Call: GetString(STR__LANGUAGE_NAME, LID_ENGLISH)
*/
const __FlashStringHelper* GetString(const __FlashStringHelper* arr[], byte lang){
  const __FlashStringHelper* ret_str = arr[0];
  if(arr[lang] != nullptr){
    ret_str = arr[lang];
  } // } else{
  //   Serial.println(">GetString OverBounds Detected");
  // }
  return ret_str;
}
//Shorthand:
#define GS(a,l) GetString(a,l)
//Get String in Current Language:
#define GSC(a) GetString(a, CURR_LANG)

/*
 Stores the Given String in PROGMEM Flash.
 This macro exists in case MCUs are changed and the flash storage technique
 is different.
*/
#define SS(s) F(s)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// -- STRINGS --
////////////////////////////////////////////////////////////////////////////////
const __FlashStringHelper* STR__LANGUAGE_NAME[NUM_LANG];/// PROGMEMify?

const __FlashStringHelper* STR__X_Position[NUM_LANG];
const __FlashStringHelper* STR__Y_Position[NUM_LANG];
const __FlashStringHelper* STR__Z_Position[NUM_LANG];

// This function exists because #PSTR/F (inside #SS) must be called from within
// a function.
void populateStrings(){
  static bool populated = 0;
  //Name of Language at Index
  if(!populated){
    STR__LANGUAGE_NAME[LID_ENGLISH] = SS("ENGLISH");
    STR__LANGUAGE_NAME[LID_SPANISH] = SS("ESPANOL");

    STR__X_Position[LID_ENGLISH] = SS("X Position");
    STR__X_Position[LID_SPANISH] = SS("Posicion X");

    STR__Y_Position[LID_ENGLISH] = SS("Y Position");
    STR__Y_Position[LID_SPANISH] = SS("Posicion Y");

    STR__Z_Position[LID_ENGLISH] = SS("Z Position");
    STR__Z_Position[LID_SPANISH] = SS("Posicion Z");
    populated = 1;
  }
};


#endif //_BB8_STRINGS_H
