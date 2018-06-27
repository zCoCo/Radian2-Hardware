/*
 * A5984 - Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 * Modified from A4988.cpp by Connor Colombo (Ottia), 2018
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "A5984.h"

/*
 * Microstepping resolution truth table (Page 8 of A5984 pdf)
 * 0bMS3,MS2,MS1 for 1,2,4,8,16,32 microsteps
 */
const uint8_t A5984::MS_TABLE[] = {0b000, 0b001, 0b110, 0b0111, 0b010, 0b011};

/*
 * Fully wired.
 * All the necessary control pins for A5984 are connected.
 */
A5984::A5984(short steps, short dir_pin, short step_pin, short ms1_pin, short ms2_pin, short ms3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin),
    ms1_pin(ms1_pin), ms2_pin(ms2_pin), ms3_pin(ms3_pin)
{}

void A5984::begin(short rpm, short microsteps){
    BasicStepperDriver::begin(rpm, microsteps);

    pinMode(ms1_pin, OUTPUT);
    pinMode(ms2_pin, OUTPUT);
    pinMode(ms3_pin, OUTPUT);
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for A5984 are 1:1 to 1:32
 * If the control pins are not connected, we recalculate the timing only
 */
short A5984::setMicrostep(short microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    const uint8_t* ms_table = getMicrostepTable();
    size_t ms_table_size = getMicrostepTableSize();

    unsigned short i = 0;
    while (i < ms_table_size){
        if (this->microsteps & (1<<i)){
            uint8_t mask = ms_table[i];
            digitalWrite(ms3_pin, mask & 4);
            digitalWrite(ms2_pin, mask & 2);
            digitalWrite(ms1_pin, mask & 1);
            break;
        }
        i++;
    }
    return this->microsteps;
}

const uint8_t* A5984::getMicrostepTable(){
    return A5984::MS_TABLE;
}

size_t A5984::getMicrostepTableSize(){
    return sizeof(A5984::MS_TABLE);
}

short A5984::getMaxMicrostep(){
    return A5984::MAX_MICROSTEP;
}
