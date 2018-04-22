/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 6:     Voltage Indicator using A/D Converter
 * File:          project_6.c
 *
 *
 *********************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include "AD.h"

int main(int argc, char *argv[])
{
  printf("Welcome to the QNX Momentics IDE\n");

  AD_controlThread();
  AD_mapAllRegisters();
  AD_initAD();
  AD_startAD();

  return EXIT_SUCCESS;
}
