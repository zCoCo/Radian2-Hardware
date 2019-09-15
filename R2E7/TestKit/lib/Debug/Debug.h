#ifndef _DEBUG_H
#define _DEBUG_H

  #include "Arduino.h"

  #if DEBUG
    HardwareSerial* Debug = &Serial1;
  #else
    HardwareSerial* Debug = nullptr;
  #endif

#endif // _DEBUG_H
