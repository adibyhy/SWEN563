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

typedef struct queue
{
  int* cust_maxQueue;  // Number of spots available in the queue
  int  cust_available;  // Number of customers in the queue
  int  front;           // First customer in queue
  int  rear;            // Last customer in queue
}queue_t;



#endif /* QUEUE_ */
