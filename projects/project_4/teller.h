/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          teller.h
 *
 *
 *********************************************
 */

#ifndef TELLER_H_
#define TELLER_H_

// Includes
#include <stdlib.h>
#include <time.h>
#include "queue.h"

// Struct
typedef struct teller
{
  int              current_customer;
  int              teller_id;
  struct timespec* time_custEnterQueue;
  struct timespec* time_custMeetTeller;
}teller_t;

// Function prototypes
teller_t* teller_createTeller(void);
void      teller_initTeller(teller_t* teller, int id);
void      teller_runTeller(queue_t* queue, teller_t* teller, struct timespec* time_custEnterQueue);

#endif /* TELLER_H_ */
