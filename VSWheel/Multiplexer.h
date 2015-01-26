#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include "mbed.h"

#define CH0 	0b0000
#define CH1 	0b0001
#define CH2 	0b0010
#define CH3 	0b0011
#define CH4 	0b0100
#define CH5 	0b0101
#define CH6 	0b0110
#define CH7 	0b0111
#define CH8 	0b1000
#define CH9 	0b1001
#define CH10 	0b1010
#define CH11 	0b1011
#define CH12 	0b1100
#define CH13 	0b1101
#define CH14 	0b1110
#define CH15 	0b1111

uint16_t requestButtonState(uint8_t channel);

#endif