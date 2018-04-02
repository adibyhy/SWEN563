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

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// Definitions
#define QUEUE_MAX_CAPACITY        (420)
// Variables
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;


// Function prototypes



// Start

queue_t* queue_createQueue(void)
{
  queue_t* queue;

  queue = (queue_t*)malloc(sizeof(queue));      // Allocate memory for this pointer

  return queue;
}

void queue_initQueue(queue_t* queue)
{
  queue->array           = (int*)malloc(sizeof(int)*QUEUE_MAX_CAPACITY);
  queue->capacity        = QUEUE_MAX_CAPACITY;
  queue->size            = 0;
  queue->front           = 0;
  queue->rear            = 0;
}

void queue_enqueue(queue_t* queue, int element)
{
  int result;
  result = pthread_mutex_lock(&queue_mutex);
  if (result == EOK)
  {
    if(queue->size == queue->capacity)
    {
      //do nothing because the line is full
    }
    else
    {
      queue->size++; //adds a person to the line
      queue->rear = queue->rear + 1;

      if (queue->rear == queue->capacity)
      {
        queue->rear = 0;
      }
      queue->array[queue->rear] = element;
    }
  }
  else
  {
    printf ("pthread_mutex_lock(&queue_mutex) failed: %d\n", result);
  }

  pthread_mutex_unlock(&queue_mutex);
}

void queue_dequeue(queue_t* queue)
{
  int result;
  result = pthread_mutex_lock(&queue_mutex);

  if (result == EOK)
  {
    if (queue->size == 0)
    {
      //Check is there anyone in the Queue (if this runs there isn't)
    }
    else
    {
      queue->size--;
      queue->front++;

      if(queue->front == queue->capacity)
      {
        queue->front = 0;
      }
    }
  }
  else
  {
    printf ("pthread_mutex_lock(&queue_mutex) failed: %d\n", result);
  }

  pthread_mutex_unlock(&queue_mutex);
}

int queue_front(queue_t* queue)
{
  if(queue->size == 0)
  {
    return 0; //The queue is empty
  }
  else
  {
    return queue->array[queue->front];
  }
}

int queue_rear(queue_t* queue)
{
  if(queue->size == 0)
  {
    return 0; //The queue is empty
  }
  else
  {
    return queue->array[queue->rear];
  }
}

