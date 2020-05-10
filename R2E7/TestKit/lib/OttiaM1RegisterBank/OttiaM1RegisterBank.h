/****
 * Registers of the M1 to be Accessed Remotely over RS-485.
 * This is to be used as a common message definition file for use by all devices
 * that communicate on the Ottia M1 RS-485 bus.
 *
 * NOTE: To maintain >100Hz comm. speed with 25 devices on the line where RS485
 *       is clipped at 500kbps, registers must be <=14Bytes to maintain message
 *       sizes <= 21B with a message header of 7B.
 *
 * Author: Connor W. Colombo, Ottia
 * Last Update: 11/24/2019, Colombo
****/
#ifndef _OTTIA_M1_REG_H
#define _OTTIA_M1_REG_H

  #include "Register.h"

  #define MAX_REG_LENGTH 15 // Absolute maximum acceptible number of bytes in a register

  struct OttiaM1RegisterBank{ // Multiple banks will need to be instantiated for basestation
    // Frequently Updated / Requested:
    Register<command_reg_t> LatestCommand = { .id = 0 };
    Register<driverState_reg_t> DriverState = { .id = 1 };
    // Infrequently Updated / Requested:
    Register<settings_reg_t> Settings = { .id = 2 };
    Register<systemState_reg_t> SystemState = { .id = 3 };

    /* Returns a Pointer to the Byte Array for the Raw Data of the Register with
    the Given ID. */
    unsigned char* getRegisterBytesFromID(unsigned int id){
      switch(id){ // TODO: find an automatic, equally memory efficient way to do this.
                  // void* ARRAY???
        case 0:
          return this->LatestCommand.raw;
        break;
        case 1:
          return this->DriverState.raw;
        break;
        case 2:
          return this->Settings.raw;
        break;
        case 3:
          return this->SystemState.raw;
        break;
        default:
          return nullptr;
      }
    }
  };

  // Most Recent Command Sent to the Device (updated frequently):
  struct command_reg_t { // Size: 12B Sent / 13B Local.
    // "Registers":
    long double position; //        - Position in deg. With 64bits, IEEE754 has
                         //         52 bit mantissa, therefore 0-2^53 can be
                        //          stored with integer precision; so, angles
                       //           can be tracked continuously with > 1/100th deg
                      //            resolution for up to +-250 Billion revolutions.
                     //             (only 233 revolutions where double=float is 32bit).
    float speed; //                 - Target Average Speed in deg/s. With 32bits,
                //                  IEEE754 has 23 bit mantissa, therefore 0-2^23
               //                   can be stored with integer precision; so,
              //                    speedcan be tracked continuously at
             //                     > 1/100th deg/s resolution up to
            //                      +-83,800deg/s = 1300rpm.
    unsigned char commandLoaded; // - Whether this Command has been Loaded into
                                //  the Buffer (not sent over but just stored here).
  };

  // Important Data and Vitals about Device (checked frequently):
  struct driverState_reg_t { // Size: 14B
    // "Registers":
    long double position; //        - Position in deg. With 64bits, IEEE754 has
                         //         52 bit mantissa, therefore 0-2^53 can be
                        //          stored with integer precision; so, angles
                       //           can be tracked continuously with > 1/100th deg
                      //            resolution for up to +-250 Billion revolutions.
                     //             (only 233 revolutions where double=float is 32bit).
    float speed; //                 - Speed in deg/s. With 32bits, IEEE754 has
                //                  23 bit mantissa, therefore 0-2^23 can be
               //                   stored with integer precision; so, speed
              //                    can be tracked continuously at > 1/100th deg/s
             //                     resolution up to +-83,800deg/s = 1300rpm.
    uint8_t voltage; //             - Supply Voltage (VM), in decivolts (eg. 15.2V = 152);

    // "Coils":
    enum m1_bufferState_t = {
      open = 0, //  - Buffer in nominal state
      warn = 1, //  - Buffer filling up (Navail > 0.5*SERIAL_RX_BUFFER_SIZE)
      severe = 2, //- Buffer near max capacity (Navail > 0.75*SERIAL_RX_BUFFER_SIZE)
      overload = 3//- Buffer at max capacity. Driver likely missing messages (Navail > SERIAL_RX_BUFFER_SIZE)
    };
    unsigned char bufferState:2; // - State of the Primary Comms Buffer (takes m1_bufferState_t)
    unsigned char still:1; //       - Motor Still (see HAL#motor_still)
    unsigned char driverWarm:1; //  - Motor Getting Hot (see HAL#driver_warm)
    struct fault{
      unsigned char stalled:1; //   - Motor Stalled (see HAL#motor_stalled)
      unsigned char shorted:1; //   - Motor Shorted (see HAL#motor_shorted)
      unsigned char openCoil:1; //  - Motor has Open Coil (see HAL#motor_open)
      unsigned char overloaded:1; //- Motor Driver is Overtemperature (see HAL#driver_overloaded)
    }
  };

  // Device Settings (not likely to be changed often, so it doesn't need to be
  // sent everytime):
  enum m1_fanState_t = {fan_always_off = 0, fan_always_on = 1, fan_on_warm = 2, fan_on_overload = 3};
  struct settings_reg_t { // Size: 9Bytes
    unsigned long max_accel; //       - Max. Allowable Output Acceleration in deg/s/s
    unsigned long jerk; //            - Constant Output Jerk in deg/s/s/s
    unsigned char fan_behavior:2; //  - Set the Fan Behavior (0=always off, 1=always on, 2=on when warm 3=on when overloaded)
  };

  // Useful but Mostly Static Information about the Device (not likely to change
  // often, so it doesn't need to be sent everytime):
  struct systemState_reg_t { // Size: 4Bytes
    unsigned long uptime; //        - Time Device has been Turned On in ms.
    unsigned char lastDevice:1; //  - Whether this Device is the Last in the Chain.
    unsigned char PIB:1;  //        - Whether the Connector is Detected as being Plugged-In-Backwards.
    uint16_t DevID_raw;  //         - Raw Reading from Device ID.
  };
#endif //_OTTIA_M1_REG_H
