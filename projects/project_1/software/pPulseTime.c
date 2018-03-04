/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/1/2018
* Project 01:    Timer
* File:          pPulseTime.c
*
*
*********************************************
*/

#include "pPulseTime.h"
#include "UART.h"
#include "post.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "error.h"


#define LIMIT_LOWER_MIN                       (50)
#define LIMIT_LOWER_MAX                       (9950)
#define LIMIT_LOWER                           (950)
#define LIMIT_UPPER                           (LIMIT_LOWER + 100)
#define LIMIT_UPPER_ADD                       (100)
#define ARRAY_SIZE                            (10)
#define MEASUREMENTS_MAX_COUNT                (1002)
#define BUCKETS_SIZE                          (101)

static unsigned int pulse_lower_limit;
static unsigned int pulse_upper_limit;

// Function prototypes
void limits_init(void);
void start_prompt(void);

void limits_init(void)
{
  pulse_lower_limit = LIMIT_LOWER;
  pulse_upper_limit = LIMIT_UPPER;
}

int pulse_limit_change(void)
{
  int result = OK;
  char rxByte;
  char input[ARRAY_SIZE];
  unsigned int input_int;
	int cnt = 0;
  
  USART_Write(USART2, (uint8_t *)"\r\nChange limits? (Y/N)\r\n", 24);
  
  rxByte = USART_Read(USART2);
  if (rxByte == 'N' || rxByte == 'n')
  {
   // do nothing
  }
  else if (rxByte == 'Y' || rxByte == 'y')
  {
    USART_Write(USART2, (uint8_t *)"Enter lower limit:\r\n", 20);

	  while (rxByte != '\r')
	  {
	    rxByte = USART_Read(USART2);
	    input[cnt] = rxByte;
      cnt++;
	  }
    
    input[cnt] = '\0';  // insert NULL into the last index
    
    input_int = strtol(input, (char **)NULL, 10);  // convert from string(array) to integer
    
    if ((input_int >= LIMIT_LOWER_MIN) && (input_int <= LIMIT_LOWER_MAX))
    {
      pulse_lower_limit = input_int;
      pulse_upper_limit = pulse_lower_limit + LIMIT_UPPER_ADD;
    }
    else
    {
      USART_Write(USART2, (uint8_t *)"Error. Using default values\r\n", 30);
      pulse_lower_limit = LIMIT_LOWER;
      pulse_upper_limit = LIMIT_UPPER;
    }
  }
  
  display_limits();
  
  return result;
}

int display_limits(void)
{
  int result = OK;
  char buffer[ARRAY_SIZE];
  
  memset(buffer, 0, sizeof(buffer));  // clear array
  
  
  sprintf(buffer, "%u", pulse_lower_limit);  // convert from int to string
  USART_Write(USART2, (uint8_t *)"\r\nLower limit: ", 15);
  USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
  
  sprintf(buffer, "%u", pulse_upper_limit);
  USART_Write(USART2, (uint8_t *)"\t\tUpper limit: ", 15);
  USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
  USART_Write(USART2, (uint8_t *)"\r\n", 2);
  
  return result;
}

void start_prompt(void)
{
  char rxByte;
  USART_Write(USART2, (uint8_t *)"Press Enter to start\r\n", 22);
  while (rxByte != '\r')
  {
    rxByte = USART_Read(USART2);
  }
}

int display_counter_value(unsigned int counter[], int size)
{
  int result = OK;
  char buffer[ARRAY_SIZE];
  int i;
  for (i = 1; i <= size; i++)
  {
		if (counter[i] > 0)
		{
		memset(buffer, 0, sizeof(buffer));  // clear array
		sprintf(buffer, "%u", (pulse_lower_limit + i));  // convert from int to string
		USART_Write(USART2, (uint8_t *)"\r\nTime: ", 8);
    USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
      
    memset(buffer, 0, sizeof(buffer));  // clear array
    sprintf(buffer, "%u", counter[i]);  // convert from int to string
    USART_Write(USART2, (uint8_t *)"  Count: ", 9);
    USART_Write(USART2, (uint8_t *)buffer, sizeof(buffer));
		}
  }
  return result;
}

void measurements_start(void)
{
  unsigned int counter_value_arr[BUCKETS_SIZE];
  int status                       = 0;
  int i                            = 0;
	unsigned int curr_time           = 0;
  unsigned int old_time            = 0;
  unsigned int result              = 0;
  
	memset(counter_value_arr, 0, sizeof(counter_value_arr));
  inputCapture_Start();
  while (i < MEASUREMENTS_MAX_COUNT)
  { 
    status = inputCapture_ReadStatus();
    if (status)
    {
      if (i < 2)
      {
        // remove the first few readings because they are unreliable
        old_time = inputCapture_ReadCounter();
      }
      else
      {
        curr_time = inputCapture_ReadCounter();
        result    = curr_time - old_time;
        old_time  = curr_time;
        counter_value_arr[result-pulse_lower_limit]++;
      }
      i++;
    }
  }
  
  inputCapture_Stop();
  display_counter_value(counter_value_arr, 101);
}

void display_main(void)
{
  limits_init();
  display_limits();
  while(1)
  {
    pulse_limit_change();
    start_prompt();
    measurements_start();
  }
}
