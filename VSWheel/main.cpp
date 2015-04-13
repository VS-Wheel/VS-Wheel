#include "mbed.h"
#include "rtos.h"
#include "Inputs.h"
#include "Motors.h"

// Variables for Heartbeat and Status monitoring
DigitalOut led_2(P0_22);

// PWM direction for motor 1
DigitalOut m1DIR(P0_9);
// PWM direction for motor 2
DigitalOut m2DIR(P0_8);

// PWM output for the first motor
PwmOut m1PWM(P2_0);
// PWM output for the second motor
PwmOut m2PWM(P2_1);

void centerWheel(INPUTS *in);

// Mail between threads
typedef struct {
  // Motor 1
  float m1_period; // PWM period
  float m1_duty; // PWM duty cycle
  bool  m1_dir; // Rotation direction
  // Motor 2
  float m2_period;
  float m2_duty;
  bool  m2_dir;
} mail_t;

Mail<mail_t, 1> mailbox;

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
    float m1_p = 0.00f, m1_d  = 0.00f;
    float m2_p = 0.00f, m2_d = 0.00f;
    bool m1_r = 0, m2_r = 0;

    while(true) {
        osEvent event = mailbox.get();
        if (event.status == osEventMail) {
            mail_t *mail = (mail_t*)event.value.p;
            m1_p = mail->m1_period;
            m1_d = mail->m1_duty;
            m1_r = mail->m1_dir;
            m2_p = mail->m2_period;
            m2_d = mail->m2_duty;
            m2_r = mail->m2_dir;
            mailbox.free(mail);
        }

        // Motor 1
        m1PWM.period(m1_p);
        m1PWM.write(m1_d);
        m1DIR = m1_r;
        // Motor 2
        m2PWM.period(m2_p);
        m2PWM.write(m2_d);
        m2DIR = m2_r;

        Thread::wait(1);
    }
}

void joy_thread(void const *args) {
    INPUTS inputs; // Joystick obj (The whole setup)
    MOTORS motors;
    USBJoystick joystick;
    float m1_p = 0.00f, m1_d  = 0.00f;
    float m2_p = 0.00f, m2_d = 0.00f;
    bool m1_r = 0, m2_r = 0;

    inputs.init(&joystick);
    motors.init(&inputs, &m1_p, &m1_d, &m1_r, &m2_p, &m2_d, &m2_r);
    centerWheel(&inputs);

    while (true) {
        inputs.send();
        joystick.retrieveFFBData();

        mail_t *mail = mailbox.alloc();
        mail->m1_period = m1_p;
        mail->m1_duty = m1_d;
        mail->m1_dir = m1_r;
        mail->m2_period = m2_p;
        mail->m2_duty = m2_d;
        mail->m2_dir = m2_r;
        mailbox.put(mail);

        Thread::wait(1);
    }
}

int main() {
    //Start threads
    Thread t1(joy_thread);
    Thread t2(led2_thread);
    Thread t3(motors_thread);
    while (true) {
    }
}

void centerWheel(INPUTS *in)
{
    INPUTS * inputs;
    bool leftOK = false, rightOK = false, hasMoved = false;
    int16_t prevEncPulses;
    int16_t encPulses;
    int16_t leftExtremity;
    int16_t rightExtremity;

    inputs = in;

    m1PWM.period(0.01f);
    m1PWM.write(0.6f);
    m2PWM.period(0.01f);
    m2PWM.write(0.6f);
    m1DIR = 1;
    m2DIR = 1;
    wait(0.005); // Delay is needed to prevent reading the changes to fast

    // Make sure the wheel has moved before starting the centering
    while(!hasMoved)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses != encPulses)
        {
            hasMoved = true;
        }
    }

    while(!rightOK)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses == encPulses)
        {
            rightExtremity = encPulses;
            rightOK = true;
        }
    }
    
    m1DIR = 0;
    m2DIR = 0;
    wait(0.005);
    
    while(!leftOK)
    {
        prevEncPulses = (int16_t)inputs->get_encPulses();
        wait(0.005);
        encPulses = (int16_t)inputs->get_encPulses();
        if(prevEncPulses == encPulses)
        {
            leftExtremity = encPulses;
            leftOK = true;
        }
    }

    //inputs->set_DEBUG(rightExtremity); // Updates the Y axis with the value we want to print
    inputs->set_multiplier((float)leftExtremity-rightExtremity);
    inputs->set_offsetX(rightExtremity);
    inputs->send();

    m1PWM.period(0.01f);
    m1PWM.write(0.3f);
    m2PWM.period(0.01f);
    m2PWM.write(0.3f);
    m1DIR = 1;
    m2DIR = 1;
    wait(0.005);

    //while(!((inputs->get_XAxis() > 16328) && (inputs->get_XAxis() < 16438)))
    while(!((inputs->get_XAxis() > 16358) && (inputs->get_XAxis() < 16408)))
    {
        inputs->send();
        wait(0.0001);
    }

    m1PWM.period(0.0f);
    m1PWM.write(0.00f);
    m2PWM.period(0.00f);
    m2PWM.write(0.00f);
    m1DIR = 0;
    m2DIR = 0;
}