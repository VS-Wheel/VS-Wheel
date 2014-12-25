#include "mbed.h"
#include "USBMouse.h"
#include "USBJoystick.h"

//USBMouse mouse;
USBJoystick joystick;

// Variables for Heartbeat and Status monitoring
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut heartbeatLED(LED4);

Ticker heartbeat;
Serial pc(USBTX, USBRX); // tx, rx

// Heartbeat monitor
void pulse() {
  heartbeatLED = !heartbeatLED;
}

void heartbeat_start() {
  heartbeat.attach(&pulse, 0.5);
}

void heartbeat_stop() {
  heartbeat.detach();
}


int main() {
    int16_t i = 0;
    int16_t throttle = 0;
    int16_t rudder = 0;    
    int16_t x = 0;
    int16_t y = 0;
    int32_t radius = 120;
    int32_t angle = 0;
    int8_t button = 0;    
    int8_t hat = 0;    
    
    pc.printf("Hello World!\n\r");

    heartbeat_start();

    while (1) {
        // Basic Joystick
        throttle = (i >> 8) & 0xFF; // value -127 .. 128
        rudder = (i >> 8) & 0xFF;   // value -127 .. 128        
        button = (i >> 8) & 0x0F;   // value    0 .. 15, one bit per button     
//        hat    = (i >> 8) & 0x03;   // value 0, 1, 2, 3 or 4 for neutral 
        hat    = (i >> 8) & 0x07;   // value 0..7 or 8 for neutral         
        i++;        
        
        x = cos((double)angle*3.14/180.0)*radius;  // value -127 .. 128
        y = sin((double)angle*3.14/180.0)*radius;  // value -127 .. 128
        angle += 3;        

        joystick.update(throttle, rudder, x, y, button, hat);

        wait(0.001);
    }
    pc.printf("Bye World!\n\r");                           
}