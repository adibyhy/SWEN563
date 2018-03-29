/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          queue.c
 *
 *
 *********************************************
 */

#include <stdlib.h>
#include "queue.h"

queue_t* queue_createQueue(void)
{
  queue_t* queue;
  queue                = (queue_t*)malloc(sizeof(queue));      // Allocate memory for this pointer
}

void queue_initQueue(queue_t* queue)
{
  queue->cust_maxQueue  = (int*)malloc(sizeof(int)*QUEUE_MAX);
  queue->cust_available = 0;
  queue->front          = 0;
  queue->rear           = 0;
}

