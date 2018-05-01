/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          4/17/2018
 * Project 6:     Control 2 Servos using QNX
 * File:          timer.h
 *
 *
 *********************************************
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/neutrino.h>  // for timers
#include <stdint.h>

// Function prototypes
int     timer_main(int servo_id);
void    timer_setup(void);
timer_t timer_create_pulse_timer(int *ptr_channel_id);
void    timer_start_timer(time_t timer_id, int timeOutSec, int timeOutNsec, int periodSec, int periodNsec);
void    timer_pwm_setPulseWidth(uint8_t servo, uint8_t pulse_width);
void    timer_runPWM(int channel_id, int dutyCycle_count);

#endif /* TIMER_H_ */
