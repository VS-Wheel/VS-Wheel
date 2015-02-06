#ifndef INPUTS_H
#define INPUTS_H

#include "mbed.h"
#include "USBJoystick.h"
#include "Multiplexer.h"
#include "QEI.h"

#define PEDDIV 258 // Pedal divider
#define PEDZERO 127 // Pedal zero substraction

#define ROTENC_CH1      P0_27               // Channel 1
#define ROTENC_CH2      P0_28               // Channel 2
#define ROTENC_INDEX    NC                  // Index
#define ROTENC_PULSES   48                  // Pulses per revolution
#define ROTENC_ENCODING QEI::X4_ENCODING    // X2 encoding = looks at the state every time a rising or falling edge occurs on channel A
                                            // X4 encoding = looks at the state every time a rising or falling edge occurs on channel A or channel B

class INPUTS {
   public:   		
   		void init(USBJoystick *joy); // Initialize all the inputs
		void send(void); // Send wheel inputs to the PC

   private:
   		// Current values
   		int8_t throttle_;
		int8_t brake_;
		int8_t clutch_;
		int16_t x_;
		int16_t y_;
		int32_t radius_;
		int32_t angle_;
		uint32_t buttons_;

   		// Previous Values
   		int8_t prevThrottle_;
		int8_t prevBrake_;
		int8_t prevClutch_;
		int16_t prevX_;
		int16_t prevY_;
		int32_t prevRadius_;
		int32_t prevAngle_;
		uint32_t prevButtons_;

        uint8_t ped_getValue(uint16_t value); // returns 8 bit value
};

#endif