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
#include <time.h>

// Definitions
#define THREADS_MAX                                    (4)                // Number of threads to be created
#define QUEUE_ARRIVALTIME_MIN                          (60)               // second
#define QUEUE_ARRIVALTIME_MAX                          (240)              // second
#define TIME_CONVERTTO_SIMULATIONMILLISECOND           (1.6666666666667f)

// Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void thread_create(void);
void* threadFn_queue(void* arg);
void* threadFn_teller0(void* arg);
void* threadFn_teller1(void* arg);
void* threadFn_teller2(void* arg);
int   RNG_get(int lower, int upper);

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
  void*     threadsFn[] = {threadFn_queue, threadFn_teller0, threadFn_teller1, threadFn_teller2, "\0"};
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

//  result = pthread_create(&thread_0, NULL, threadFn_queue, NULL);
//  result = pthread_create(&thread_1, NULL, threadFn_teller0, NULL);
//  result = pthread_create(&thread_2, NULL, threadFn_teller1, NULL);
//  result = pthread_create(&thread_3, NULL, threadFn_teller2, NULL);

}

void* threadFn_queue(void* arg)
{
  int newCust_arrivalTime;
  float x;

  printf("This is threadFn_queue(), thread number is %d\n", (int) arg);
//  printf("This is threadFn_queue()\n");

  newCust_arrivalTime = RNG_get(QUEUE_ARRIVALTIME_MIN, QUEUE_ARRIVALTIME_MAX);
  x = newCust_arrivalTime*(TIME_CONVERTTO_SIMULATIONMILLISECOND);
  printf("Arrival time is %d\n", newCust_arrivalTime);
  printf("Simulation delay is %d milliseconds\n", (int)x);

  delay((int)x);

  return 0;
}

void* threadFn_teller0(void* arg)
{
  printf("This is threadFn_teller0(), thread number is %d\n", (int) arg);
//  printf("This is threadFn_teller0()\n");
  return 0;
}

void* threadFn_teller1(void* arg)
{
  printf("This is threadFn_teller1(), thread number is %d\n", (int) arg);
//  printf("This is threadFn_teller1()\n");
  return 0;
}

void* threadFn_teller2(void* arg)
{
  printf("This is threadFn_teller2(), thread number is %d\n", (int) arg);
//  printf("This is threadFn_teller2()\n");
  return 0;
}


int main(int argc, char *argv[])
{
  srand(time(NULL));
  // queue_create();

  printf("Welcome to the Bank Flow Simulator!\n");

  thread_create();

  sleep(42);  //0900-1600 business hours

  return EXIT_SUCCESS;
}

