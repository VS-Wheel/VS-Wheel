#ifndef MOTORS_H
#define MOTORS_H

#include "mbed.h"
#include "Inputs.h"

#define FACTOR 5
#define PERIOD 255 / FACTOR

class MOTORS {
   public:   		
		void init(INPUTS *in, USBJoystick *j, uint16_t * m1_p, uint16_t * m1_d, bool * m1_r, uint16_t * m2_p, uint16_t * m2_d, bool * m2_r);
		void manageFFBData();
   private:
   		uint16_t * m1_period;
   		uint16_t * m1_duty;
   		bool * m1_dir;
   		uint16_t * m2_period;
   		uint16_t * m2_duty;
   		bool * m2_dir;
};

#endif