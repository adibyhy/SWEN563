/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          project_4.c
 *
 *
 *********************************************
 */

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "queue.h"
#include "teller.h"
#include "metrics.h"

// Definitions
#define THREADS_MAX                                    (4)                // Number of threads to be created
#define QUEUE_ARRIVALTIME_MIN                          (60)               // second
#define QUEUE_ARRIVALTIME_MAX                          (240)              // second
#define QUEUE_TRANSACTIONTIME_MIN                      (30)               // second
#define QUEUE_TRANSACTIONTIME_MAX                      (480)              // second
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)
#define TIME_MILLISECONDTONANO                         (1000000L)    // convert time in millisecond to nanosecond
#define BUSINESS_HOURS                                 (42000)  // in millisecond, 42 second = 7 hours 0900-1600
#define TELLER0_ID                                     (0)
#define TELLER1_ID                                     (1)
#define TELLER2_ID                                     (2)
#define PRINT_DEBUG_MESSAGE                            (0)

// Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
queue_t*        g_queue;
teller_t*       g_teller0;
teller_t*       g_teller1;
teller_t*       g_teller2;
static bool     g_bank_open = false;
struct timespec time_cust_enterQueue;

// Function prototypes
void    thread_create(void);
void*   threadFn_queue(void* arg);
void*   threadFn_teller0(void* arg);
void*   threadFn_teller1(void* arg);
void*   threadFn_teller2(void* arg);
int     RNG_get(int lower, int upper);

// Start

int RNG_get(int lower, int upper)
{
  double rand_num;
  int    range;
  int    rand_scaled;

  rand_num       = rand()/(1.0 + RAND_MAX);
  range          = upper - lower + 1;
  rand_scaled    = (rand_num * range) + lower;

  return rand_scaled;
}

void thread_create(void)
{
  pthread_t thread_0;
  pthread_t thread_1;
  pthread_t thread_2;
  pthread_t thread_3;
  void*     threads[]   = {&thread_0, &thread_1, &thread_2, &thread_3};
  void*     threadsFn[] = {threadFn_queue, threadFn_teller0, threadFn_teller1, threadFn_teller2};
  int       result = 0;
  int       k;

  for (k = 0; k < THREADS_MAX; k++)
  {
    result = pthread_create(threads[k], NULL, threadsFn[k], (void*)k);
    if (result != 0)
    {
      printf("Error: pthread_create() failed\n");
      exit(EXIT_FAILURE);
    }
  }
}

void* threadFn_queue(void* arg)
{
  int   newCust_arrivalTime;
  int   newCust_transactionTime;
  float delay_time;

//  printf("This is threadFn_queue(), thread number is %d\n", (int) arg);

  while (1)
  {
    if (g_bank_open)
    {
      newCust_arrivalTime     = RNG_get(QUEUE_ARRIVALTIME_MIN, QUEUE_ARRIVALTIME_MAX);
      newCust_transactionTime = RNG_get(QUEUE_TRANSACTIONTIME_MIN, QUEUE_TRANSACTIONTIME_MAX);
      delay_time              = newCust_arrivalTime*(TIME_CONVERTTO_SIMULATIONMILLISECOND);

      delay((int)(delay_time-1.0));  // wait for customer to get into queue

      clock_gettime(CLOCK_REALTIME, &time_cust_enterQueue);

      queue_enqueue(g_queue, newCust_transactionTime);  // add new customer to the queue

#if PRINT_DEBUG_MESSAGE
//      printf("Queue thread: Arrival time    : %d\n", newCust_arrivalTime);
      printf("Queue thread: Transaction time: %d\n", newCust_transactionTime);
      printf("Queue thread: Queue wait time : %f\n", delay_time);
#endif
    }
    else  // if (g_bank_open)
    {
      break;  // Out of business hours, bank is closed
    }
  }
  return 0;
}

void* threadFn_teller0(void* arg)
{
  int result;

//  printf("This is threadFn_teller0(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);
    if (g_queue->size > 0)
    {
      result = pthread_mutex_unlock(&mutex);
      teller_runTeller(g_queue, g_teller0, &time_cust_enterQueue);

    }

  }
  return 0;
}

void* threadFn_teller1(void* arg)
{
  int result;

//  printf("This is threadFn_teller1(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);
    if (g_queue->size > 0)
    {
      result = pthread_mutex_unlock(&mutex);
      teller_runTeller(g_queue, g_teller1, &time_cust_enterQueue);

    }

  }
  return 0;
}

void* threadFn_teller2(void* arg)
{
  int result;

//  printf("This is threadFn_teller2(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);
    if (g_queue->size > 0)
    {
      result = pthread_mutex_unlock(&mutex);
      teller_runTeller(g_queue, g_teller2, &time_cust_enterQueue);

    }

  }
  return 0;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  g_queue = queue_createQueue();
  queue_initQueue(g_queue);

  g_teller0 = teller_createTeller();
  g_teller1 = teller_createTeller();
  g_teller2 = teller_createTeller();
  teller_initTeller(g_teller0, TELLER0_ID);
  teller_initTeller(g_teller1, TELLER1_ID);
  teller_initTeller(g_teller2, TELLER2_ID);

  printf("Welcome to the Bank Flow Simulator!\n");

  g_bank_open = true;
  thread_create();

  delay(BUSINESS_HOURS);  // 42 = 0900-1600 business hours
  g_bank_open = false;
  printf("Bank is now closed!\n");
  printf("Here are todays metrics\n");

  metrics_getCustomerTotal();
  metrics_getTellerWaitTimeAvg();
  metrics_getCustQueueWaitTimeAvg();
  metrics_getCustTransactionTimeAvg();
  metrics_getMetrics();

  return EXIT_SUCCESS;
}

