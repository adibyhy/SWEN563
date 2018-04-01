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

// Definitions
#define QUEUE_MAX             (420)

// Structs
typedef struct queue
{
  int* array;         // The actual array f values (i.e the customers)
  int  capacity;      // Number of spots available in the queue
  int  size;          // Number of customers in the queue
  int  front;         // First customer in queue
  int  rear;          // Last customer in queue
}queue_t;

// Function prototypes
queue_t* queue_createQueue(void);
void     queue_initQueue(queue_t* queue);

void     queue_enqueue(queue_t* queue, int element);
void     queue_dequeue(queue_t* queue);
int      queue_front(queue_t* queue);
int      queue_rear(queue_t* queue);

#endif /* QUEUE_ */
