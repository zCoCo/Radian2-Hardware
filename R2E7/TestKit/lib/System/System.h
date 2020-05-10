/****
 * Struct defining data to be accessed system-wide, including the registers
 * which will be shared over RS-485.
 *
 * Author: Connor W. Colombo, Ottia
 * Last Update: 9/13/2019, Colombo
****/
#ifndef _M1_SYSTEM_H
#define _M1_SYSTEM_H

  #include <OttiaM1RegisterBank.h>
  #include <HAL.h>

  // By encapsulating the Registers Struct, the rest of the system can more
  // seamlessly access what's on a shared register in the same way as everything
  // else.
  struct system_t {
    OttiaM1RegisterBank Registers;

    system_t(){
      updateDriverStateRegister();
      updateSystemStateRegister();
    }

    void updateDriverStateRegister(){
      static float bufferRatio; // Ratio of used buffer to available buffer.

      Registers.DriverState.data.position = 0.0;
      Registers.DriverState.data.speed = 0.0;
      Registers.DriverState.data.voltage = HAL.power_voltage();

      Registers.DriverState.data.bufferFull = 0;

      // Update the Buffer Warning Flags:
      bufferRatio = 1.0f * stream.available() / SERIAL_RX_BUFFER_SIZE;
      if(bufferRatio > 1.0f){
        Registers.DriverState.data.bufferState = Registers.DriverState.data.m1_bufferState_t.overload;
      } else if(bufferRatio > 0.75f){
        Registers.DriverState.data.bufferState = Registers.DriverState.data.m1_bufferState_t.severe;
      } else if(bufferRatio > 0.5f){
        Registers.DriverState.data.bufferState = Registers.DriverState.data.m1_bufferState_t.warn;
      } else{
        Registers.DriverState.data.bufferState = Registers.DriverState.data.m1_bufferState_t.open;
      }
      Registers.DriverState.data.still = HAL.motor_still();
      Registers.DriverState.data.driverWarm = HAL.driver_warm();

      Registers.DriverState.data.fault.stalled = HAL.motor_stalled();
      Registers.DriverState.data.fault.shorted = HAL.motor_shorted();
      Registers.DriverState.data.fault.openCoil = HAL.motor_open();
      Registers.DriverState.data.fault.overloaded = HAL.driver_overloaded();
    }

    void updateSystemStateRegister(){
      Registers.SystemState.data.uptime = millis();
      Registers.SystemState.data.lastDevice = HAL.last_device_in_chain();
      Registers.SystemState.data.PIB = HAL.plugged_in_backwards();
      Registers.SystemState.data.DevID_raw = analogRead(DEV_ID);
    }
  } System;

#endif // _M1_SYSTEM_H
