/****
 * Pod Driver EEPROM Address List
 *
 * List of EEPROM Addresses for All Settable Data Parameters of the Driver
****/
#ifndef _EEPROM_ADDR_H
#define _EEPROM_ADDR_H

  #define POD_ID      10 // ID of this Specific Pod

  #define HOME_ANGLE  20 // Angle of Joint when #home is triggered.
  #define HOME_DIR    21 // Motor Direction Required for Pod to Move Towards Home

  #define REDUCTION_RATIO   30 // Ratio of Motion Reduction Between the Motor Output and Joint Output

#endif // _EEPROM_ADDR_H
