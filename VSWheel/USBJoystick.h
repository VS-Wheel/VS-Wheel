/* USBJoystick.h */
/* USB device example: Joystick*/
/* Copyright (c) 2011 ARM Limited. All rights reserved. */
/* Modified Mouse code for Joystick - WH 2012 */

#ifndef USBJOYSTICK_H
#define USBJOYSTICK_H

#include "USBHID.h"

// Output ID 1 (Set Effect Report)
struct setEffectReport{
    uint8_t reportID;
    uint8_t effectBlockIndex;
    uint8_t directionEnable;
    uint16_t duration;
    uint16_t triggerRepeatInterval;
    uint16_t samplePeriod;
    uint8_t gain;
    uint8_t triggerButton;
    uint8_t ordinal1;
    uint8_t ordinal2;
};

// Output ID 5 (Set Constant Force Report)
struct setConstantForceReport{
    uint8_t reportID;
    uint8_t effectBlockIndex;
    int16_t magnitude;
};

// Output ID 10 (Effect Operation Report)
struct effectOperationReport{
    uint8_t reportID;
    uint8_t effectBlockIndex;
    uint8_t opEffect;
    uint8_t loopCount;
};

// Output ID 12 (PID Device Control)
struct pidDeviceControl{
    uint8_t reportID;
    uint8_t DC; // Device control
};

// Input ID 2 (PID State Report)
struct pidStateReport{
    uint8_t reportID;
    uint8_t stateReport;
    uint8_t deviceCtrl; // Device control
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
         void update(int16_t x, int16_t y, uint32_t buttons, int8_t throttle, int8_t brake, int8_t clutch);

         bool retrieveFFBData();
         int16_t get_magnitude(void);
         bool get_effectPlaying(void);
         
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
         void responseToHOST(uint8_t id);
};

#endif