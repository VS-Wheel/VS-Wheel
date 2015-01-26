/*********************************************
*	Using a 4067 - 16bit analog multiplexor	 *
*	from sparkfun							 *
**********************************************/

#include "Multiplexer.h"

AnalogIn sig(P1_31);

DigitalOut s0(P1_31);		// Signal 0 on the 4067
DigitalOut s1(P2_10);		// Signal 1 on the 4067
DigitalOut s2(P2_11);		// Signal 2 on the 4067
DigitalOut s3(P2_12);		// Signal 3 on the 4067


uint16_t requestButtonState(uint8_t channel)
{   
    //Mask each channel to send the right value to the multiplexor and send the value 
	s0 = channel & 0b0001;
	s1 = channel & 0b0010;
	s2 = channel & 0b0100;
	s3 = channel & 0b1000;

	return sig.read_u16();	
}