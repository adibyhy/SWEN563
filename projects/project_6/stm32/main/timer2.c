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

#define PRESCALER_VALUE           (8000)  // 0.1 milliseconds per count
#define PWM_PULSE_PERIOD          (200)

#define PULSEWIDTH_POS_0          (4)
#define PULSEWIDTH_POS_1          (8)
#define PULSEWIDTH_POS_2          (10)
#define PULSEWIDTH_POS_3          (13)
#define PULSEWIDTH_POS_4          (16)
#define PULSEWIDTH_POS_5          (24)


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
  GPIOA->MODER   &= ~GPIO_MODER_MODER0_1;  // enables input mode for middle button joystick
  // GPIOA->AFR[0]  |= 0;
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

void timer2_EGR_UpdateRegisters(void)
{
  TIM2->EGR |= 1;                 // Force update generation
}

void timer2_init_output_compare(void)//
{
  /*
  TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;    // Clear OC1M bits
  TIM2->CCMR1 |= 0x60;               // Set PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC1PE;    // Enable output compare 1 preload
  TIM2->CR1   |= TIM_CR1_ARPE;       // Enable auto-reload of preload
  TIM2->CCER  &= ~TIM_CCER_CC1P;     // Output polarity OC1 active high
  TIM2->CCER  |= TIM_CCER_CC1E;      // Enable OC1 output on pin 
  */
  
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
  timer2_pwm_setPulseWidth(PULSEWIDTH_POS_0);
}

void timer2_pwm_setPulseWidth(uint8_t pulse_width)
{
  uint8_t count = pulse_width;  // determines the duty cycle. Min: 4(~2%), max: 20(~10%)
	TIM2->CCR2 = count;
  timer2_EGR_UpdateRegisters();
}

void timer2_pwm_setPulsePeriod(uint16_t pulse_period)
{
  uint16_t count = pulse_period;
  TIM2->ARR = count;
  timer2_EGR_UpdateRegisters();
}

int get_voltage(){ 
	  //read once int16_t val = (GPIOE->IDR &0xffff) 
	  int pe15_bit, pe14_bit, pe13_bit, pe12_bit, pe11_bit, pe10_bit;
    int voltage = 0;
	
	  //int16_t val = (GPIOE->IDR & 0xffff); //change to read once then add the voltage
	  //int32_t x = val... x= x + 32k.... (x*20)/64k 
	  //need to convert from signed to unsigned
	  //val = val&0x3f
	  //val = val ^0x20
	
    pe15_bit  =   (GPIOE->IDR & 0x8000);
    pe14_bit  =   (GPIOE->IDR & 0x4000);
    pe13_bit  =   (GPIOE->IDR & 0x2000);
    pe12_bit  =   (GPIOE->IDR & 0x1000);
    pe10_bit  =   (GPIOE->IDR & 0x400); 
    pe11_bit  =   (GPIOE->IDR & 0x800); 
	
    if (pe15_bit > 0){
      voltage+=1;
    }
    if (pe14_bit > 0){
      voltage+=2;
    }
    if (pe13_bit > 0){
      voltage+=4;
    }
    if (pe12_bit > 0){
      voltage+=8;
    }
    if (pe10_bit > 0){
      voltage+=16;
    }
    if (pe11_bit > 0){
      voltage+=32;
    }	
    
  return voltage;		
}

int scaled_voltage(int pin_voltage)
{
	int new_voltage = pin_voltage;
	  
  if (new_voltage < PULSEWIDTH_POS_0)
  {
    new_voltage = PULSEWIDTH_POS_0;
  }
  
  else if(new_voltage > PULSEWIDTH_POS_5)
  {
    new_voltage = PULSEWIDTH_POS_5;
  }
	
	return new_voltage;
}

