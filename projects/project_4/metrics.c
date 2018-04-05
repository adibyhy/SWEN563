/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          metrics.c
 *
 *
 *********************************************
 */
#include <stdlib.h>
#include <stdio.h>
#include "metrics.h"

// Definitions
#define TELLER0_ID                                     (0)
#define TELLER1_ID                                     (1)
#define TELLER2_ID                                     (2)
#define TIME_MILLITOSIMULATIONMINUTE                   (100)

// Variables
static int maxCustWaitTime    = 0;
static int custWaitTimeTotal  = 0;
static double avgCustWaitTime = 0;

static int    maxCustTransactionTime   = 0;
static int    custTransactionTimeTotal = 0;
static double avgCustTransactionTime   = 0;

static int maxQueueDepth = 0;

static int sum_customersServed         = 0;
static int sum_customersServed_teller0 = 0;
static int sum_customersServed_teller1 = 0;
static int sum_customersServed_teller2 = 0;

static double avg_timeTellerWait = 0;
static int    max_timeTellerWait = 0;
static int    sum_timeTellerWait = 0;

// Function prototypes


// Start

void metrics_getMetrics(void)
{
  // print out all metric here
  printf("1. The total number of customers serviced during the day      : %d\n", sum_customersServed);
  printf("2. Customers served by Teller 1, 2, and 3 respectively        : Teller1 = %d Teller2 = %d Teller3 = %d\n", sum_customersServed_teller0, sum_customersServed_teller1, sum_customersServed_teller2);
  printf("3. The average time each customer spends waiting in the queue : %f\n", avgCustWaitTime/TIME_MILLITOSIMULATIONMINUTE);
  printf("4. The average time each customer spends with the teller      : %f\n", avgCustTransactionTime/TIME_MILLITOSIMULATIONMINUTE);
  printf("5. The average time tellers wait for customers                : %f\n", avg_timeTellerWait/TIME_MILLITOSIMULATIONMINUTE);
  printf("6. The maximum customer wait time in the queue                : %f\n", (float)maxCustWaitTime/TIME_MILLITOSIMULATIONMINUTE);
  printf("7. The maximum wait time for tellers waiting for customers    : %f\n", (float)max_timeTellerWait/TIME_MILLITOSIMULATIONMINUTE);
  printf("8. The maximum transaction time for the tellers               : %f\n", (float)maxCustTransactionTime/TIME_MILLITOSIMULATIONMINUTE);
  printf("9. The maximum depth of the customer queue                    : %d\n", maxQueueDepth);

}


void metrics_getCustQueueWaitTime(int timeSpentInQueue)// 3,6
{
  if(timeSpentInQueue > maxCustWaitTime) //checking if time in queue is greater than current max
  {
    maxCustWaitTime = timeSpentInQueue;  // setting new max queue wait time
  }

  custWaitTimeTotal = custWaitTimeTotal + timeSpentInQueue; //getting the total time of all customers waiting in queue
}

void metrics_getCustTransactionTime(int transactionTime)//4,8
{
  if(transactionTime > maxCustTransactionTime) //checking if transaction time is greater than current max
  {
    maxCustTransactionTime = transactionTime;  // setting new max transaction time
  }

  custTransactionTimeTotal = custTransactionTimeTotal + transactionTime; //getting the total time of all customers transaction time
}

void metrics_getQueueMaxDepth(int queueSize)//9
{
  if(queueSize > maxQueueDepth) //checking if queue size is greater than current max
  {
    maxQueueDepth = queueSize;  // setting new max queue size
  }
}

void metrics_getCustomerTotal(void)
{
  sum_customersServed = sum_customersServed_teller0 + sum_customersServed_teller1 + sum_customersServed_teller2;
}

void metrics_getCustomerServed(int whichTeller)
{
  if (whichTeller == TELLER0_ID)
  {
    sum_customersServed_teller0 += 1;
  }
  else if (whichTeller == TELLER1_ID)
  {
    sum_customersServed_teller1 += 1;
  }
  else if (whichTeller == TELLER2_ID)
  {
    sum_customersServed_teller2 += 1;
  }
}

void metrics_getTellerWaitTimeMax(int tellerWaitTime)
{
  if (tellerWaitTime > max_timeTellerWait)
  {
    max_timeTellerWait = tellerWaitTime;
  }
}

void metrics_getTellerWaitTimeSum(int tellerWaitTime)
{
  sum_timeTellerWait += tellerWaitTime;
}

void metrics_getTellerWaitTimeAvg(void)
{
  avg_timeTellerWait = sum_timeTellerWait / sum_customersServed;
}

void metrics_getCustQueueWaitTimeAvg(void)
{
  avgCustWaitTime = custWaitTimeTotal / sum_customersServed;
}
void metrics_getCustTransactionTimeAvg(void)
{
  avgCustTransactionTime = custTransactionTimeTotal / sum_customersServed;
}


