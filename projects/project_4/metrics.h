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

void metrics_getMetrics(void);
void metrics_getCustomerTotal(void);
void metrics_getCustomerServed(int whichTeller);
void metrics_getCustQueueWaitTime(int timeSpentInQueue);
void metrics_getCustTransactionTime(int transactionTime);
void metrics_getTellerWaitTimeAvg(void);
void metrics_getTellerWaitTimeMax(int tellerWaitTime);
void metrics_getTellerWaitTimeSum(int tellerWaitTime);
void metrics_getQueueMaxDepth(int queueSize);
void metrics_getCustQueueWaitTimeAvg(void);
void metrics_getCustTransactionTimeAvg(void);



#endif /* METRICS_H_ */
