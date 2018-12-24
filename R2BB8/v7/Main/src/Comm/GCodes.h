/* All Valid GCodes Stored in PROGMEM. */

#ifndef _GCODE_LIST_H
#define _GCODE_LIST_H

#include "../Util/StringUtils.h"

// GCodes:
const __FlashStringHelper* G0, G1, G399;

// This function exists because #PSTR/F (inside #SS) must be called from within
// a function.
void populateGCODE(){
  // GCodes:
  G0 = S("G0");
  G1 = S("G1");

  G399 = S("G399");
}

#endif // _GCODE_LIST_H
