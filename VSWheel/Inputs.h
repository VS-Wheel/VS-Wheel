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
#include "Multiplexer.h"
#include "QEI.h"

#define PEDDIV 258 							// Pedal divider
#define PEDZERO 127 						// Pedal zero substraction

#define ROTENC_CH1      P0_10               // Channel 1
#define ROTENC_CH2      P0_11               // Channel 2
#define ROTENC_INDEX    NC                  // Index
#define ROTENC_PULSES   60                  // Pulses per revolution
#define ROTENC_ENCODING QEI::X4_ENCODING    // X2 encoding = looks at the state every time a rising or falling edge occurs on channel A
                                            // X4 encoding = looks at the state every time a rising or falling edge occurs on channel A or channel B
#define ROTENC_MAX_VALUE	32767
#define ROTENC_CENTER		16383	

#define PAD_SHIFT_LEFT		P0_0
#define PAD_SHIFT_RIGHT		P0_1
#define WHEEL_BTN_LEFT		P0_7
#define WHEEL_BTN_RIGHT		P0_6

#define POT_THROTTLE		P0_23
#define POT_BRAKE			P0_24
#define POT_CLUTCH			P0_25
#define POT_X_AXIS_SHIFTER	P1_31
#define POT_Y_AXIS_SHIFTER	P1_30

#define DATA_IN_PIN        P0_4
#define MODE_PIN           P0_18
#define CLOCK_PIN          P0_5

// H-shifter mode analog axis thresholds
#define HS_XAXIS_12        350
#define HS_XAXIS_56        680
#define HS_YAXIS_135       800
#define HS_YAXIS_246       100

// Sequential shifter mode analog axis thresholds
#define SS_UPSHIFT_BEGIN   570
#define SS_UPSHIFT_END     650
#define SS_DOWNSHIFT_BEGIN 285
#define SS_DOWNSHIFT_END   240

// Digital inputs definitions
#define DI_REVERSE         1
#define DI_MODE            3

// Shifter state
#define DOWN_SHIFT         -1
#define NO_SHIFT           0
#define UP_SHIFT           1

// Shifter mode
#define SHIFTER_MODE       0

//1 & 0 
#define HIGH	1
#define LOW		0

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
		void send(void); // Send wheel inputs to the PC

		int16_t get_XAxis(void);
		int16_t get_encPulses(void);

		void set_offsetX(int16_t offX);
		void set_multiplier(float difLR);
		void set_DEBUG(int16_t Y);
		
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
        
    /**
	 * @brief After the g25 shifter reads the buttons and the shifter axes, this function is called to setup
	 * the virtual buttons in a predefined order
	 * @param Current gear of the shifter (1 to 6, 7 == reverse, 8 and 9 is for the 
	 * sequential shifter mode)
	 * @param The buffer containing the button values
	 */
        void setVirtualButtons(int16_t gear, int btns[16]);

    /**
	 * @brief Reads the potentiometers value of the g25_Pedals (Throttle, brake, clutch)
	 */
        void g25_readPedals(void); 
    /**
	 * @brief Reads rotary encoder and convert to the size of report descriptor 32767,
	 * reads the paddle switches and the buttons on the wheel 
	 */             
        void g25_readWheel(void);
    /**
	 * @brief Reads the X and Y axis of the shifter + all the button states from the
	 * 2 x 8 bit shift register
	 */				
        void g25_readShifter(void);				// Reads the shifter of the g25
    /**
	 * @brief Reads the potentiometers value of the g25_Pedals (Throttle, brake, clutch)
	 * @param 16 bit value received from the ADC 
	 * @return Return an uint8 and formats it for the report descriptor size
	 */
        uint8_t ped_getValue(uint16_t value); 	// returns 8 bit value
};

#endif