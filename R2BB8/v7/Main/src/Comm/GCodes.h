/* All Valid GCodes and Associated Command Letters, Stored in PROGMEM. */

#ifndef _GCODE_LIST_H
#define _GCODE_LIST_H

#include "../Util/StringUtils.h"

// GCodes:
const __FlashStringHelper* G0, G1, G399;

// GCode Command Letters:
const __FlashStringHelper* GcX, GcY, GcZ, GcF, GcS, GcT;

// This function exists because #PSTR/F (inside #SS) must be called from within
// a function.
void populateGCODE(){
  // GCode Command Letters:
  GcX = S("X");
  GcY = S("Y");
  GcZ = S("Z");
  GcF = S("F");
  GcS = S("S");
  GcT = S("T");


  // GCodes:
  G0 = S("G0");
  G1 = S("G1");

  G399 = S("G399");
}

#endif // _GCODE_LIST_H
