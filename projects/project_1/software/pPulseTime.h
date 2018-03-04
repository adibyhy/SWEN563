/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/1/2018
* Project 01:    Timer
* File:          pPulseTime.h
*
*
*********************************************
*/

void timer_init(void);
int pulse_limit_change(void);
int measure_pulse(void);
int display_measurements(void);
void display_main(void);
int display_limits(void);
void measurements_start(void);
