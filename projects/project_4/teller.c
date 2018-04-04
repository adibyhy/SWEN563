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
#include <unistd.h>
#include "teller.h"

// Definitions
#define TIME_NANOTOSECOND                              (1000000000.0)    // convert time in nanosecond to second
#define TIME_NANOTOMILLI                               (1000000L)       // convert time in nanosecond to nanosecond
#define TIME_SECTOMILLI                                (1000)           // conver time in second to millisecond
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)
#define PRINT_DEBUG_MESSAGE                            (1)

// Variables
pthread_mutex_t teller_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
double teller_getTimeDifference(struct timespec* current, struct timespec* old);
int teller_getDelayTime(int time);

// Start

int teller_getDelayTime(int time)
{
  float result;
  result = time*TIME_CONVERTTO_SIMULATIONMILLISECOND;
  return (int)result;
}

double teller_getTimeDifference(struct timespec* current, struct timespec* old)
{
  double result;

  result = (current->tv_sec - old->tv_sec) + (double)(current->tv_nsec - old->tv_nsec)/TIME_NANOTOSECOND;

  return result;
}


void teller_runTeller(queue_t* queue, teller_t* teller, struct timespec* time_custEnterQueue)
{
  int              result;
  double           time_custWaitInQueue;
  double           time_tellerWaitForCustomer;
  struct timespec* time_meetTeller;  // time it takes for the customer to meet the teller
  struct timespec* time_tellerWaitForCust;

  time_meetTeller        = (struct timespec*)malloc(sizeof(time_meetTeller));
  time_tellerWaitForCust = (struct timespec*)malloc(sizeof(time_tellerWaitForCust));

  result = pthread_mutex_lock(&teller_mutex);

  if (result == EOK)
  {
    clock_gettime(CLOCK_REALTIME, time_meetTeller);
    teller->time_custMeetTeller = time_meetTeller;
    time_custWaitInQueue        = teller_getTimeDifference(teller->time_custMeetTeller, time_custEnterQueue)*TIME_SECTOMILLI;

    teller->current_customer = queue_front(queue);
    queue_dequeue(queue);
    delay(teller_getDelayTime(teller->current_customer)-1);

    clock_gettime(CLOCK_REALTIME, time_tellerWaitForCust);
    time_tellerWaitForCustomer = teller_getTimeDifference(teller->time_custMeetTeller, time_tellerWaitForCust);

    if (teller->time_tellerWaitForCustMax < (int)time_tellerWaitForCustomer)
    {
      teller->time_tellerWaitForCustMax = (int)time_tellerWaitForCustomer;
    }
#if PRINT_DEBUG_MESSAGE
    printf("TellerID %d: Queue wait time : %f \n", teller->teller_id, time_custWaitInQueue);
    printf("TellerID %d: Transaction time: %d \n", teller->teller_id, teller->current_customer);
#endif

    result = pthread_mutex_unlock(&teller_mutex);
  }
  else
  {
    printf ("pthread_mutex_lock(&teller_mutex) failed: %d\n", result);
  }
}

teller_t* teller_createTeller(void)
{
  teller_t* teller;
  teller = (teller_t*)malloc(sizeof(teller));
  return teller;
}

void teller_initTeller(teller_t* teller, int id)
{
  teller->current_customer       = 0;
  teller->teller_id              = id;
  teller->time_custEnterQueue    = (struct timespec*)malloc(sizeof(teller->time_custEnterQueue));
  teller->time_custMeetTeller    = (struct timespec*)malloc(sizeof(teller->time_custMeetTeller));
}




