#ifndef MOTORS_H
#define MOTORS_H

#include "mbed.h"
#include "Inputs.h"

class MOTORS {
   public:   		
		void init(INPUTS *in, USBJoystick *j,float * m1_p, float * m1_d, bool * m1_r, float * m2_p, float * m2_d, bool * m2_r);
		bool manageFFBData();
   private:
   		float * m1_period;
   		float * m1_duty;
   		bool * m1_dir;
   		float * m2_period;
   		float * m2_duty;
   		bool * m2_dir;
};

#endif