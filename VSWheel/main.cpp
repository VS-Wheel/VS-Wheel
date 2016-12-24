#include "mbed.h"
#include "Inputs.h"
#include "Motors.h"

// PWM direction for motor 1
DigitalOut m1DIR(P0_9);
// PWM direction for motor 2
DigitalOut m2DIR(P0_8);

// PWM output for the first motor
PwmOut m1PWM(P2_0);
// PWM output for the second motor
PwmOut m2PWM(P2_1);

void centerWheel(INPUTS *in);

int main() {
    // Initialization of the needed objects
    static USBJoystick joystick;
    static MOTORS motors;
    static INPUTS inputs;

    // Initialization of the motor1/motor2 PWM values (period, duty cycle, direction)
    static uint16_t m1_p = 0, m1_d  = 0;
    static uint16_t m2_p = 0, m2_d = 0;
    static bool m1_r = 0, m2_r = 0;

    // Initialization of the inputs & the motors + wheel calibration at start
    inputs.init(&joystick);
    motors.init(&inputs, &joystick, &m1_p, &m1_d, &m1_r, &m2_p, &m2_d, &m2_r);
    wait(1.0);
    centerWheel(&inputs);

    // Infinite loop
    while (true) {

        // Send the inputs to the HOST
        inputs.send();
        // Retrieval & Analyzis of FFB Data
        joystick.retrieveFFBData();
        // Playing FFB
        motors.manageFFBData(); 

        //Sending PWM's to the motors
        // Motor 1
        m1PWM.period_us(m1_p);
        m1PWM.pulsewidth_us(m1_d);
        m1DIR = m1_r;
        // Motor 2
        m2PWM.period_us(m2_p);
        m2PWM.pulsewidth_us(m2_d);
        m2DIR = m2_r;
    }
}

void centerWheel(INPUTS *in)
{
    INPUTS * inputs;
    bool leftOK = false, rightOK = false, hasMoved = false;
    int16_t prevEncPulses;
    int16_t encPulses;
    int16_t leftExtremity;
    int16_t rightExtremity;

    inputs = in;

    m1PWM.period(0.004f);
    m1PWM.write(0.8f);
    m2PWM.period(0.004f);
    m2PWM.write(0.8f);
    m1DIR = 1;
    m2DIR = 1;
    wait(0.001); // Delay is needed to prevent reading the changes to fast

    // Make sure the wheel has moved before starting the calibration
    while(!hasMoved)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses != encPulses)
        {
            hasMoved = true;
        }
    }

    // Right extremity
    while(!rightOK)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses == encPulses)
        {
            rightExtremity = encPulses;
            rightOK = true;
        }
    }
    
    m1DIR = 0;
    m2DIR = 0;
    wait(0.001);
    
    // Left extremity
    while(!leftOK)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses == encPulses)
        {
            leftExtremity = encPulses;
            leftOK = true;
        }
    }

    // Updating the wheel resolution from acquired data
    inputs->set_multiplier((float)(leftExtremity-rightExtremity));
    inputs->set_offsetX(rightExtremity);

    m1DIR = 1;
    m2DIR = 1;
    wait(0.001);

    // Centering wheel
    while(!((inputs->get_XAxis() > 16328) && (inputs->get_XAxis() < 16438)))
    {
        wait(0.0001);
    }

    m1PWM.period(0.0f);
    m1PWM.write(0.00f);
    m2PWM.period(0.00f);
    m2PWM.write(0.00f);
}