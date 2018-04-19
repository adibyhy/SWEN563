/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          4/9/2018
* Project 6:     Servo control through QNX Helios
* File:          main.c
*
*
*********************************************
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "SysClock.h"
#include "UART.h"
#include "timer2.h"
#include "LED.h"

#define PRINT_AD      (1)

int main (void)
{ 
  bool run     = true;
  bool measure = false;
  int  AD;
  int  AD_scaled;
  #if PRINT_AD
  char buffer[10];
  #endif  // PRINT_AD
  
  System_Clock_Init();
  UART2_Init();
  LED_Init();
  timer5_init();
  timer2_pwm_init();
  timer2_init_pin_GPIOE();
  
  Red_LED_On();
  
  while(run)
  {
    if((GPIOA->IDR & 0x1) != 0x00)  // if middle joystick button is pushed
    {
      Green_LED_On();
      Red_LED_Off();
      measure = true;
      while((GPIOA->IDR & 0x1) != 0x00);
    }
    
    if (measure == true)
    {
      AD        = get_AD();
      AD_scaled = scale_AD(AD);
      
      timer2_pwm_setPulseWidth(AD_scaled);
      
      #if PRINT_AD
      memset(buffer, 0, sizeof(buffer));    // clear array
      sprintf(buffer, "%u", AD_scaled);     // convert from int to string
      USART_Write(USART2, (uint8_t *)"\r\nAD: ", 6);
      USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
      #endif  // PRINT_AD
      
      timer5_delay();  // 100 msec delay for 20ms period
    }
  }

  return 0;
}

