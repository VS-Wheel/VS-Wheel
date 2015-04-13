/* USBJoystick.h */
/* USB device example: Joystick*/
/* Copyright (c) 2011 ARM Limited. All rights reserved. */
/* Modified Mouse code for Joystick - WH 2012 */

#ifndef USBJOYSTICK_H
#define USBJOYSTICK_H

#include "USBHID.h"

/**
 *
 * USBJoystick example
 * @code
 * #include "mbed.h"
 * #include "USBJoystick.h"
 *
 * USBJoystick joystick;
 *
 * int main(void)
 * {
 *   while (1)
 *   {
 *      joystick.move(20, 0);
 *      wait(0.5);
 *   }
 * }
 *
 * @endcode
 *
 *
 * @code
 * #include "mbed.h"
 * #include "USBJoystick.h"
 * #include <math.h>
 *
 * USBJoystick joystick;
 *
 * int main(void)
 * {
 *   int16_t i = 0;
 *   int16_t throttle = 0;
 *   int16_t rudder = 0;    
 *   int16_t x = 0;
 *   int16_t y = 0;
 *   int32_t radius = 120;
 *   int32_t angle = 0;
 *   int8_t button = 0;    
 *   int8_t hat = 0;    
 *   
 *   while (1) {
 *       // Basic Joystick
 *       throttle = (i >> 8) & 0xFF; // value -127 .. 128
 *       rudder = (i >> 8) & 0xFF;   // value -127 .. 128        
 *       button = (i >> 8) & 0x0F;   // value    0 .. 15, one bit per button     
 *        hat    = (i >> 8) & 0x07;   // value 0..7 or 8 for neutral         
 *       i++;        
 *       
 *       x = cos((double)angle*3.14/180.0)*radius;  // value -127 .. 128
 *       y = sin((double)angle*3.14/180.0)*radius;  // value -127 .. 128
 *       angle += 3;        
 *
 *       joystick.update(throttle, rudder, x, y, button, hat);
 *
 *       wait(0.001);
 *   }
 * }
 * @endcode
 */

// Feature ID 1 (Create New Effect Report)
struct CreateNewEffect{
    uint8_t reportID;
    uint8_t effectType;
    uint16_t byteCount;
};

// Feature ID 2 (Block Load Report)
struct BlockLoadReport{
    uint8_t reportID;
    uint8_t blockIndex;
    uint16_t status;
    uint16_t byteCount;
};

// Feature ID 3 (PID Pool Report)
struct PidPoolReport{
    uint8_t reportID;
    uint16_t ramPoolSize;
    uint8_t simultaneousEffectMax;
    uint8_t deviceManagedPool;
    uint8_t sharedParameterBlock;
};

class USBJoystick: public USBHID {
   public:

        /**
         *   Constructor
         *
         * @param vendor_id Your vendor_id (default: 0x1234)
         * @param product_id Your product_id (default: 0x0002)
         * @param product_release Your product_release (default: 0x0001)
         */
         USBJoystick(uint16_t vendor_id = 0x1337, uint16_t product_id = 0x7331, uint16_t product_release = 0x0001): 
             USBHID(0, 0, vendor_id, product_id, product_release, false)
             { 
                 _init();
                 connect();
             };
         
         /**
         * Write a state of the mouse
         *
         * @param t throttle position
         * @param r rudder position         
         * @param x x-axis position
         * @param y y-axis position
         * @param buttons buttons state
         * @param hat hat state 0 (up), 1 (right, 2 (down), 3 (left) or 4 (neutral)
         * @returns true if there is no error, false otherwise
         */
         bool update(int16_t x, int16_t y, uint32_t buttons, int8_t throttle, int8_t brake, int8_t clutch);


         bool retrieveFFBData();
         
         /*
         * To define the report descriptor. Warning: this method has to store the length of the report descriptor in reportLength.
         *
         * @returns pointer to the report descriptor
         */
         virtual uint8_t * reportDesc();

     private:
        //very important           
         int16_t x_;                       
         int16_t y_;     
         int8_t throttle_;
         int8_t brake_;
         int8_t clutch_;
         uint32_t buttons_;

         void _init();
         // Added functions
         void extractDataOut(void); // Extract the data from the outputReport and place it in the right struct
};

#endif