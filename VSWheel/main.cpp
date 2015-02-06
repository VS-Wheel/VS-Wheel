#include "mbed.h"
#include "rtos.h"
#include "Inputs.h"
#include "Motors.h"

// Variables for Heartbeat and Status monitoring
DigitalOut led_2(P0_22);
DigitalOut dir1(P2_6);
DigitalOut dir2(P2_7);

//Serial serial(P0_0, P0_1); // tx rx

PwmOut pwm1(P2_5);

void led2_thread(void const *args) {
    int i;
    while (true) {
        for(i=1;i<4;i++)
        {
            led_2 = !led_2;
            Thread::wait(i*30);
        }
        for(i=1;i<4;i++)
        {
            led_2 = !led_2;
            Thread::wait(i*100);
        }
    }
}

void motors_thread(void const *args) {
    pwm1.period(0.25f);
    pwm1.write(0.75f);
    while(true) {
    }
}

void joy_thread(void const *args) {
    INPUTS inputs; // Joystick obj (The whole setup)
    USBJoystick joystick;

    inputs.init(&joystick);
    while (true) {
        inputs.send();
        joystick.retrieveFFBData();
        //serial.printf("%d\n\r", requestButtonState(CH9));
        Thread::wait(1);
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