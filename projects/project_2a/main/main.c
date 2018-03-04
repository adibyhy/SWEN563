/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          main.c
*
*
*********************************************
*/

#include <stdio.h>
#include <stdbool.h>
#include "SysClock.h"
#include "UART.h"
#include "timer2.h"
#include "recipe.h"

int main (void)
{
  //bool runMain = true;
  
  System_Clock_Init();
  UART2_Init();
  timer2_pwm_init();
  
  USART_Write(USART2, (uint8_t *)"Project 2A\r\n", 12);
  USART_Write(USART2, (uint8_t *)"\r>", 2);
  
  recipe_main();
//  while (runMain)
//  {
//    recipe_main();
//  }
  
  return (0);
}


