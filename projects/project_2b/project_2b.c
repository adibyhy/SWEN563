/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          4/17/2018
 * Project 6:     Control 2 Servos using QNX
 * File:          project_2b.c
 *
 *
 *********************************************
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"
#include "recipe.h"

// Definitions
#define SERVOID_0    (0)
#define SERVOID_1    (1)

// Function prototypes
void  thread_create(void);
void* threadFn_servo0(void* arg);
void* threadFn_servo1(void* arg);


int main(int argc, char *argv[]) {
  int run = 1;
  printf("Welcome to the QNX Momentics IDE\n");

  timer_setup();
  thread_create();

  while(run)
  {
    // do nothing
  }


  return EXIT_SUCCESS;
}

void thread_create(void)
{
  pthread_t thread_0;  // servo 0
  pthread_t thread_1;  // servo 1
  int       result = 0;

  result = pthread_create(&thread_0, NULL, threadFn_servo0, NULL);
  if (result != 0)
  {
    printf("Error: pthread_create() failed\n");
    exit(EXIT_FAILURE);
  }

  result = pthread_create(&thread_1, NULL, threadFn_servo1, NULL);
  if (result != 0)
  {
    printf("Error: pthread_create() failed\n");
    exit(EXIT_FAILURE);
  }
}

void* threadFn_servo0(void* arg)
{
  // run timer 0, channel 1
  recipe_main(SERVOID_0);

  return 0;
}

void* threadFn_servo1(void* arg)
{
  // run timer 0, channel 3
  recipe_main(SERVOID_1);

  return 0;
}









