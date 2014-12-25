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
   report.data[0] = _throttle;
   report.data[1] = _brake;
   report.data[2] = _clutch;            
   report.data[3] = _x & 0xff; 
   report.data[4] = _x >> 8;           
   report.data[5] = _y & 0xff;
   report.data[6] = _y >> 8;            
   report.data[7] = _button & 0xff;
   report.data[8] = _button >> 8;                                                                         
   report.data[9] = _button >> 16;
   report.data[10] = _button >> 24;
   report.length = 11; 
           
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
  //             0x85,0x01,    //    Report ID 1
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
/*
             //need report ID
               0x09,0x21,    //    Usage Set Effect Report (USAGE)
                  0xA1,0x02,    //    Collection Datalink
                     0x85,0x02,    //    Report ID 2
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
                     0xC0,              //    End Collection
                  0xC0              //    End Collection*/
        };

      reportLength = sizeof(reportDescriptor);
      return reportDescriptor;
}


