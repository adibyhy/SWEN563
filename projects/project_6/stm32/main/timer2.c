/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          4/9/2018
* Project 6:     Servo control through QNX Helios
* File:          timer2.c
*
*
*********************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include "SysClock.h"
#include "stm32l476xx.h"
#include "timer2.h"

#define PRESCALER_VALUE           (79)  // 1 microsecond per count
#define PWM_PULSE_PERIOD          (20000)  // 20 millisecond
#define DELAY_COUNT               (20000)

#define PULSEWIDTH_POS_MIN         (400)
#define PULSEWIDTH_POS_MAX         (2000)

// Function prototypes
void timer2_EGR_UpdateRegisters(void);
void timer2_init_pin_A0(void);
void timer2_init_pin_A1(void);
void timer2_init_timer2(void);
void timer2_init_output_compare(void);
void timer2_outputCompare_Start(void);
void timer2_outputCompare_Stop(void);
void timer2_pwm_setPulsePeriod(uint16_t pulse_period);
void timer5_EGR_UpdateRegisters(void);
void timer5_init_timer5(void);

void timer5_delay(void)
{
  timer5_EGR_UpdateRegisters();
  TIM5->SR &= ~TIM_SR_UIF;  // Clear the update event flag
  TIM5->ARR = DELAY_COUNT;
  // Loop until the update event flag is set
  while (!(TIM5->SR & TIM_SR_UIF));
  // The required time delay has been elapsed
  // User code can be executed
}

void timer5_init_timer5(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;     // enable TIM5 clock
  TIM5->PSC      = PRESCALER_VALUE;         // load prescaler value
  TIM5->CR1     |= TIM_CR1_CEN;             // Enable counter
  timer5_EGR_UpdateRegisters();
}

void timer5_init(void)
{
  timer5_init_timer5();
}

void timer2_init_pin_A0(void)
{
  RCC->AHB2ENR   |= RCC_AHB2ENR_GPIOAEN;   // enable clock for A group of GPIO
  GPIOA->MODER   &= ~GPIO_MODER_MODER0;    // Clear moder 0 bits
  GPIOA->MODER   &= ~GPIO_MODER_MODER0_1;  // enables input mode for middle button joystick
  GPIOA->PUPDR   &= ~0x3;                  // set to pull down
  GPIOA->PUPDR   |= 0x2;
}

void timer2_init_pin_A1(void)
{
  GPIOA->MODER   &= ~GPIO_MODER_MODER1;    // Clear moder 1 bits
  GPIOA->MODER   |= GPIO_MODER_MODER1_1;   // enables alternate function mode
  GPIOA->AFR[0]  |= 0x10;                  //  Enable AF1
}

void timer2_init_pin_GPIOE()
{
  RCC->AHB2ENR   |= RCC_AHB2ENR_GPIOEEN;   // enable clock for E group of GPIO
  GPIOE->MODER   &= 0x000FFFFF;            // Clear bits PE15 -> PE10
}

void timer2_init_timer2(void)
{
  RCC->APB1ENR1 |= 1;             // enable TIM2 clock
  TIM2->PSC = PRESCALER_VALUE;    // load prescaler value
  timer2_EGR_UpdateRegisters();
}

void timer5_EGR_UpdateRegisters(void)
{
  TIM5->EGR |= 1;  // Force update generation
}

void timer2_EGR_UpdateRegisters(void)
{
  TIM2->EGR |= 1;  // Force update generation
}

void timer2_init_output_compare(void)//
{
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
  timer2_pwm_setPulseWidth(PULSEWIDTH_POS_MIN);
}

void timer2_pwm_setPulseWidth(uint16_t pulse_width)
{
  TIM2->CCR2 = pulse_width;  // determines the duty cycle. Min: 4(~2%), max: 20(~10%)
  timer2_EGR_UpdateRegisters();
}

void timer2_pwm_setPulsePeriod(uint16_t pulse_period)
{
  TIM2->ARR = pulse_period;
  timer2_EGR_UpdateRegisters();
}

int get_AD()
{ 
  int pe15_bit, pe14_bit, pe13_bit, pe12_bit, pe11_bit, pe10_bit;
  int AD = 0;

  pe15_bit  =   (GPIOE->IDR & 0x8000);
  pe14_bit  =   (GPIOE->IDR & 0x4000);
  pe13_bit  =   (GPIOE->IDR & 0x2000);
  pe12_bit  =   (GPIOE->IDR & 0x1000);
  pe10_bit  =   (GPIOE->IDR & 0x400); 
  pe11_bit  =   (GPIOE->IDR & 0x800); 

  if (pe15_bit > 0)  // least significant bit
  {
    AD+=1;
  }
  if (pe14_bit > 0)
  {
    AD+=2;
  }
  if (pe13_bit > 0)
  {
    AD+=4;
  }
  if (pe12_bit > 0)
  {
    AD+=8;
  }
  if (pe10_bit > 0)
  {
    AD+=16;
  }
  if (pe11_bit > 0)  // most significant bit
  {
    AD+=32;
  }

  return AD;
}

int scale_AD(int AD)
{
  int pulse_width = (AD*25)+400;
  
  return pulse_width;
}

