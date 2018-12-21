/* Infrastructure Neccessary to Send GCodes to the Attached Machine */

#ifndef _COMMANDER_H
#define _COMMANDER_H

void initCommander(){
  Serial.begin(115200);
} // #init_command

void issueCommand(const char* gcode, const char* letter){
  Serial.
} // #issueCommand

#endif // _COMMANDER_H
