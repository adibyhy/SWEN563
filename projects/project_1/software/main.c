/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/1/2018
* Project 01:    Timer
* File:          main.c
*
*
*********************************************
*/

#include "stm32l476xx.h"
#include "SysClock.h"
#include "UART.h"
#include "post.h"
#include "pPulseTime.h"
#include "error.h"

int main(void)
{
  int error = OK;
	
  System_Clock_Init(); // Switch System Clock = 80 MHz
  UART2_Init();
  inputCapture_Init();  // Initiate POST
  error = post_start();

  if (error < OK)
  {
    USART_Write(USART2, (uint8_t *)"Error occured\r\n", 15);
    return (0);
  }
  
  display_main();
  
  return (0);
}
