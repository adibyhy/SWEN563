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

#define PRINT_AD      (0)

int main (void)
{ 
	bool run     = true;
	bool measure = false;
  int  i       = 0;
	int  pin_voltage;
	int  pin_voltage_scaled;
	
  System_Clock_Init();
  UART2_Init();
  LED_Init();
  timer2_pwm_init();
	timer2_init_pin_GPIOE();
  
  Red_LED_On();
	
  // USART_Write(USART2, (uint8_t *)"Project 6\r\n", 11);
	
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
        pin_voltage = get_voltage();
         
        pin_voltage_scaled = scaled_voltage(pin_voltage);
        
        timer2_pwm_setPulseWidth(pin_voltage_scaled);
        
        #if PRINT_AD         
        char buffer[10];
        memset(buffer, 0, sizeof(buffer));             // clear array
        sprintf(buffer, "%u", pin_voltage_scaled);     // convert from int to string
        USART_Write(USART2, (uint8_t *)"\r\nAD: ", 6);
        USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
        #endif          
        
        for(i = 0; i < 268000; i++); // Delay for 20ms period
      }
    }

  return 0;
}


