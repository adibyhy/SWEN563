/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/1/2018
* Project 01:    Timer
* File:          post.h
*
*
*********************************************
*/

void init_pin_A0(void);
int read_pin_A0(void);
void init_timer(void);
void init_input_capture(void);
void inputCapture_Start(void);
void inputCapture_Stop(void);
int inputCapture_ReadStatus(void);
int inputCapture_ReadCounter(void);
void inputCapture_Init(void);
int post_start(void);
void inputCapture_ClearReadStatus(void);
void EGR_UpdateRegisters(void);
