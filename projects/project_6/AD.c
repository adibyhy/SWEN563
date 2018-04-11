/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          3/8/2018
 * Project 6:     Voltage Indicator using A/D Converter
 * File:          AD.c
 *
 *
 *********************************************
 */

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h> /* for in*() and out*() functions */
#include "AD.h"

// Definitions
#define PORT_LENGTH                      (1)  // 1 Byte

#define BASE_ADDRESS                     (0x280)
#define REG_ADDRESS_COMMAND              (BASE_ADDRESS + 0x0)
#define REG_ADDRESS_PAGE                 (BASE_ADDRESS + 0x1)
#define REG_ADDRESS_ADCHANNEL            (BASE_ADDRESS + 0x2)
#define REG_ADDRESS_ADGAINSCAN           (BASE_ADDRESS + 0x3)
#define REG_ADDRESS_INTERRUPTCOUNTER     (BASE_ADDRESS + 0x4)
#define REG_ADDRESS_ADMODECONFIG         (BASE_ADDRESS + 0xD)  // IMPORTANT: Set page register to page 2 first
#define REG_ADDRESS_LSB                  (BASE_ADDRESS + 0x0)  // LSB and MSB have the same address as COMMAND and PAGE
#define REG_ADDRESS_MSB                  (BASE_ADDRESS + 0x1)

#define ADWAIT_ADDRESS                   (0x20)
#define ADBUSY_ADDRESS                   (0x80)

#define INPUT_VOLTAGE_POSITIVE           (5.0f)
#define INPUT_VOLTAGE_NEGATIVE           (-5.0f)

// Variables
uintptr_t regHandle_command;
uintptr_t regHandle_page;
uintptr_t regHandle_ADchannel;
uintptr_t regHandle_ADgainscan;
uintptr_t regHandle_interruptCounter;
uintptr_t regHandle_ADmodeconfig;


// Function prototypes
uintptr_t   AD_accessRegister(uint64_t address);
int         AD_checkADstatus(uint64_t address);
int         AD_setSingleEndedBipolarRange(void);
int         AD_convertMeasurementToVolt(int ADdata);
int         AD_startMeasurement(void);
double      AD_getMeasurement(void);

// Start

int AD_controlThread(void)
{
  int result = EXIT_SUCCESS;
  if (ThreadCtl (_NTO_TCTL_IO, 0) == -1)
  {
    perror ("ThreadCtl()\n");
    result = EXIT_FAILURE;
  }
  else
  {
    printf("AD_controlThread() success.\n");
  }

  return result;
}

uintptr_t  AD_accessRegister(uint64_t address)
{
  uintptr_t  result;

  result = mmap_device_io(PORT_LENGTH, address);

  if (result == MAP_DEVICE_FAILED)
  {
    printf("Register access error: %d Address: %lld\n", result, address);
  }

  return result;
}

int AD_setSingleEndedBipolarRange(void)
{
  out8(regHandle_page, 0x02);  // write to page 2
  out8(regHandle_ADmodeconfig, 0x00);  // set to single ended bipolar
  out8(regHandle_page, 0x00);  // reset back to page 0

  return 0;
}

int AD_mapAllRegisters(void)
{
  regHandle_command           = AD_accessRegister(REG_ADDRESS_COMMAND);
  regHandle_page              = AD_accessRegister(REG_ADDRESS_PAGE);
  regHandle_ADchannel         = AD_accessRegister(REG_ADDRESS_ADCHANNEL);
  regHandle_ADgainscan        = AD_accessRegister(REG_ADDRESS_ADGAINSCAN);
  regHandle_interruptCounter  = AD_accessRegister(REG_ADDRESS_INTERRUPTCOUNTER);
  regHandle_ADmodeconfig      = AD_accessRegister(REG_ADDRESS_ADMODECONFIG);

  return 0;
}

int AD_initAD(void)
{
  AD_setSingleEndedBipolarRange();
  out8(regHandle_command, 0x77);  // clear command register without starting a new AD conversion
  out8(regHandle_ADgainscan, 0x00);  // set gain to use 10V range
  out8(regHandle_interruptCounter, 0x00);  // set AINTE bit to off (no interrupt)
  out8(regHandle_ADchannel, 0x11);  // use only one channel in both low and high channels
  // do a wait because it takes some time to complete the channel selection
  AD_checkADstatus(ADWAIT_ADDRESS);  // read status register and wait

  return 0;
}

int AD_checkADstatus(uint64_t address) // returns 0 if ok, -1 if error
{
  int i;
  for (i = 0; i < 20000; i++)
  {
    if (!(in8(BASE_ADDRESS+3) & address))
      {
        return (EOK);
      }
  }
  printf("AD status timeout\n");
  return(-1);
}

int AD_startAD(void)
{
  int     result = EOK;
  int     ADdata;
  double  voltage;

  while (1)
  {
    result = AD_startMeasurement();
    if (result == EOK)
    {
      ADdata  = AD_getMeasurement();
      voltage = AD_convertMeasurementToVolt(ADdata);
      if ((voltage > INPUT_VOLTAGE_POSITIVE) || (voltage < INPUT_VOLTAGE_NEGATIVE))
      {
        printf("AD: Error, voltage exceeded constraint!\n");
        break;
      }
      printf("Voltage: %f\n", voltage);
    }
    else
    {
      printf("AD status timeout\n");
    }
  }
  return 0;
}

int AD_startMeasurement(void)
{
  int result;

  out8(regHandle_command, 0x80);  // set bit 7 STRTAD; starts AD conversion; base+3 bit 7 (ADBUSY) will go high
  result = AD_checkADstatus(ADBUSY_ADDRESS);

  return result;
}

double AD_getMeasurement(void)
{
  uint8_t   lsb;
  uint8_t   msb;
  double    ADdata;

  lsb = in8(REG_ADDRESS_LSB);
  msb = in8(REG_ADDRESS_MSB);

  ADdata = (msb*256) + lsb;

  return ADdata;
}

int AD_convertMeasurementToVolt(int ADdata)
{
  int voltage;

  voltage = (ADdata/32768)*INPUT_VOLTAGE_POSITIVE;

  return voltage;

}






