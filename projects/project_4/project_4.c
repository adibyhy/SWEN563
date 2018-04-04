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
#include <sys/siginfo.h>
#include <sys/neutrino.h>
#include "queue.h"
#include "teller.h"

// Definitions
#define THREADS_MAX                                    (4)                // Number of threads to be created
#define QUEUE_ARRIVALTIME_MIN                          (60)               // second
#define QUEUE_ARRIVALTIME_MAX                          (240)              // second
#define QUEUE_TRANSACTIONTIME_MIN                      (30)               // second
#define QUEUE_TRANSACTIONTIME_MAX                      (480)              // second
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)
#define TIME_MILLISECONDTONANO                         (1000000L)    // convert time in millisecond to nanosecond
#define BUSINESS_HOURS                                 (42000)  // in millisecond, 42 second = 7 hours 0900-1600
#define CLOCKID                                        CLOCK_REALTIME
#define SIG                                            SIGUSR1
#define CODE_TIMER                                     (1)       // pulse from timer
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
//static bool     g_runTeller = false;
int             chid;  // channel ID (global)

typedef union
{
  // a message can be either from a client, or a pulse
  struct _pulse   pulse;
} MessageT;



// Function prototypes
void    thread_create(void);
void*   threadFn_queue(void* arg);
void*   threadFn_teller0(void* arg);
void*   threadFn_teller1(void* arg);
void*   threadFn_teller2(void* arg);
int     RNG_get(int lower, int upper);
static  void setupPulseAndTimer (void);

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

void setupPulseAndTimer (void)
{
  timer_t             timerid;    // timer ID for timer
  struct sigevent     event;      // event to deliver
  struct itimerspec   timer;      // the timer data structure
  int                 coid;       // connection back to ourselves

  // create a connection back to ourselves
  coid = ConnectAttach (0, 0, chid, 0, 0);
  if (coid == -1)
  {
    fprintf (stderr, "couldn't ConnectAttach to self!\n");
    perror (NULL);
    exit (EXIT_FAILURE);
  }

  // set up the kind of event that we want to deliver -- a pulse
  SIGEV_PULSE_INIT (&event, coid, SIGEV_PULSE_PRIO_INHERIT, CODE_TIMER, 0);

  // create the timer, binding it to the event
  if (timer_create (CLOCK_REALTIME, &event, &timerid) == -1)
  {
    fprintf (stderr, "couldn't create a timer, errno %d\n", errno);
    perror (NULL);
    exit (EXIT_FAILURE);
  }

  // setup the timer
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_nsec = 999999999;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_nsec = 999999999;

  // and start it!
  timer_settime (timerid, 0, &timer, NULL);
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

//  int rcvid;              // process ID of the sender
//  MessageT msg;           // the message itself

  printf("This is threadFn_queue(), thread number is %d\n", (int) arg);

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

        clock_gettime(CLOCK_REALTIME, &time_cust_enterQueue);
        delay((int)(delay_time-1.0));  // wait for customer to get into queue

        queue_enqueue(g_queue, newCust_transactionTime);  // add new customer to the queue
//        g_runTeller = true;

#if PRINT_DEBUG_MESSAGE
//        printf("Queue thread: Arrival time    : %d\n", newCust_arrivalTime);
        printf("Queue thread: Transaction time: %d\n", newCust_transactionTime);
        printf("Queue thread: Queue wait time : %f\n", delay_time);
#endif
        // determine who the message came from
//        rcvid = MsgReceive (chid, &msg, sizeof (msg), NULL);
//        if (rcvid == 0)
//        {
//          //printf("Got at pulse\n");
//          g_runTeller = true;
//        }
//        else  // if (rcvid == 0)
//        {
//          printf("Didn't get pulse\n");;
//        }

        result = pthread_mutex_unlock(&mutex);
        if (result == EOK)
        {
          system(0);
        }
        else
        {
          printf ("pthread_mutex_unlock(&mutex) failed: %d\n", result);
        }

      }
      else  // if (result == EOK)
      {
        printf ("pthread_mutex_lock(&mutex) failed: %d\n", result);
      }
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

  printf("This is threadFn_teller0(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);

    if (result == EOK)
    {
      if (g_queue->size > 0)
      {
        teller_runTeller(g_queue, g_teller0, &time_cust_enterQueue);
//        g_runTeller = false;
      }
      result = pthread_mutex_unlock(&mutex);
      if (result == EOK)
      {
        //system(0);
      }
      else
      {
        printf ("pthread_mutex_unlock(&mutex) failed: %d\n", result);
      }
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
  int result;

  printf("This is threadFn_teller1(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);

    if (result == EOK)
    {
      if (g_queue->size > 0)
      {
        teller_runTeller(g_queue, g_teller1, &time_cust_enterQueue);
//        g_runTeller = false;
      }
      result = pthread_mutex_unlock(&mutex);
      if (result == EOK)
      {
        //system(0);
      }
      else
      {
        printf ("pthread_mutex_unlock(&mutex) failed: %d\n", result);
      }
    }
    else
    {
      printf ("pthread_mutex_lock(&mutex) failed: %d\n", result);
    }
  }
  return 0;
}

void* threadFn_teller2(void* arg)
{
  int result;

  printf("This is threadFn_teller2(), thread number is %d\n", (int) arg);

  while(1)
  {
    result = pthread_mutex_lock(&mutex);

    if (result == EOK)
    {
      if (g_queue->size > 0)
      {
        teller_runTeller(g_queue, g_teller2, &time_cust_enterQueue);
//        g_runTeller = false;
      }
      result = pthread_mutex_unlock(&mutex);
      if (result == EOK)
      {
        //system(0);
      }
      else
      {
        printf ("pthread_mutex_unlock(&mutex) failed: %d\n", result);
      }
    }
    else
    {
      printf ("pthread_mutex_lock(&mutex) failed: %d\n", result);
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

//  if ((chid = ChannelCreate (0)) == -1)
//  {
//    fprintf (stderr, "couldn't create channel!\n");
//    perror (NULL);
//    exit (EXIT_FAILURE);
//  }

//  setupPulseAndTimer();

  printf("Welcome to the Bank Flow Simulator!\n");

  g_bank_open = true;
  thread_create();

  delay(BUSINESS_HOURS);  // 42 = 0900-1600 business hours
  g_bank_open = false;
  printf("Bank is now closed!\n");

  return EXIT_SUCCESS;
}

