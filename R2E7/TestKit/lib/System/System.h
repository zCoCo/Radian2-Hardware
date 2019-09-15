/****
 * Struct defining data to be accessed system-wide, including the registers
 * which will be shared over RS-485.
 *
 * Author: Connor W. Colombo
 * Last Update: 9/13/2019, Colombo
****/
#ifndef _M1_SYSTEM_H
#define _M1_SYSTEM_H

  #include <OttiaM1RegisterBank.h>
  #include <HAL.h>

  // By extending the Registers Struct, the rest of the system can more
  // seamlessly access what's on a shared register in the same way as everything
  // else.
  struct system_t {
    OttiaM1RegisterBank Registers;

    system_t(){
      updateDriverStateRegister();
      updateSystemStateRegister();
    }

    void updateDriverStateRegister(){
      Registers.DriverState.position = 0.0;
      Registers.DriverState.speed = 0.0;
      Registers.DriverState.voltage = HAL.power_voltage();

      Registers.DriverState.bufferFull = 0;
      Registers.DriverState.still = HAL.motor_still();
      Registers.DriverState.driverWarm = HAL.driver_warm();

      Registers.DriverState.fault.stalled = HAL.motor_stalled();
      Registers.DriverState.fault.shorted = HAL.motor_shorted();
      Registers.DriverState.fault.openCoil = HAL.motor_open();
      Registers.DriverState.fault.overloaded = HAL.driver_overloaded();
    }

    void updateSystemStateRegister(){
      Registers.SystemState.uptime = millis();
      Registers.SystemState.lastDevice = HAL.last_device_in_chain();
      Registers.SystemState.PIB = HAL.plugged_in_backwards();
      Registers.SystemState.DevID_raw = analogRead(DEV_ID);
    }
  } System;

#endif // _M1_SYSTEM_H
