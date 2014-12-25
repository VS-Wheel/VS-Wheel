#ifndef INPUTS_H
#define INPUTS_H

#include "mbed.h"

#define PEDDIV 258 // Pedal divider
#define PEDZERO 127 // Pedal zero substraction

uint8_t ped_getValue(uint16_t value); // returns 8 bit value

#endif