/* Infrastructure Neccessary to Send GCodes to the Attached Machine */

#ifndef _COMMANDER_H
#define _COMMANDER_H

#include "../Util/Pair.h"
#include "../Util/UtilMacros.h"

void initCommander(){
  Serial.begin(115200);
} // #init_command

#define MSG_MAX_SIZE = 45;
void issueCommand(const char* gcode, Pair<char, float>* arguments){
  char msg[MSG_MAX_SIZE]; // 1b + 4b for GCODE + up to 4 argument pairs at 10b per argument pair formatted as _Xyyy.zzz
  uint8_t idx = snprintf(msg, MSG_MAX_SIZE );
  for(uint8_t i=0; i<N_ARR(arguments); i++){
    idx += snprintf(msg+idx, MSG_MAX_SIZE-idx, "%c%f", (arguments[i])->key, (arguments[i]->value));
  }
  Serial.println(buff);
} // #issueCommand

#endif // _COMMANDER_H
