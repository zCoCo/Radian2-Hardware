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
#include "./Util/StringUtils.h"

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
  }
  return ret_str;
}
//Shorthand:
#define GS(a,l) GetString(a,l)
//Get String in Current Language:
#define GSC(a) GetString(a, CURR_LANG)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// -- STRINGS --
////////////////////////////////////////////////////////////////////////////////
const __FlashStringHelper* STR__LANGUAGE_NAME[NUM_LANG];/// PROGMEMify?

const __FlashStringHelper* STR__X_Position[NUM_LANG];
const __FlashStringHelper* STR__Y_Position[NUM_LANG];
const __FlashStringHelper* STR__Z_Position[NUM_LANG];

const __FlashStringHelper* STR__Job_Percentage[NUM_LANG];
const __FlashStringHelper* STR__Time_Remaining[NUM_LANG];

const __FlashStringHelper* STR__Hotend_Temp[NUM_LANG];

// This function exists because #PSTR/F (inside #SS) must be called from within
// a function.
void populateStrings(){
  static bool populated = 0;
  //Name of Language at Index
  if(!populated){
    /////////////////////////////////////////////////////////////// - META DATA
    STR__LANGUAGE_NAME[LID_ENGLISH] = SS("ENGLISH");
    STR__LANGUAGE_NAME[LID_SPANISH] = SS("ESPANOL");

    ///////////////////////////////////////////////////////////// - POSITIONING
    STR__X_Position[LID_ENGLISH] = SS("X Position");
    STR__X_Position[LID_SPANISH] = SS("Posicion X");

    STR__Y_Position[LID_ENGLISH] = SS("Y Position");
    STR__Y_Position[LID_SPANISH] = SS("Posicion Y");

    STR__Z_Position[LID_ENGLISH] = SS("Z Position");
    STR__Z_Position[LID_SPANISH] = SS("Posicion Z");

    //////////////////////////////////////////////////////////// - PROJECT DATA
    STR__Job_Percentage[LID_ENGLISH] = SS("Job Percentage");
    STR__Job_Percentage[LID_SPANISH] = SS("Porcentaje de Trabajo");

    STR__Time_Remaining[LID_ENGLISH] = SS("Time Remaining");
    STR__Time_Remaining[LID_SPANISH] = SS("Tiempo Restante");

    ///////////////////////////////////////////////////////////// - 3D PRINTING
    STR__Hotend_Temp[LID_ENGLISH] = SS("Hotend Temperature");
    STR__Hotend_Temp[LID_SPANISH] = SS("Temperatura de Hotend");
    populated = 1;
  }
};


#endif //_BB8_STRINGS_H
