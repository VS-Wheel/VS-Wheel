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
   
   //report id 1
   //effect block index
   //duration
   //sample period
   //gain
   //type specific block offset 1
   //type specific block offset 2

   //report id 2
   //parameter block offset (bits 7-0)
   //parameter block offset (bits 14-8)
   //Attack level
   //attack time
   //fade level
   //fade time

   //report id 3
   //parameter block offset (bits 7-0)
   //parameter block offset (bits 14-8)
   //cp offset
   //positive coefficient
   //negative coefficient
   //positive saturation
   //negative saturation
   //dead band

   //report id 4
   //parameter block (bits 7-0)
   //parameter block (bits 14-8)
   //magnitude
   //offset
   //phase
   //period

   //report id 5
   //parameter offset (bits 7-0)
   //parameter block offset (bits 14-8)
   //magnitude

   //ramp report that I did not do

   //report id 7
   //parameter block offset (bits 7-0)
   //parameter block offset (bits 14-8)
   //byte count (bits 8-1)
   //custom force data (256 bytes)

   //Some stuff that I remove 

   //report id 9
   //parameter block offset 
   //custom force data offset 
   //sample count

   //report id 10
   //effect block index 
   //effect operation
   //loop count

   //Some stuff that I remove 


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
             //END_COLLECTION(0),
              

             // Force Feedback Descriptor from this website
             // http://www.usb.org/developers/hidpage/pid1_01.pdf

             //Start Force Feedback command definition
             0x05,0x0F,     //  Usage Page(Physical interface)
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
              0x09,0x73,    //  Usage (Set constant Force report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x05,    //  Report ID (5)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x26,0xFD,0x7F,   //  Logical maximum (32765)
                0x75,0x0F,    //  Report size (15)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x24,    //  Usage (ROM flag)
                0x25,0x01,    //  Logical maximum (1)
                0x75,0x01,    //  Report size (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x70,    //  Usage (Magnitude)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x75,0x08,    //  Report size (8)
                0x91,0x02,    //  Output (Dat, Var, Abs)
              0xC0,   //  End collection

              //I skip ramp force for now. I want to test just custom and constant forces, later I need to do it with triangle, sine and etc.
              //Constant force Report definition
              0x09,0x68,    //  Usage (Custom force data report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x05,    //  Report ID (7)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x26,0xFD,0x7F,   //  Logical maximum (32765)
                0x75,0x0F,    //  Report size (15)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x0B,0x3B,0x00,0x01,0x00,    //  Usage (Generic Desktop:Byte count)
                0x26,0x00,0x01,    //  Logical maximum (256)
                0x75,0x0F,    //  Report size (15)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x69,    //  Usage (Custom Force data)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x75,0x08,    //  Report size (8)
                0x96,0x00,0x01,   //  Report count (256)
                0x91,0x02,    //  Output (Dat, Var, Abs)
              0xC0,   //  End collection

              //Download force sample definition
              0x09,0x66,    //  Usage (Download force sample)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x08,    //  Report ID (8)
                0x05,0x01,    //  Usage (Generic Desktop)
                0x09,0x01,    //  Usage (Pointer)
                0xA1,0x02,    //  Collection (Logical)
                  0x09,0x30,    //  Usage (X)
                  0x09,0x31,    //  Usage (Y)
                  0x15,0x81,    //  Logical minimum (-127)
                  0x25,0x7F,    //  Logical maximum (127)
                  0x75,0x08,    //  Report size (8)
                  0x95,0x02,    //  Report count (2)
                  0x91,0x02,    //  Output (Data, Var, Abs)
                0xC0,   //  End collection
              0xC0,   //  End collection

              //Define the custom force parameter block
              0x09,0x6B,    //  Usage (Set custom force report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x09,    //  Report ID (9)
                0x09,0x23,    //  Usage (Parameter block offset)
                0x09,0x6C,    //  Usage (Custom force data offset)
                0x09,0x6D,    //  Usage (Sample count)
                0x15,0x00,    //  Logical minimum (0)
                0x26,0xFD,0x7F,   //  Logical maximum (32765)
                0x95,0x03,    //  Report count (3)
                0x75,0x10,    //  Report size (16)
                0x91,0x02,    //  Output(Data, Var, Abs)
              0xC0,   //  End collection

              //Effect operation report definition
              0x09,0x77,    //  Usage (Effect operation report)
              0xA1,0x02,    //  Collection (Logical)
                0x85,0x0A,    //  Report ID (10)
                0x09,0x22,    //  Usage (Effect block index)
                0x25,0x7F,    //  Logical maximum (127)
                0x75,0x07,    //  Report size (7)
                0x95,0x01,    //  Report count (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x24,    //  Usage (Rom flag)
                0x25,0x01,    //  Logical maximum (1)
                0x75,0x01,    //  Report size (1)
                0x91,0x02,    //  Output (Data, Var, Abs)

                0x09,0x75,    //  Usage (Effect operation)
                0xA1,0x02,    //  Collection (Logical)
                  0x09,0x79,    //  Usage (Op effect start)
                  0x09,0x7A,    //  Usage (Op effect start solo)
                  0x09,0x7B,    //  Usage (Op effect stop)
                  0x15,0x01,    //  Logical minimum (1)
                  0x25,0x03,    //  Logical maximum (3)
                  0x75,0x08,    //  Report size (8)
                  0x91,0x00,    //  Output (Data, Ary, Abs)
                0xC0,   //  End collection

                0x09,0x7C,    //  Usage (Loop count)
                0x15,0x00,    //  Logical minimum (0)
                0x26,0xFF,0x00,   //  Logical maximum (255)
                0x91,0x02,    //  Output (Data, Var, Abs)
              0xC0,   //  End collection
            0xC0
              //I skip the rest of the code because it seems to be useless for our project 




        };

      reportLength = sizeof(reportDescriptor);
      return reportDescriptor;
}


