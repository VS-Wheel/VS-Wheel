/**
 * @file   Inputs.cpp
 * @author The VS-Wheel team (Alexander Selivanov and Pascal Vaillancourt)
 * @date   March 27 2015
 * @brief  All the functions needed for the inputs
 *
 * @version 1.2 : First version of all the working inputs
 * Developpement: Sublime Text 2
 * Compiler: GNU ARM GCC
 *
 * Material:
 *   Microcontroller: LPC 1769 from embedded artists
 *   Programming interface: LPC link 1.3
 *   Working OS: Windows 8.1, also works on Linux debian variants
 */

#include "Inputs.h"

//g25 pedals potentiometers
AnalogIn pot_throttle(POT_THROTTLE);
AnalogIn pot_brake(POT_BRAKE);
AnalogIn pot_clutch(POT_CLUTCH);

//g25 shifter inputs
AnalogIn pot_XAxisShifter(POT_X_AXIS_SHIFTER);
AnalogIn pot_YAxisShifter(POT_Y_AXIS_SHIFTER);
DigitalIn dataInPin(DATA_IN_PIN);
DigitalOut modePin(MODE_PIN);
DigitalOut clockPin(CLOCK_PIN);

//g25 wheel buttons
DigitalIn padRight(PAD_SHIFT_LEFT);
DigitalIn padLeft(PAD_SHIFT_RIGHT);
DigitalIn wheelBtnLeft(WHEEL_BTN_LEFT);
DigitalIn wheelBtnRight(WHEEL_BTN_RIGHT);

USBJoystick * joystick; // Joystick obj
QEI rotEnc(ROTENC_CH1, ROTENC_CH2, ROTENC_INDEX, ROTENC_PULSES, ROTENC_ENCODING); // Rotary encoder obj (Physical Wheel)

void INPUTS::init(USBJoystick *joy)
{
    // Current values
    throttle_ = 0;
    brake_ = 0;
    clutch_ = 0;
    x_ = 0;
    y_ = 0; // The Y axis is not used but it still there for anyone willing to use it
    buttons_ = 0;

    // Previous Values
    prevThrottle_ = 0;
    prevBrake_ = 0;
    prevClutch_ = 0;
    prevX_ = 0;
    prevY_ = 0;
    prevButtons_ = 0;

    joystick = joy; // Object association
}

bool INPUTS::send(void)
{
        // Read the X axis , the Y axis and the buttons
        g25_readShifter();
        // Read the 3 potentimeters
        g25_readPedals();
        // Read the rotary encoder and the buttons on the wheel
        g25_readWheel();

        // Comparing the previous and the new value, before sending data.
        // This prevents flooding the transmission buffer
        if(throttle_ != prevThrottle_ || brake_ != prevBrake_ || clutch_ != prevClutch_ || x_ != prevX_ || y_ != prevY_|| buttons_ != prevButtons_)
        {
            joystick->update(x_, y_, buttons_, throttle_, brake_, clutch_);
        }
        
        // Setting the previous values
        prevThrottle_ = throttle_;
        prevBrake_ = brake_;
        prevClutch_ = clutch_;
        prevX_ = x_;
        prevY_ = y_;
        prevButtons_ = buttons_;

        return true;
}

void INPUTS::g25_readShifter(void)
{
    // Modified by the VS Wheel team
    // The initial code was taken from
    // pascalh from this website:
    // http://www.isrtv.com/forums/topic/13189-diy-g25-shifter-interface-with-h-pattern-sequential-and-handbrake-modes/

    // Shifter state
    static int shift;
    shift = NO_SHIFT;
    static int mode;
    mode = SHIFTER_MODE;

    // Reading of button states from G25 shift register
    static int b[16];

    modePin = 0;                            // Parallel mode: inputs are read into shift register
    wait(0.00005);                           // Wait for signal to settle
    modePin = 1;                            // Serial mode: data bits are output on clock falling edge
    
    for(int i=0; i<16; i++)                 // Iteration over both 8 bit registers
    {
      clockPin = 0;                         // Generate clock falling edge
      wait(0.00005);                         // Wait for signal to settle
      b[i]=dataInPin.read();                // Read data bit and store it into bit array
      clockPin = 1;                         // Generate clock rising edge          
      wait(0.00005);                         // Wait for signal to settle
    }

    // Reading of shifter position
    static int x, y;
    x = (pot_XAxisShifter.read() * 1024);  // X axis
    y = (pot_YAxisShifter.read() * 1024);  // Y axis
    
    // Current gear calculation
    static int gear;                          // Default value is neutral
    gear = 0;

    if(b[DI_MODE]==0)                    // H-shifter mode?
    {
      if(x<HS_XAXIS_12)                  // Shifter on the left?
      {
        if(y>HS_YAXIS_135) gear=1;       // 1st gear
        if(y<HS_YAXIS_246) gear=2;       // 2nd gear
      }
      else if(x>HS_XAXIS_56)             // Shifter on the right?
      {
        if(y>HS_YAXIS_135) gear=5;       // 5th gear
        if(y<HS_YAXIS_246) gear=6;       // 6th gear
      }
      else                               // Shifter is in the middle
      {
        if(y>HS_YAXIS_135) gear=3;       // 3rd gear
        if(y<HS_YAXIS_246) gear=4;       // 4th gear
      }
    }
    else                                 // Sequential mode 
    {
      if(mode==SHIFTER_MODE)             // Shifter mode?
      {
        x = 0;
        if(shift==NO_SHIFT)              // Current state: no shift
        {
          if(y>SS_UPSHIFT_BEGIN)         // Shifter to the front?
          {
            gear=7;                      // Shift-up
            shift=UP_SHIFT;              // New state: up-shift
          }
          if(y<SS_DOWNSHIFT_BEGIN)       // Shifter to the rear?
          {
            gear=8;                      // Shift-down
            shift=DOWN_SHIFT;            // New state: down-shift
          }
        }
        if(shift==UP_SHIFT)              // Current state: up-shift?
        {
          if(y>SS_UPSHIFT_END) gear=7;   // Beyond lower up-shift threshold: up-shift
          else shift=NO_SHIFT;           // Else new state: no shift
        }
        if(shift==DOWN_SHIFT)            // Current state: down-shift
        {
          if(y<SS_DOWNSHIFT_END) gear=8; // Below higher down-shift threshold: down-shift
          else shift=0;                  // Else new state: no shift
        }
      }
    }
    
    if(gear!=6) b[DI_REVERSE]=0;         // Reverse gear is allowed only on 6th gear position
    if(b[DI_REVERSE]==1) gear=0;         // 6th gear is deactivated if reverse gear is engaged
    
    // Set state of virtual buttons for all the physical buttons (including reverse)
    setVirtualButtons(gear, b);
}

void INPUTS::setVirtualButtons(int16_t gear, int btns[16])
{
    // Release virtual buttons for all gears and buttons
    buttons_ = 0;

    // Press virtual button for current gear
    // H-Shifter
    if(gear > 0 && gear <= 6)
    {
      buttons_ = buttons_ | 1 << (gear-1);
    }
    // Sequential shifter
    if(gear >= 7)
    {
      buttons_ = buttons_ | 1 << gear;
    }
    
    //Reverse gear
    buttons_ = buttons_ | btns[DI_REVERSE] << 6;  // The bit shifting is needed to position the reverse gear in the right virtual button

    // DPad + Black buttons on shifter
    for(int i=8;i<=15;i++)
    {
      buttons_ = buttons_ | btns[i] << (5+i);     // the +5 is needed to place the buttons at the wanted position in the virtual buttons
    }

    // Red buttons on shifter
    for(int i=4;i<=7;i++)
    {
      buttons_ = buttons_ | btns[i] << (17+i);    // Same goes for the +17
    }
}

void INPUTS::g25_readPedals(void)
{
  // Set the values of the pedals
  throttle_ = ped_getValue(pot_throttle.read_u16()) ;   // value -127 .. 128
  brake_ = ped_getValue(pot_brake.read_u16());          // value -127 .. 128
  clutch_ = ped_getValue(pot_clutch.read_u16());        // value -127 .. 128
}

void INPUTS::g25_readWheel(void)
{
  x_ = get_XAxis();
  // Read the paddles and the buttons
  buttons_ = buttons_ | padLeft.read() << 9;
  buttons_ = buttons_ | padRight.read() << 10;
  buttons_ = buttons_ | wheelBtnLeft.read() << 11;
  buttons_ = buttons_ | wheelBtnRight.read() << 12;
}

uint8_t INPUTS::ped_getValue(uint16_t value)
{   
    return ((value / PEDDIV) - PEDZERO);
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