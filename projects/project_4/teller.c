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
#include <stdio.h>
#include <unistd.h>
#include "teller.h"
#include "metrics.h"

// Definitions
#define TIME_NANOTOSECOND                              (1000000000.0)    // convert time in nanosecond to second
#define TIME_NANOTOMILLI                               (1000000L)        // convert time in nanosecond to nanosecond
#define TIME_SECTOMILLI                                (1000)            // conver time in second to millisecond
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)
#define PRINT_DEBUG_MESSAGE                            (0)

// Variables
pthread_mutex_t teller_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
double teller_getTimeDifference(struct timespec* current, struct timespec* old);
int    teller_getSimulationTime(int time);

// Start

int teller_getSimulationTime(int time)
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
  int              delay_transactionTime;
  double           time_custWaitInQueue;
  double           time_tellerWaitForCustomer;
  struct timespec* time_meetTeller;  // time it takes for the customer to meet the teller
  struct timespec* time_tellerWaitForCust;

  time_meetTeller        = (struct timespec*)malloc(sizeof(time_meetTeller));
  time_tellerWaitForCust = (struct timespec*)malloc(sizeof(time_tellerWaitForCust));

  pthread_mutex_lock(&teller_mutex);
  metrics_getQueueMaxDepth(queue->size);
  pthread_mutex_unlock(&teller_mutex);

  clock_gettime(CLOCK_REALTIME, time_meetTeller);
  teller->time_custMeetTeller = time_meetTeller;
  time_custWaitInQueue        = teller_getTimeDifference(teller->time_custMeetTeller, time_custEnterQueue)*TIME_SECTOMILLI;
  time_custWaitInQueue        = teller_getSimulationTime(time_custWaitInQueue);

  pthread_mutex_lock(&teller_mutex);
  teller->current_customer = queue_front(queue);
  queue_dequeue(queue);
  pthread_mutex_unlock(&teller_mutex);

  delay_transactionTime = teller_getSimulationTime(teller->current_customer);
  delay(delay_transactionTime);

  clock_gettime(CLOCK_REALTIME, time_tellerWaitForCust);
  time_tellerWaitForCustomer = teller_getTimeDifference(time_tellerWaitForCust, teller->time_custMeetTeller)*TIME_SECTOMILLI;
  time_tellerWaitForCustomer = teller_getSimulationTime(time_tellerWaitForCustomer);

#if PRINT_DEBUG_MESSAGE
  printf("TellerID %d doing transaction\n", teller->teller_id);
//  printf("TellerID %d: Queue wait time : %f \n", teller->teller_id, time_custWaitInQueue);
//  printf("TellerID %d: Transaction time: %d \n", teller->teller_id, delay_transactionTime);
//  printf("TellerID %d: Teller wait time: %f \n", teller->teller_id, time_tellerWaitForCustomer);
#endif

  metrics_getCustTransactionTime(delay_transactionTime);
  metrics_getCustQueueWaitTime(time_custWaitInQueue); //sending the customers time in queue to be evaluated for metrics
  metrics_getCustomerServed(teller->teller_id);
  metrics_getTellerWaitTimeMax((int)time_tellerWaitForCustomer);
  metrics_getTellerWaitTimeSum((int)time_tellerWaitForCustomer);
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




