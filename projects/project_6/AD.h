/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 6:     Voltage Indicator using A/D Converter
 * File:          AD.H
 *
 *
 *********************************************
 */

#ifndef AD_H_
#define AD_H_

#include <stdint.h>


// Function prototypes
int        AD_controlThread(void);
int        AD_mapAllRegisters(void);
int        AD_initAD(void);
int        AD_startAD(void);

#endif /* AD_H_ */
