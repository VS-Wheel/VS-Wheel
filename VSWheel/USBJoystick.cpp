/* Copyright (c) 2010-2011 mbed.org, MIT License
* Modified Mouse code for Joystick - WH 2012
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdint.h"
#include "USBJoystick.h"

bool USBJoystick::update(int16_t x, int16_t y, uint32_t button, int8_t throttle, int8_t brake, int8_t clutch) {
             HID_REPORT report;
   _x = x;
   _y = y;
   _button = button;     
   _throttle = throttle;
   _brake = brake;
   _clutch = clutch;

   // Fill the report according to the Joystick Descriptor
   /*
   report.data[0] = 0x01;   // Report ID 1
   report.data[1] = _throttle;
   report.data[2] = _brake;
   report.data[3] = _clutch;            
   report.data[4] = _x & 0xff; 
   report.data[5] = _x >> 8;           
   report.data[6] = _y & 0xff;
   report.data[7] = _y >> 8;            
   report.data[8] = _button & 0xff;
   report.data[9] = _button >> 8;                                                                         
   report.data[10] = _button >> 16;
   report.data[11] = _button >> 24;
   */
   report.data[0] = 0x01;   // Report ID 1
   report.data[1] = _throttle;
   report.data[2] = _brake;
   report.data[3] = _clutch;    
   report.data[4] = _button & 0xff;
   report.data[5] = _button >> 8;                                                                         
   report.data[6] = _button >> 16;
   report.data[7] = _button >> 24;    
   report.data[8] = _x & 0xff; 
   report.data[9] = _x >> 8;           
   report.data[10] = _y & 0xff;
   report.data[11] = _y >> 8;            
   report.data[12] = 0x02;   // Report ID 2

   report.length = 13; 
           
   return send(&report);
}


void USBJoystick::_init() {

   _throttle = -127;
   _brake = -127;
   _clutch = -127;   
   _x = 0;                       
   _y = 0;     
   _button = 0x00000000;
   //_hat = 0x00;              
}


uint8_t * USBJoystick::reportDesc() {    
         static uint8_t reportDescriptor[] = {

             USAGE_PAGE(1), 0x01,           // Generic Desktop           
             LOGICAL_MINIMUM(1), 0x00,      // Logical_Minimum (0)             
             USAGE(1), 0x04,                // Usage (Joystick)
             COLLECTION(1), 0x01,           // Application
               USAGE_PAGE(1), 0x02,            // Simulation Controls
               0x85,0x01,    //    Report ID 1
               USAGE(1), 0xBB,                 // Throttle             
               USAGE(1), 0xC5,                 // Brake
               USAGE(1), 0xC6,                 // Clutch               
               LOGICAL_MINIMUM(1), 0x81,       // -127
               LOGICAL_MAXIMUM(1), 0x7F,       // 127
               REPORT_SIZE(1), 0x08,
               REPORT_COUNT(1), 0x03,          //3, because (throttle,brake,clutch)
               INPUT(1), 0x02,                 // Data, Variable, Absolute               
               USAGE_PAGE(1), 0x01,            // Generic Desktop
               USAGE(1), 0x01,                 // Usage (Pointer)
               COLLECTION(1), 0x00,            // Physical
                 USAGE(1), 0x30,                 // X
                 USAGE(1), 0x31,                 // Y
/*
//  8 bit values
                 LOGICAL_MINIMUM(1), 0x81,       // -127
                 LOGICAL_MAXIMUM(1), 0x7f,       // 127
                 REPORT_SIZE(1), 0x08,
                 REPORT_COUNT(1), 0x02,
                 INPUT(1), 0x02,                 // Data, Variable, Absolute   
*/               
// 16 bit values
                 LOGICAL_MINIMUM(1), 0x00,       // 0
                 LOGICAL_MAXIMUM(2), 0xff, 0x7f, // 32768
                 REPORT_SIZE(1), 0x10,
                 REPORT_COUNT(1), 0x02,
                 INPUT(1), 0x02,                 // Data, Variable, Absolute               
               END_COLLECTION(0),               
//
               USAGE_PAGE(1), 0x09,            // Buttons
               USAGE_MINIMUM(1), 0x00,         // 1
               USAGE_MAXIMUM(1), 0x20,         // 32
               LOGICAL_MINIMUM(1), 0x00,       // 0
               LOGICAL_MAXIMUM(1), 0x01,       // 1
               REPORT_SIZE(1), 0x01,           // 8 bytes
               REPORT_COUNT(1), 0x20,          // 32 bytes
               UNIT_EXPONENT(1), 0x00,         // Unit_Exponent (0)
               UNIT(1), 0x00,                  // Unit (None)                                           
               INPUT(1), 0x02,                 // Data, Variable, Absolute
             END_COLLECTION(0),
              

             // Force Feedback Descriptor from this website
             // http://www.usb.org/developers/hidpage/pid1_01.pdf

             //Start Force Feedback command definition
             0x05,0x0F,     //  Usage Page(Set Effect report)
             //Effect Report Definition
             0x09,0x21,     //  Usage (Set effect report)
             0xA1,0x02,     //  Collection (Logical)
              0x09,0x22,    //  Usage (Effect Block Index)
              0x25,0x7F,    //  Logical Maximum (127)
              0x75,0x07,    //  Report Size (7)
              0x95,0x01,    //  Report Count (1)
              0x91,0x02,    //  Output (Data, Var, Abs)
              0x09,0x24,    //  Usage (Rom Flag)
              0x25,0x01,    //  Logical Maximum (1)
              0x75,0x01,    //  Report Size (1)
              0x91,0x02,    //  Output (Data, Var, Abs)
              //Define the available effect tyes
              0x09,0x25,    //  Usage (Effect Type)
              0xA1,0x02,    //  Collection (Logical)
                0x09,0x26,    //  Usage (Effect Type Constant Force)
                0x09,0x27,    //  Usage (Effect Type Ramp)
                0x09,0x30,    //  Usage (Effect Type Square)
                0x09,0x31,    //  Usage (Effect Type Sine)
                0x09,0x32,    //  Usage (Effect Type Triangle)
                0x09,0x33,    //  Usage (Effect Type Sawtooth Up)
                0x09,0x34,    //  Usage (Effect Type Sawtooth Down)
                0x09,0x40,    //  Usage (Effect Type Spring)
                0x09,0x41,    //  Usage (Effect Type Damper)
                0x09,0x42,    //  Usage (Effect Type Inertia)
                0x15,0x01,    //  Logical minimum (1)
                0x25,0x0A,    //  Logical maximum (10)
                0x91,0x00,    //  Output (Data, Var, Abs)
              0xC0,   //  End Collection

              0x09,0x50,    //  Usage (Duration)
              0x09,0x54,    //  Usage (Trigger Repeat Interval)
              0x15,0x00,    //  Logical Minimum (0)
              0x26,0x10,0x27,   //  Logical Maximum (10000)
              0x46,0x10,0x27,   //  Physical Maximum (10000)
              0x75,0x10,    //  Report size (16)
              0x66,0x03,0x10,   //  Unit (Eng Lin:Time)
              0x55,0x0D,    //  Unit exposant (-3)
              0x95,0x02,    //  Report count (2)
              0x91,0x02,    //  Output (Data, Var, Abs)
              
              0x55,0x0A,    //  Unit exposant (-6)
              0x09,0x51,    //  Usage (Sample period)
              0x95,0x01,    //  Report count (1)
              0x91,0x02,    //  Ouput (Data, Var, Abs)
              0x45,0x00,    //  Physical maximum (0)
              0x55,0x00,    //  Unit exposant(0)
              0x65,0x00,    //  Unit none

              //  So skip a a part in the pdf not necessary I THINK FOR NOW            
              0x05,0x0F,    //  Usage page (Physical interface)
              0x09,0x58,    //  Usage (Type specific block offset)
              0xA1,0x02,    //  Collection (logical)
                0x0B,0x01,0x00,0x0A,0x00,   //  Usage (Ordinals:Instance 1)
                0x0B,0x02,0x00,0x0A,0x00,   //  Usage (Ordinals:Instance 2)
                0x26,0xFD,0x7F,   //  Logical maximum (32765) 32K RAM or ROM max
                0x75,0x10,    //  Report size (16)
                0x95,0x02,    //  Report coumt (2)
                0x91,0x02,    //  Output (Data, Var, Abs)
                0xC0,   //  End collection
              0xC0,   //  End collection 

              //  Envelope Report Definition
              0x09,0x5A,    //  Usage (Set envelope report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x02,    //  Report ID (2)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x26,0xFD,0x7F,   //  Logical maximum (32765) 32K RAM or ROM max
                0x75,0x0F,    //  Report size (15)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x24,    //  Usage (ROM flag)
                0x25,0x01,    //  Logical maximum (1)
                0x75,0x01,    //  Report size (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x5B,    //  Usage (Attack level)
                0x09,0x5D,    //  Usage (Fade level)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x75,0x08,    //  Report size (8)
                0x95,0x02,    //  Report count (2)
                0x91,0x02,    //  Output (Data, Var, Abs)
                
                0x09,0x5C,    //  Usage (Attack time)
                0x09,0x5E,    //  Usage (Fade time)
                0x26,0x10,0x27,   //  Logical maximum (10000)
                0x46,0x10,0x27,   //  Physical maximum (10000)
                0x66,0x03,0x10,   //  Unit (Eng Lin:Time)
                0x55,0x0D,    //  Unit exposant (-3)
                0x75,0x10,    //  Report size (16)
                0x91,0x02,    //  Ouput (Data, Var, Abs)
                0x45,0x00,    //  Physical maximum (0)
                0x65,0x00,    //  Unit none
                0x55,0x00,    //  Unit exponent (0)
              0xC0,   //  End collection

              //Condition report definition
              0x09,0x5F,    //  Usage (Set condition report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x03,    //  Report ID (3)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x26,0xFD,0x7F,   //  Logical maximum (32765)
                0x75,0x0F,    //  Report size (15)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x24,    //  Usage (ROM flag)
                0x25,0x01,    //  Logical maximum (1)
                0x75,0x01,    //  Report size (1)
                0x91,0x02,    //  Output (Data, Var, Abs)
                0x09,0x60,    //  Usage (CP offset)
                0x09,0x61,    //  Usage (Positive coefficient)
                0x09,0x62,    //  Usage (Negative coefficient)
                0x09,0x63,    //  Usage (Positive Saturation)
                0x09,0x64,    //  Usage (Negative saturation)
                0x09,0x65,    //  Usage (Dead band)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x75,0x08,    //  Report size (8)
                0x95,0x06,    //  Report count (6)
                0x91,0x02,    //  Output (Data, Var, Abs)
              0xC0,   //  End collection

              //Periodic report definition
              0x09,0x6E,    //  Usage (Set periodic report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x04,    //  Report ID (4)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x26,0xFD,0x7F,   //  Logical maximum (32765)
                0x75,0x0F,    //  Report size (15)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //Output (Data, Var, Abs)

                0x09,0x24,    //  Usage (ROM flag)
                0x25,0x01,    //  Logical maximum (1)
                0x75,0x01,    //  Report size (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x70,    //  Usage (Magnitude)
                0x09,0x6F,    //  Usage (Offset)
                0x09,0x71,    //  Usage (Phase)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x75,0x08,    //  Report size (8)
                0x95,0x03,    //  Report count (3)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x72,    //  Usage (Period)
                0x26,0xFF,0x27,   //  Logical maximum (10000)
                0x46,0x10,0x27,   //  Physical maximum (10000)
                0x66,0x03,0x10,   //  Unit (Eng Lin:Time)
                0x55,0x0D,    //  Unit exponent (-3)
                0x75,0x10,    //  Report sixe (16)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)
                0x45,0x00,    //  Physical maximum (0)
                0x65,0x00,    //  Unit none
                0x55,0x00,    //  Unit exponent (0)
              0xC0,   //  End collection

              //Constant force Report definition
              
                /****************** Additional **********************************/
     /*          
    //0x06,0x01,0xFF,   //    Usage Page Generic Desktop
    //0x09,0x49,        //    Usage Undefined
    //0x75,0x01,        //    Report Size 1
    //0x95,0x01,        //    Report Count 1
    //0x81,0x02,        //    Input (Variable)
    //0x75,0x07,        //    Report Size 7
    //0x81,0x03,        //    Input (Constant, Variable)
  
  
    //Input
    //Collection Datalink (sub-collection)
    //Physical Interface (Usage: PID State report)
    //ID: 2
    //state report:   5X1bit
    //Padding:      3bit
    //PID Device Control: 1bit
    //Effect Block Index: 7bit  
  
    0x05,0x0F,        //    Usage Page Physical Interface
    0x09,0x92,        //    Usage PID State report 
    0xA1,0x02,        //    Collection Datalink (logical)
       0x85,0x02,    //    Report ID 2
       0x09,0x9F,    //    Usage Device is Pause 
       0x09,0xA0,    //    Usage Actuators Enabled
       0x09,0xA4,    //    Usage Safety Switch
       0x09,0xA5,    //    Usage Actuator Override Switch
       0x09,0xA6,    //    Usage Actuator Power
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x05,    //    Report Count 5
       0x81,0x02,    //    Input (Variable)
       0x95,0x03,    //    Report Count 3
       0x81,0x03,    //    Input (Constant, Variable)
       0x09,0x94,    //    Usage Effect Playing
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x07,    //    Report Size 7
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
    0xC0    ,    // End Collection
   0xC0              //    End Collection
  
   // Output
    //Collection  Datalink:
    //Usage Set Effect Report
    
    //ID:1
    //Effect Block Index: 8bit
    
    //subcollection Effect Type
    //12 effect types, 8bit each
    
  
    
    0x09,0x21,    //    Usage Set Effect Report
    0xA1,0x02,    //    Collection Datalink (Logical)
       0x85,0x01,    //    Report ID 1
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x25,    //    Usage Effect Type
       0xA1,0x02,    //    Collection Datalink
          0x09,0x26,    //    Usage ET Constant Force
          0x09,0x27,    //    Usage ET Ramp
          0x09,0x30,    //    Usage ET Square
          0x09,0x31,    //    Usage ET Sine
          0x09,0x32,    //    Usage ET Triangle
          0x09,0x33,    //    Usage ET Sawtooth Up
          0x09,0x34,    //    Usage ET Sawtooth Down
          0x09,0x40,    //    Usage ET Spring
          0x09,0x41,    //    Usage ET Damper
          0x09,0x42,    //    Usage ET Inertia
          0x09,0x43,    //    Usage ET Friction
          0x09,0x28,    //    Usage ET Custom Force Data
          0x25,0x0C,    //    Logical Maximum Ch (12d)
          0x15,0x01,    //    Logical Minimum 1
          0x35,0x01,    //    Physical Minimum 1
          0x45,0x0C,    //    Physical Maximum Ch (12d)
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output
       0xC0    ,          //    End Collection
       0x09,0x50,         //    Usage Duration
       0x09,0x54,         //    Usage Trigger Repeat Interval
       0x09,0x51,         //    Usage Sample Period
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x03,         //    Report Count 3
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
       0x09,0x52,         //    Usage Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x53,         //    Usage Trigger Button
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x08,         //    Logical Maximum 8
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x08,         //    Physical Maximum 8
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x55,         //    Usage Axes Enable
       0xA1,0x02,         //    Collection Datalink
          0x05,0x01,    //    Usage Page Generic Desktop
          0x09,0x30,    //    Usage X
          0x09,0x31,    //    Usage Y
          0x15,0x00,    //    Logical Minimum 0
          0x25,0x01,    //    Logical Maximum 1
          0x75,0x01,    //    Report Size 1
          0x95,0x02,    //    Report Count 2
          0x91,0x02,    //    Output (Variable)
       0xC0     ,    // End Collection
       0x05,0x0F,    //    Usage Page Physical Interface
       0x09,0x56,    //    Usage Direction Enable
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x95,0x05,    //    Report Count 5
       0x91,0x03,    //    Output (Constant, Variable)
       0x09,0x57,    //    Usage Direction
       0xA1,0x02,    //    Collection Datalink
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x66,0x14,0x00,              //    Unit 14h (20d)
          0x55,0xFE,                   //    Unit Exponent FEh (254d)
          0x15,0x00,                   //    Logical Minimum 0
          0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
          0x35,0x00,                   //    Physical Minimum 0
          0x47,0xA0,0x8C,0x00,0x00,    //    Physical Maximum 8CA0h (36000d)
          0x66,0x00,0x00,              //    Unit 0
          0x75,0x08,                   //    Report Size 8
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
          0x55,0x00,                   //    Unit Exponent 0
          0x66,0x00,0x00,              //    Unit 0
       0xC0     ,         //    End Collection
       0x05,0x0F,         //    Usage Page Physical Interface
       0x09,0xA7,         //    Usage Undefined
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x66,0x00,0x00,    //    Unit 0
       0x55,0x00,         //    Unit Exponent 0
    0xC0     ,    //    End Collection
    0x05,0x0F,    //    Usage Page Physical Interface
    0x09,0x5A,    //    Usage Set Envelope Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x02,         //    Report ID 2
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x5B,         //    Usage Attack Level
       0x09,0x5D,         //    Usage Fade Level
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x5C,         //    Usage Attack Time
       0x09,0x5E,         //    Usage Fade Time
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x91,0x02,         //    Output (Variable)
       0x45,0x00,         //    Physical Maximum 0
       0x66,0x00,0x00,    //    Unit 0
       0x55,0x00,         //    Unit Exponent 0
    0xC0     ,            //    End Collection
    0x09,0x5F,    //    Usage Set Condition Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x03,    //    Report ID 3
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x23,    //    Usage Parameter Block Offset
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x04,    //    Report Size 4
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x58,    //    Usage Type Specific Block Off...
       0xA1,0x02,    //    Collection Datalink
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x75,0x02,                   //    Report Size 2
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
       0xC0     ,         //    End Collection
       0x15,0x80,         //    Logical Minimum 80h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x60,         //    Usage CP Offset
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x61,         //    Usage Positive Coefficient
       0x09,0x62,         //    Usage Negative Coefficient
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x63,         //    Usage Positive Saturation
       0x09,0x64,         //    Usage Negative Saturation
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x65,         //    Usage Dead Band
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x6E,    //    Usage Set Periodic Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x04,                   //    Report ID 4
       0x09,0x22,                   //    Usage Effect Block Index
       0x15,0x01,                   //    Logical Minimum 1
       0x25,0x28,                   //    Logical Maximum 28h (40d)
       0x35,0x01,                   //    Physical Minimum 1
       0x45,0x28,                   //    Physical Maximum 28h (40d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x70,                   //    Usage Magnitude
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x6F,                   //    Usage Offset
       0x15,0x80,                   //    Logical Minimum 80h (-128d)
       0x25,0x7F,                   //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,              //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x71,                   //    Usage Phase
       0x66,0x14,0x00,              //    Unit 14h (20d)
       0x55,0xFE,                   //    Unit Exponent FEh (254d)
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x47,0xA0,0x8C,0x00,0x00,    //    Physical Maximum 8CA0h (36000d)
       0x91,0x02,                   //    Output (Variable)
       0x09,0x72,                   //    Usage Period
       0x26,0xFF,0x7F,              //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,              //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,              //    Unit 1003h (4099d)
       0x55,0xFD,                   //    Unit Exponent FDh (253d)
       0x75,0x10,                   //    Report Size 10h (16d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x66,0x00,0x00,              //    Unit 0
       0x55,0x00,                   //    Unit Exponent 0
    0xC0     ,    // End Collection
    0x09,0x73,    //    Usage Set Constant Force Rep...
    0xA1,0x02,    //    Collection Datalink
       0x85,0x05,         //    Report ID 5
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x70,         //    Usage Magnitude
       0x16,0x01,0xFF,    //    Logical Minimum FF01h (-255d)
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x74,    //    Usage Set Ramp Force Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x06,         //    Report ID 6
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x75,         //    Usage Ramp Start
       0x09,0x76,         //    Usage Ramp End
       0x15,0x80,         //    Logical Minimum 80h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x68,    //    Usage Custom Force Data Rep...
    0xA1,0x02,    //    Collection Datalink
       0x85,0x07,         //    Report ID 7
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6C,         //    Usage Custom Force Data Offset
       0x15,0x00,         //    Logical Minimum 0
       0x26,0x10,0x27,    //    Logical Maximum 2710h (10000d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x69,         //    Usage Custom Force Data
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x0C,         //    Report Count Ch (12d)
       0x92,0x02,0x01,    //       Output (Variable, Buffered)
    0xC0     ,    //    End Collection
    0x09,0x66,    //    Usage Download Force Sample
    0xA1,0x02,    //    Collection Datalink
       0x85,0x08,         //    Report ID 8
       0x05,0x01,         //    Usage Page Generic Desktop
       0x09,0x30,         //    Usage X
       0x09,0x31,         //    Usage Y
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,   //    End Collection
    0x05,0x0F,   //    Usage Page Physical Interface
    0x09,0x77,   //    Usage Effect Operation Report
    0xA1,0x02,   //    Collection Datalink
       0x85,0x0A,    //    Report ID Ah (10d)
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x78,    //    Usage Effect Operation
       0xA1,0x02,    //    Collection Datalink
          0x09,0x79,    //    Usage Op Effect Start
          0x09,0x7A,    //    Usage Op Effect Start Solo
          0x09,0x7B,    //    Usage Op Effect Stop
          0x15,0x01,    //    Logical Minimum 1
          0x25,0x03,    //    Logical Maximum 3
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output
       0xC0     ,         //    End Collection
       0x09,0x7C,         //    Usage Loop Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x90,    //    Usage PID Block Free Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0B,    //    Report ID Bh (11d)
       0x09,0x22,    //    Usage Effect Block Index
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x96,    //    Usage PID Device Control
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0C,    //    Report ID Ch (12d)
       0x09,0x97,    //    Usage DC Enable Actuators
       0x09,0x98,    //    Usage DC Disable Actuators
       0x09,0x99,    //    Usage DC Stop All Effects
       0x09,0x9A,    //    Usage DC Device Reset
       0x09,0x9B,    //    Usage DC Device Pause
       0x09,0x9C,    //    Usage DC Device Continue
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x06,    //    Logical Maximum 6
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x00,    //    Output
    0xC0     ,    //    End Collection
    0x09,0x7D,    //    Usage Device Gain Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0D,         //    Report ID Dh (13d)
       0x09,0x7E,         //    Usage Device Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,            //    End Collection
    0x09,0x6B,    //    Usage Set Custom Force Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0E,         //    Report ID Eh (14d)
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6D,         //    Usage Sample Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x51,         //    Usage Sample Period
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
    0xC0     ,    //    End Collection
    0x09,0xAB,    //    Usage Create New Effect Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x01,    //    Report ID 1
       0x09,0x25,    //    Usage Effect Type
       0xA1,0x02,    //    Collection Datalink
       0x09,0x26,    //    Usage ET Constant Force
       0x09,0x27,    //    Usage ET Ramp
       0x09,0x30,    //    Usage ET Square
       0x09,0x31,    //    Usage ET Sine
       0x09,0x32,    //    Usage ET Triangle
       0x09,0x33,    //    Usage ET Sawtooth Up
       0x09,0x34,    //    Usage ET Sawtooth Down
       0x09,0x40,    //    Usage ET Spring
       0x09,0x41,    //    Usage ET Damper
       0x09,0x42,    //    Usage ET Inertia
       0x09,0x43,    //    Usage ET Friction
       0x09,0x28,    //    Usage ET Custom Force Data
       0x25,0x0C,    //    Logical Maximum Ch (12d)
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x0C,    //    Physical Maximum Ch (12d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0xB1,0x00,    //    Feature
    0xC0     ,    // End Collection
    0x05,0x01,         //    Usage Page Generic Desktop
    0x09,0x3B,         //    Usage Reserved
    0x15,0x00,         //    Logical Minimum 0
    0x26,0xFF,0x01,    //    Logical Maximum 1FFh (511d)
    0x35,0x00,         //    Physical Minimum 0
    0x46,0xFF,0x01,    //    Physical Maximum 1FFh (511d)
    0x75,0x0A,         //    Report Size Ah (10d)
    0x95,0x01,         //    Report Count 1
    0xB1,0x02,         //    Feature (Variable)
    0x75,0x06,         //    Report Size 6
    0xB1,0x01,         //    Feature (Constant)
 0xC0     ,    //    End Collection
 0x05,0x0F,    //    Usage Page Physical Interface
 0x09,0x89,    //    Usage Block Load Status
 0xA1,0x02,    //    Collection Datalink
    0x85,0x02,    //    Report ID 2
    0x09,0x22,    //    Usage Effect Block Index
    0x25,0x28,    //    Logical Maximum 28h (40d)
    0x15,0x01,    //    Logical Minimum 1
    0x35,0x01,    //    Physical Minimum 1
    0x45,0x28,    //    Physical Maximum 28h (40d)
    0x75,0x08,    //    Report Size 8
    0x95,0x01,    //    Report Count 1
    0xB1,0x02,    //    Feature (Variable)
    0x09,0x8B,    //    Usage Block Load Status
    0xA1,0x02,    //    Collection Datalink
       0x09,0x8C,    //    Usage Block Load Success
       0x09,0x8D,    //    Usage Block Load Full
       0x09,0x8E,    //    Usage Block Load Error
       0x25,0x03,    //    Logical Maximum 3
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x03,    //    Physical Maximum 3
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0xB1,0x00,    //    Feature
    0xC0     ,                   // End Collection
    0x09,0xAC,                   //    Usage Undefined
    0x15,0x00,                   //    Logical Minimum 0
    0x27,0xFF,0xFF,0x00,0x00,    //    Logical Maximum FFFFh (65535d)
    0x35,0x00,                   //    Physical Minimum 0
    0x47,0xFF,0xFF,0x00,0x00,    //    Physical Maximum FFFFh (65535d)
    0x75,0x10,                   //    Report Size 10h (16d)
    0x95,0x01,                   //    Report Count 1
    0xB1,0x00,                   //    Feature
 0xC0     ,    //    End Collection
 0x09,0x7F,    //    Usage PID Pool Report
 0xA1,0x02,    //    Collection Datalink
    0x85,0x03,                   //    Report ID 3
    0x09,0x80,                   //    Usage RAM Pool size
    0x75,0x10,                   //    Report Size 10h (16d)
    0x95,0x01,                   //    Report Count 1
    0x15,0x00,                   //    Logical Minimum 0
    0x35,0x00,                   //    Physical Minimum 0
    0x27,0xFF,0xFF,0x00,0x00,    //    Logical Maximum FFFFh (65535d)
    0x47,0xFF,0xFF,0x00,0x00,    //    Physical Maximum FFFFh (65535d)
    0xB1,0x02,                   //    Feature (Variable)
    0x09,0x83,                   //    Usage Simultaneous Effects Max
    0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
    0x46,0xFF,0x00,              //    Physical Maximum FFh (255d)
    0x75,0x08,                   //    Report Size 8
    0x95,0x01,                   //    Report Count 1
    0xB1,0x02,                   //    Feature (Variable)
    0x09,0xA9,                   //    Usage Device Managed Pool
    0x09,0xAA,                   //    Usage Shared Parameter Blocks
    0x75,0x01,                   //    Report Size 1
    0x95,0x02,                   //    Report Count 2
    0x15,0x00,                   //    Logical Minimum 0
    0x25,0x01,                   //    Logical Maximum 1
    0x35,0x00,                   //    Physical Minimum 0
    0x45,0x01,                   //    Physical Maximum 1
    0xB1,0x02,                   //    Feature (Variable)
    0x75,0x06,                   //    Report Size 6
    0x95,0x01,                   //    Report Count 1
    0xB1,0x03,                   //    Feature (Constant, Variable)
    0xC0,    //    End Collection
    0xC0        // End Collection      
    */ 

        };

      reportLength = sizeof(reportDescriptor);
      return reportDescriptor;
}


