#include "Inputs.h"

BusIn busButtons(
    P0_9, P0_8, P0_7, P0_6
);

AnalogIn pot_throttle(P0_23);
AnalogIn pot_brake(P0_24);
AnalogIn pot_clutch(P0_25);

USBJoystick * joystick; // Joystick obj (The whole setup)
QEI rotEnc(ROTENC_CH1, ROTENC_CH2, ROTENC_INDEX, ROTENC_PULSES, ROTENC_ENCODING); // Rotary encoder obj (Physical Wheel)

void INPUTS::init(USBJoystick *joy)
{
    // Current values
    throttle_ = 0;
    brake_ = 0;
    clutch_ = 0;
    x_ = 0;
    y_ = 0;
    radius_ = 0;
    angle_ = 0;
    buttons_ = 0;

    // Previous Values
    prevThrottle_ = 0;
    prevBrake_ = 0;
    prevClutch_ = 0;
    prevX_ = 0;
    prevY_ = 0;
    prevRadius_ = 0;
    prevAngle_ = 0;
    prevButtons_ = 0;

    busButtons.mode(OpenDrain); // A kind of internal pull-down resistor for a bus
    joystick = joy;
}

void INPUTS::send(void)
{
        throttle_ = ped_getValue(pot_throttle.read_u16()) & 0xFF; // value -127 .. 128
        brake_ = ped_getValue(pot_brake.read_u16()) & 0xFF; // value -127 .. 128
        clutch_ = ped_getValue(pot_clutch.read_u16()) & 0xFF; // value -127 .. 128

        // Reading button values          
        buttons_ = busButtons.read() & 0xFFFFFFFF;     
        //y = pot1.read_u16() & 0xFFFF;
        y_ = 0;
        x_ = (rotEnc.getPulses() * 682) & 0xFFFF;

        if(throttle_ != prevThrottle_ || brake_ != prevBrake_ || clutch_ != prevClutch_ || x_ != prevX_ || y_ != prevY_|| radius_ != prevRadius_ || angle_ != prevAngle_ || buttons_ != prevButtons_)
        {
            joystick->update(x_, y_, buttons_, throttle_, brake_, clutch_);
        }
        
        prevThrottle_ = throttle_;
        prevBrake_ = brake_;
        prevClutch_ = clutch_;
        prevX_ = x_;
        prevY_ = y_;
        prevRadius_ = radius_;
        prevAngle_ = angle_;
        prevButtons_ = buttons_;
}

uint8_t INPUTS::ped_getValue(uint16_t value)
{   
    return ((value / PEDDIV) - PEDZERO);
}