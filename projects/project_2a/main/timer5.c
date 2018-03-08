/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          timer5.c
*
*
*********************************************
*/

#include <stdio.h>
#include "stm32l476xx.h"
#include "timer5.h"

#define PRESCALER_VALUE           (8000)  // 0.1 milliseconds per count
#define DELAY_COUNT               (1000)   // 100 milliseconds

// Function prototypes
void timer5_EGR_UpdateRegisters(void);
void timer5_init_timer3(void);

void timer5_delay(void)
{
  TIM5->SR &= ~TIM_SR_UIF;  // Clear the update event flag
  TIM5->ARR = DELAY_COUNT;
  // Loop until the update event flag is set
  while (!(TIM5->SR & TIM_SR_UIF));
  // The required time delay has been elapsed
  // User code can be executed
}


void timer5_init_timer3(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;     // enable TIM3 clock
  TIM5->PSC      = PRESCALER_VALUE;         // load prescaler value
  TIM5->CR1     |= TIM_CR1_CEN;             // Enable counter
  timer5_EGR_UpdateRegisters();
}

void timer5_EGR_UpdateRegisters(void)
{
  TIM5->EGR |= 1;  // Force update generation
}

void timer5_init(void)
{
  timer5_init_timer3();
}

