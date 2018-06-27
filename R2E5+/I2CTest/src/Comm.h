// I2C Communication Protocol
#ifndef _COMM_H
#define _COMM_H

#include <Wire.h>
#include <EEPROM.h>

#include "EEPROM_addrs.h"
#include "Util.h"
#include "Motion.h"

  int pod_id = 100; // Default Value
  #define POD_ID_EEPROM_ADDR 10 // Address of the POD_ID in EEPROM

  // Check if a Special Pod ID has been Set in EEPROM and, if so, Return it, if
  // not, return the current value of POD_ID (the default).
  int getPodID(){
    int id = EEPROM.read(POD_ID_EEPROM_ADDR);
    if(id == 0xFF){ // Default Value of Clean EEPROM
      id = POD_ID; // then use the default value for a pod-driver
    }

   return id;
  } // #getPodID

  // Sets a Special Pod ID to be used for Communication and Stores it in EEPROM.
  // By design, this requires a reset to take effect so as to not disrupt ongoing
  // communication.
  // NOTE: I2C Address are 7bit so Max ID is 127.
  void setPodID(int id){
    EEPROM.update(POD_ID_EEPROM_ADDR, constrain(id, 0,127));
  } // #setPodID

  /* INCOMING I2C MESSAGING PROTOCOL:
   - N Byte Message:
   -- 0: N=Length (of subsequent bytes, excluding itself)  1: Command   [2,N]: Contents/Value
    Comm 1 -> Go To Angle -> [4bytes as float] Angle, in degrees
    Comm 2 -> Home Joint -> [0 bytes]

    Comm 90 -> Set Pod ID -> [1 byte as int] ID
    */
  void process_I2C_message(int8_t* msg){
    short len = msg[0];

    switch(msg[1]){ // Command
      case 1: //                                - Go To Angle
        FloatUnion_t ang;
        for(int i=0; i<4; i++){
          ang.bytes[i] = msg[2+i];
        }

        goTo(ang.number);
      break;

      case 2: //                                - Home Joint
        home();
      break;

      case 90:
        setPodID(msg[2]);
      break;

      default:
        // Do Nothing.
      break;
    }
  } // #process_I2C_message

  // I2C Message Intake
  // Low-Level Protocol Package Structure:
  // - Package Byte 0: '#' to Initiate
  // - Package Byte 1: Message Length, N
  // - Package Bytes [2,N-1]: Message Contents to be Processed Later
  #define I2C_INITIALIZER '#'
  #define DEFAULT_I2C_MSG_LENGTH 3
  #define MAX_I2C_MSG_LENGTH 10
  void read_I2C_message(int bytesIn){
    static short msg_byte = 0; // Current Message Byte being Processed
    static short msg_length = 0; // Length of Current Message
    static bool in_msg = 0; // Whether a message is currently being read.
    static int8_t curr_msg[MAX_I2C_MSG_LENGTH+1];

    int8_t c = (int8_t) Wire.read();

    if(in_msg){
      if(msg_length == 0){ // No length set yet, set it.
        if(c != 0){
          msg_length = c;
        } else{ // errant msg length, end msg and don't process.
          in_msg = 0;
        }
      } else{
        curr_msg[msg_byte] = c; // Load in Data.

        if(msg_byte == (msg_length - 1)){ // Advance Write Position or end Message.
          in_msg = 0; // End Message (before calling process to avoid desynchronization)
          process_I2C_message(curr_msg);
        } else{
          msg_byte++;
        }
      } // msg_length==0?

    } else if(c == I2C_INITIALIZER){
      in_msg = 1; // Start reading message
      msg_length = 0; // Default.
      msg_byte = 0;
    } // in_msg?
  } // #read_I2C_message

  void init_comm(){
    pod_id = getPodID();
    Wire.begin(pod_id);
    Wire.onReceive(read_I2C_message);
  } // #init_comm

#endif //_COMM_H
