/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          timer2.c
*
*
*********************************************
*/

#include <stdio.h>
#include "stm32l476xx.h"
#include "timer2.h"

#define PRESCALER_VALUE           (8000)  // 0.1 milliseconds per count
#define PWM_PULSE_WIDTH           (4)
#define PWM_PULSE_PERIOD          (200)

// Function prototypes
void timer2_EGR_UpdateRegisters(void);
void timer2_init_pin_A0(void);
void timer2_init_pin_A1(void);
void timer2_init_timer2(void);
void timer2_init_output_compare(void);
void timer2_outputCompare_Start(void);
void timer2_outputCompare_Stop(void);
void timer2_pwm_setPulsePeriod(uint16_t pulse_period);

void timer2_init_pin_A0(void)
{
  RCC->AHB2ENR   |= RCC_AHB2ENR_GPIOAEN;   // enable clock for A group of GPIO
  GPIOA->MODER   &= ~GPIO_MODER_MODER0;    // Clear moder 0 bits
  GPIOA->MODER   |= GPIO_MODER_MODER0_1;   // enables alternate function mode
  GPIOA->AFR[0]  |= 1;                     //  Enable AF1
}

void timer2_init_pin_A1(void)
{
  GPIOA->MODER   &= ~GPIO_MODER_MODER1;    // Clear moder 1 bits
  GPIOA->MODER   |= GPIO_MODER_MODER1_1;   // enables alternate function mode
  GPIOA->AFR[0]  |= 0x10;                  //  Enable AF1
}

void timer2_init_timer2(void)
{
  RCC->APB1ENR1 |= 1;             // enable TIM2 clock
  TIM2->PSC = PRESCALER_VALUE;    // load prescaler value
  timer2_EGR_UpdateRegisters();
}

void timer2_EGR_UpdateRegisters(void)
{
  TIM2->EGR |= 1;                 // Force update generation
}

void timer2_init_output_compare(void)
{
  TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;    // Clear OC1M bits
  TIM2->CCMR1 |= 0x60;               // Set PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC1PE;    // Enable output compare 1 preload
  TIM2->CR1   |= TIM_CR1_ARPE;       // Enable auto-reload of preload
  TIM2->CCER  &= ~TIM_CCER_CC1P;     // Output polarity OC1 active high
  TIM2->CCER  |= TIM_CCER_CC1E;      // Enable OC1 output on pin
  
  TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;    // Clear OC2M bits
  TIM2->CCMR1 |= 0x6000;             // Set PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC2PE;    // Enable output compare 2 preload
  TIM2->CCER  &= ~TIM_CCER_CC2P;     // Output polarity OC2 active high
  TIM2->CCER  |= TIM_CCER_CC2E;      // Enable OC2 output on pin

  timer2_EGR_UpdateRegisters();
}

void timer2_outputCompare_Start(void)
{
  TIM2->CR1 |= TIM_CR1_CEN;  // Enable PWM counter
}

void timer2_outputCompare_Stop(void)
{
  TIM2->CR1 &= ~TIM_CR1_CEN;  // Disable PWM counter
}

void timer2_pwm_init(void)
{
  timer2_init_pin_A0();
  timer2_init_pin_A1();
  timer2_init_timer2();
  timer2_init_output_compare();
  timer2_outputCompare_Start();
  
  timer2_pwm_setPulsePeriod(PWM_PULSE_PERIOD);
  timer2_pwm_setPulseWidth(SERVO_0, PWM_PULSE_WIDTH);
  timer2_pwm_setPulseWidth(SERVO_1, PWM_PULSE_WIDTH);
}

void timer2_pwm_setPulseWidth(uint8_t servo, uint8_t pulse_width)
{
  uint8_t count = pulse_width;  // determines the duty cycle. Min: 4(~2%), max: 20(~10%)
  if (servo == SERVO_0)
  {
    TIM2->CCR1 = count;
  }
  if (servo == SERVO_1)
  {
    TIM2->CCR2 = count;
  }
  timer2_EGR_UpdateRegisters();
}

void timer2_pwm_setPulsePeriod(uint16_t pulse_period)
{
  uint16_t count = pulse_period;
  TIM2->ARR = count;
  timer2_EGR_UpdateRegisters();
}


