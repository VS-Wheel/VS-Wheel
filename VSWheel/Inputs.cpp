/**
 * @file   Inputs.cpp
 * @author The VS-Wheel team (Alexander Selivanov and Pascal Vaillancourt)
 * @date   March 27 2015
 * @brief  All the functions needed for the inputs
 *
 * @version 1.2 : First version of all the working inputs
 * Developpement: Sublime Text 3
 * Compiler: GNU ARM GCC 5.4
 */

#include "Inputs.h"



USBJoystick * joystick; // Joystick obj
QEI rotEnc(ROTENC_CH1, ROTENC_CH2, ROTENC_INDEX, ROTENC_PULSES, ROTENC_ENCODING); // Rotary encoder obj (Physical Wheel)

void INPUTS::init(USBJoystick *joy)
{
    // Current values
    throttle_ = 0;
    brake_ = 0;
    clutch_ = 0;
    x_ = 0;
    buttons_ = 0;

    // Previous Values
    prevThrottle_ = 0;
    prevBrake_ = 0;
    prevClutch_ = 0;
    prevX_ = 0;
    prevButtons_ = 0;

    joystick = joy; // Object association
}

bool INPUTS::send(void)
{
        // Comparing the previous and the new value, before sending data.
        // This prevents flooding the transmission buffer
        if( x_ != prevX_ || buttons_ != prevButtons_)
        {
            joystick->update(x_, y_, buttons_, throttle_, brake_, clutch_);
        }
        
        // Setting the previous values
        prevX_ = x_;
        prevButtons_ = buttons_;
        return true;
}

int16_t INPUTS::get_XAxis(void)
{
  // getPulses() returns the number of pulses acquired
  // Casting the value into int16_t, because the host is expecting an int16_t
  // multiplier = 32767 / difference between the left and right extremities  (in our case it's read dynamically, using X4_Encoding).
  // In other words, zero for one side and the max value for the other side

  static int16_t x;
  x = (int16_t)(((rotEnc.getPulses() - offsetXAxis_) * multiplier_ * -1) + ROTENC_MAX_VALUE);
  if(x < 0 && x < (-1 * ROTENC_CENTER)) x = ROTENC_MAX_VALUE;
  if(x < 0) x = 0;
  return x;
}

int16_t INPUTS::get_encPulses(void)
{
  return rotEnc.getPulses();
}

void INPUTS::set_multiplier(float difLR)
{
  multiplier_ = ROTENC_MAX_VALUE / difLR;
}

void INPUTS::set_offsetX(int16_t offX)
{
  offsetXAxis_ = offX;
}