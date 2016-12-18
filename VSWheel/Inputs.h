/**
 * @file   Inputs.h
 * @author The VS-Wheel team (Alexander Selivanov and Pascal Vaillancourt)
 * @date   March 2015
 * @brief  It include all the defines and the functions for the inputs.cpp  
 */

#ifndef INPUTS_H
#define INPUTS_H

#include "mbed.h"
#include "USBJoystick.h"
#include "QEI.h"

#define ROTENC_CH1      p12               // Channel 1
#define ROTENC_CH2      p13               // Channel 2
#define ROTENC_INDEX    NC                  // Index
#define ROTENC_PULSES   60                  // Pulses per revolution
#define ROTENC_ENCODING QEI::X4_ENCODING    // X2 encoding = looks at the state every time a rising or falling edge occurs on channel A
                                            // X4 encoding = looks at the state every time a rising or falling edge occurs on channel A or channel B

#define ROTENC_MAX_VALUE    32767
#define ROTENC_CENTER       16383

//1 & 0 
#define HIGH    1
#define LOW     0

class INPUTS {
   public:          
    /**
     * @brief Init all of the variables and set the address of the joystick object
     * @param The joystick obj created in the main.cpp
     */
        void init(USBJoystick *joy); // Initialize all the inputs
    /**
     * @brief The send function calls g25_readShifter(), g25_readPedals() and g25_readWheel() before 
     * comparing the last values to the new values. Then it sends the data to joystick.update()
     */
        bool send(void); // Send wheel inputs to the PC

        int16_t get_XAxis(void);
        int16_t get_encPulses(void);

        void set_offsetX(int16_t offX);
        void set_multiplier(float difLR);
        
   private:
        /** Global class variable for the throttle */
        int8_t throttle_;
        /** Global class variable for the brake */
        int8_t brake_;
        /** Global class variable for the clutch */
        int8_t clutch_;
        /** Global class variable for the X axis */
        int16_t x_;
        /** Global class variable for the Y axis */
        int16_t y_;
        /** Global class variable for all the buttons used for the wheel */
        uint32_t buttons_;

        /** Global class variable for the previous value of the throttle */
        int8_t prevThrottle_;
        /** Global class variable for the previous value of the brake */
        int8_t prevBrake_;
        /** Global class variable for the previous value of the clutch */
        int8_t prevClutch_;
        /** Global class variable for the previous value of the X axis */
        int16_t prevX_;
        /** Global class variable for the previous value of the Y axis */
        int16_t prevY_;
        /** Global class variable for the previous values of all the buttons used for the wheel */
        uint32_t prevButtons_;

        /** Global class variable for the multiplier value of the wheel */
        float multiplier_;
        /** Global class variable for the offset on the X Axis */
        int16_t offsetXAxis_;
};

#endif