#ifndef __timer2_h
#define __timer2_h

/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          timer2.h
*
*
*********************************************
*/

// Defines
#define SERVO_0                     (0)
#define SERVO_1                     (1)


void timer2_pwm_init(void);
void timer2_pwm_setPulseWidth(uint8_t servo, uint8_t pulse_width);











#endif /* ndef __timer2_h */



