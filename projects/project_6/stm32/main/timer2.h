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

#include "SysClock.h"

void  timer2_pwm_init(void);
void  timer5_init(void);
void  timer5_delay(void);
int   get_AD(void);
int   scale_AD(int AD);
void  timer2_init_pin_GPIOE(void);
void  timer2_pwm_setPulseWidth(uint8_t pulse_width);

#endif /* ndef __timer2_h */
