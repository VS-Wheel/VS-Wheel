#include "mbed.h"
#include "USBJoystick.h"
#include "Inputs.h"
#include "Multiplexer.h"
#include "Motors.h"
#include "QEI.h"
#include "rtos.h"


//USBMouse mouse;
//USBJoystick joystick;
//ROTARY obj

// Variables for Heartbeat and Status monitoring
DigitalOut led_2(P0_22);
DigitalOut dir1(P2_6);
DigitalOut dir2(P2_7);

BusIn busButtons(
    P0_9, P0_8, P0_7, P0_6
);

PwmOut pwm1(P2_5);

AnalogIn pot1(P0_23);
AnalogIn pot2(P0_24);
AnalogIn pot3(P0_25);

void led2_thread(void const *args) {
    while (true) {
        led_2 = !led_2;
        switch(busButtons)
        {
            case 0x01:
            dir1 = 1;
            dir2 = 0;
            break;
            case 0x02:
            dir1 = 0;
            dir2 = 1;
            break;
        }
        Thread::wait(100);
    }
}

void motors_thread(void const *args) {
    pwm1.period(0.25f);
    pwm1.write(0.75f);
    while(true) {
    }
}

void joy_thread(void const *args) {
    USBJoystick joystick; // Joystick obj (The whole setup)
    QEI rotEnc(P0_27, P0_28, NC, 48, QEI::X4_ENCODING); // Rotary encoder obj (Physical Wheel)

    busButtons.mode(OpenDrain); // Kind of internal resistor on the bus

    int8_t throttle = 0;
    int8_t brake = 0;
    int8_t clutch = 0;
    int16_t x = 0;
    int16_t y = 0;
    int32_t radius = 120;
    int32_t angle = 0;
    uint32_t button = 0;

    while (true) {
        throttle = ped_getValue(pot1.read_u16()) & 0xFF; // value -127 .. 128
        brake = ped_getValue(pot2.read_u16()) & 0xFF; // value -127 .. 128
        clutch = ped_getValue(pot3.read_u16()) & 0xFF; // value -127 .. 128

        // Reading button values          
        button = busButtons.read() & 0xFFFFFFFF;     
        //y = pot1.read_u16() & 0xFFFF;
        y = 0;
        x = (rotEnc.getPulses() * 682) & 0xFFFF;

        joystick.update(x, y, button, throttle, brake, clutch);
    }
}

int main() {
    //Start threads
    Thread t1(joy_thread);
    Thread t2(led2_thread);
    Thread t3(motors_thread);
    while (1) {
    }
}