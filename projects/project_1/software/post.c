/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/1/2018
* Project 01:    Timer
* File:          post.c
*
*
*********************************************
*/

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "LED.h"
#include "UART.h"
#include "post.h"
#include "error.h"

#define PRESCALER_VALUE               (79)

void init_pin_A0(void)
{
  RCC->AHB2ENR   |= RCC_AHB2ENR_GPIOAEN;   // enable clock for A group of GPIO
  GPIOA->MODER   &= ~1;
  GPIOA->MODER   |= 2;                     // enables alternate function mode
  GPIOA->AFR[0]  |= 1;                     //  Enable AF1
}

void init_timer(void)
{
  RCC->APB1ENR1 |= 1;             // enable TIM2 clock
  TIM2->PSC = PRESCALER_VALUE;    // load prescaler value
  EGR_UpdateRegisters();
}

void EGR_UpdateRegisters(void)
{
  TIM2->EGR |= 1;                 // Force update generation
}

void init_input_capture(void)
{
  TIM2->CCER  &= ~1;       // Disable output enable of capture input, re-enable when ready to be used
  TIM2->CCMR1 &= ~3;       // Clear bits 1:0
  TIM2->CCMR1 |= 1;        // Configure CC1 channel as input, IC1 mapped on TI1
  TIM2->CCMR1 &= ~0xF0;    // Clear input event filter
  TIM2->CCER  &= ~0xA;     // rising edge active transition
  TIM2->CCER  |= 1;
}

void inputCapture_Start(void)
{
  TIM2->CR1 |= TIM_CR1_CEN;  // Enable counter
}

void inputCapture_Stop(void)
{
  TIM2->CR1 &= 0;  // Disable counter
}

int inputCapture_ReadStatus(void)
{
  return TIM2->SR & 2;  // read bit 1, returns 1 if edge is detected, 0 if no input capture occured
}

void inputCapture_ClearReadStatus(void)
{
  TIM2->SR &= ~2;  // clear read status
}

int inputCapture_ReadCounter(void)
{
  return TIM2->CCR1;  // Read the current timer count when capture event occurs
}

void inputCapture_Init(void)
{
  init_timer();
  init_pin_A0();
  init_input_capture();
}

int post_start(void)
{
  unsigned int IC_status = 0;
  int result             = OK;
  int tries              = 0;
  bool passed            = false;
  char rxByte;

  inputCapture_Start();
  
  USART_Write(USART2, (uint8_t *)"Starting POST...\r\n", 18);
  
  IC_status = inputCapture_ReadStatus();
	
  while (!(passed) && (tries++ < 3))
  {
    if (IC_status)
    {
      USART_Write(USART2, (uint8_t *)"POST passed\r\n", 15);
      passed = true;
    }
    else if (tries < 2)
    {
      USART_Write(USART2, (uint8_t *)"Input not detected. Press Y to retry, N to exit.\r\n", 52);
      rxByte = USART_Read(USART2);
      if (rxByte == 'N' || rxByte == 'n')
      {
        USART_Write(USART2, (uint8_t *)"Exiting\r\n", 9);
        result = ERROR_USER_EXIT;
        break;
      }
      else if (rxByte == 'Y' || rxByte == 'y')
      {
        USART_Write(USART2, (uint8_t *)"Retry\r\n", 7);
        IC_status = inputCapture_ReadStatus();
      }
    }
    else if (tries > 2)
    {
      USART_Write(USART2, (uint8_t *)"Failed. Exiting\r\n", 17);
      result = ERROR_POST_INPUT_NOTCAPTURED;
    }
  }
   
  inputCapture_Stop();
  
  return result;
}
