/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          teller.c
 *
 *
 *********************************************
 */

// Includes
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "teller.h"

// Definitions

// Variables
pthread_mutex_t teller_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
struct timespec* teller_getTimeDifference(struct timespec* current, struct timespec* old);

// Start

struct timespec* teller_getTimeDifference(struct timespec* current, struct timespec* old)
{
  struct timespec* result;

  result = (struct timespec*)malloc(sizeof(result));

  result->tv_sec  = current->tv_sec - old->tv_sec;
  result->tv_nsec = current->tv_nsec - old->tv_nsec;

  return result;
}


void teller_runTeller(queue_t* queue, teller_t* teller)
{
  int result;
  struct timespec* time_meetTeller;
  struct timespec* time_custWaitInQueue;

  time_meetTeller = (struct timespec*)malloc(sizeof(time_meetTeller));
  time_custWaitInQueue = (struct timespec*)malloc(sizeof(time_custWaitInQueue));

  result = pthread_mutex_lock(&teller_mutex);

  if (result == EOK)
  {
    // printf("Second: %d nsec:%ld \n ", teller->time_custEnterQueue->tv_sec, teller->time_custEnterQueue->tv_nsec);
    // teller->current_customer = queue_front(queue);
    clock_gettime(CLOCK_REALTIME, time_meetTeller);

    teller->time_custMeetTeller = time_meetTeller;
    time_custWaitInQueue = teller_getTimeDifference(teller->time_custMeetTeller, teller->time_custEnterQueue);
    printf("Second: %d nsec:%ld \n ", time_custWaitInQueue->tv_sec, time_custWaitInQueue->tv_nsec);
  }
  else
  {
    printf ("pthread_mutex_lock(&teller_mutex) failed: %d\n", result);
  }
  result = pthread_mutex_unlock(&teller_mutex);
}

teller_t* teller_createTeller(void)
{
  teller_t* teller;
  teller = (teller_t*)malloc(sizeof(teller));
  return teller;
}

void teller_initTeller(teller_t* teller)
{
  teller->current_customer               = 0;
  teller->time_custEnterQueue    = (struct timespec*)malloc(sizeof(teller->time_custEnterQueue));
  teller->time_custMeetTeller    = (struct timespec*)malloc(sizeof(teller->time_custMeetTeller));
}




