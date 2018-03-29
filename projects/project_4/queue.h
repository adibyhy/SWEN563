/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          queue.h
 *
 *
 *********************************************
 */

#ifndef QUEUE_
#define QUEUE_

#define QUEUE_MAX             (420)

typedef struct queue
{
  int* cust_maxQueue;   // Number of spots available in the queue
  int  cust_available;  // Number of customers in the queue
  int  front;           // First customer in queue
  int  rear;            // Last customer in queue
}queue_t;

// Function prototypes
queue_t* queue_createQueue(void);
void     queue_initQueue(queue_t* queue);

#endif /* QUEUE_ */
