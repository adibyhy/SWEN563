/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 4:     Bank Flow Simulation
 * File:          metrics.h
 *
 *
 *********************************************
 */

#ifndef METRICS_H_
#define METRICS_H_

#include "queue.h"

// Definitions


// Function prototypes

void metrics_getMetrics(queue_t* queue);
void metrics_getCustomerTotal(int queueCustomer);
void metrics_getCustomerServed(int whichTeller);
void metrics_getCustQueueWaitTime(int timeSpentInQueue);
void metrics_getCustTransactionTime(int transactionTime);
void metrics_getTellerWaitTime(int tellerWaitTime);
void metrics_getQueueMaxDepth(int queueSize);



#endif /* METRICS_H_ */
