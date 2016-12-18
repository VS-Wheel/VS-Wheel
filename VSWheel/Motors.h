#ifndef MOTORS_H
#define MOTORS_H

#include "mbed.h"
#include "Inputs.h"

class MOTORS {
   public:          
        void init(INPUTS *in, USBJoystick *j, volatile float * m1_p, volatile float * m1_d, volatile bool * m1_r, volatile float * m2_p, volatile float * m2_d, volatile bool * m2_r);
        bool manageFFBData();
   private:
        volatile float * m1_period;
        volatile float * m1_duty;
        volatile bool * m1_dir;
        volatile float * m2_period;
        volatile float * m2_duty;
        volatile bool * m2_dir;
};

#endif