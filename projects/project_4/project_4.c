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
#include <errno.h>
#include <time.h>
#include "queue.h"
#include "teller.h"

// Definitions
#define THREADS_MAX                                    (4)                // Number of threads to be created
#define QUEUE_ARRIVALTIME_MIN                          (60)               // second
#define QUEUE_ARRIVALTIME_MAX                          (240)              // second
#define QUEUE_TRANSACTIONTIME_MIN                      (30)               // second
#define QUEUE_TRANSACTIONTIME_MAX                      (480)              // second
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)

// Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
queue_t*        g_queue;
teller_t*       g_teller0;
bool            g_bank_open = false;

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
  int   result;
  int   newCust_arrivalTime;
  int   newCust_transactionTime;
  float delay_time;
  struct timespec* time_cust_enterQueue;

  printf("This is threadFn_queue(), thread number is %d\n", (int) arg);

  time_cust_enterQueue = (struct timespec*)malloc(sizeof(time_cust_enterQueue));

  while (1)
  {
    if (g_bank_open)
    {
      result = pthread_mutex_lock(&mutex);
      if (result == EOK)
      {
        newCust_arrivalTime     = RNG_get(QUEUE_ARRIVALTIME_MIN, QUEUE_ARRIVALTIME_MAX);
        newCust_transactionTime = RNG_get(QUEUE_TRANSACTIONTIME_MIN, QUEUE_TRANSACTIONTIME_MAX);
        delay_time              = newCust_arrivalTime*(TIME_CONVERTTO_SIMULATIONMILLISECOND);

        clock_gettime(CLOCK_REALTIME, time_cust_enterQueue);
        g_teller0->time_custEnterQueue = time_cust_enterQueue;

        // int timer_settime(timer_t timerid, int flags, struct itimerspec* value, struct itimerspec* ovalue);





        delay((int)delay_time);  // wait for customer to get into queue

        // printf("Arrival time is %d\n", newCust_arrivalTime);
        printf("Simulation delay is %d milliseconds\n", (int)delay_time);
        queue_enqueue(g_queue, newCust_transactionTime);

        result = pthread_mutex_unlock(&mutex);
        system(0);
      }
      else
      {
        printf ("pthread_mutex_lock(&mutex) failed: %d\n", result);
      }
    }
    else
    {
      break;  // Out of business hours, bank is closed
    }
  }
  return 0;
}

void* threadFn_teller0(void* arg)
{
  int result;

  printf("This is threadFn_teller0(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);

    if (result == EOK)
    {
      if (g_queue->size > 0)
      {
        teller_runTeller(g_queue, g_teller0);
      }
      result = pthread_mutex_unlock(&mutex);
      system(0);
    }
    else
    {
      printf ("pthread_mutex_lock(&mutex) failed: %d\n", result);
    }
  }
  return 0;
}

void* threadFn_teller1(void* arg)
{
  printf("This is threadFn_teller1(), thread number is %d\n", (int) arg);

  return 0;
}

void* threadFn_teller2(void* arg)
{
  printf("This is threadFn_teller2(), thread number is %d\n", (int) arg);

  return 0;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  // int timer_create(clockid_t clock_id, struct sigevent *event, timer_t *timerid);




  g_queue = queue_createQueue();
  queue_initQueue(g_queue);

  g_teller0 = teller_createTeller();
  teller_initTeller(g_teller0);

  printf("Welcome to the Bank Flow Simulator!\n");

  g_bank_open = true;
  thread_create();

  sleep(3);  // 42 = 0900-1600 business hours
  g_bank_open = false;

  return EXIT_SUCCESS;
}

