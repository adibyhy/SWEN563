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
#define REG_ADDRESS_COMMAND              (BASE_ADDRESS + 0x0)  // Read: LSB
#define REG_ADDRESS_PAGE                 (BASE_ADDRESS + 0x1)  // Read: MSB
#define REG_ADDRESS_ADCHANNEL            (BASE_ADDRESS + 0x2)
#define REG_ADDRESS_ADGAINSCAN           (BASE_ADDRESS + 0x3)  // Read: Analog input status
#define REG_ADDRESS_INTERRUPTCOUNTER     (BASE_ADDRESS + 0x4)
#define REG_ADDRESS_PORTA                (BASE_ADDRESS + 0x8)
#define REG_ADDRESS_IOCONTROL            (BASE_ADDRESS + 0xB)
#define REG_ADDRESS_ADMODECONFIG         (BASE_ADDRESS + 0xD)  // IMPORTANT: Set page register to page 2 first

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
uintptr_t regHandle_portA;
uintptr_t regHandle_iocontrol;


// Function prototypes
uintptr_t   AD_accessRegister(uint64_t address);
int         AD_checkADstatus(uint64_t address);
int         AD_setSingleEndedBipolarRange(void);
double      AD_convertMeasurementToVolt(int16_t ADdata);
int         AD_startMeasurement(void);
int16_t     AD_getMeasurement(void);
uint8_t     AD_scaleADvalue(int16_t ADdata);
int         AD_sendADtoExternal(uint8_t AD);

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
  out8(regHandle_page, 0x02);          // write to page 2
  out8(regHandle_ADmodeconfig, 0x00);  // set to single ended bipolar
  out8(regHandle_page, 0x00);          // reset back to page 0

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
  regHandle_portA             = AD_accessRegister(REG_ADDRESS_PORTA);
  regHandle_iocontrol         = AD_accessRegister(REG_ADDRESS_IOCONTROL);

  return 0;
}

int AD_initAD(void)
{
  AD_setSingleEndedBipolarRange();
  out8(regHandle_command, 0x77);  // clear command register without starting a new AD conversion
  out8(regHandle_ADgainscan, 0x01);  // set gain to use 10V range
  out8(regHandle_interruptCounter, 0x00);  // set AINTE bit to off (no interrupt)
  out8(regHandle_iocontrol, 0x00);  // set port A direction to output
  out8(regHandle_ADchannel, 0x00);  // use only one channel in both low and high channels
  // do a wait because it takes some time to complete the channel selection
  AD_checkADstatus(ADWAIT_ADDRESS);  // read status register and wait

  return 0;
}

int AD_checkADstatus(uint64_t address) // returns 0 if ok, -1 if error
{
  int i;
  for (i = 0; i < 20000; i++)
  {
    if (!(in8(regHandle_ADgainscan) & address))
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
  int16_t ADdata;
  double  voltage;
  uint8_t ADscaled;

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
//       printf("Voltage : %f\n", voltage);
//       printf("AD value: %d\n", ADdata);
      ADscaled = AD_scaleADvalue(ADdata);
      printf("AD scaled: %d\n", ADscaled);
      AD_sendADtoExternal(ADscaled);
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

int16_t AD_getMeasurement(void)
{
  int       lsb;
  int       msb;
  int16_t   ADdata;

  lsb = in8(regHandle_command);
  msb = in8(regHandle_page);

  ADdata = ((msb*256) + lsb) & 0xffff;

  return ADdata;
}

double AD_convertMeasurementToVolt(int16_t ADdata)
{
  double voltage;

  voltage = (ADdata/32768.0)*INPUT_VOLTAGE_POSITIVE;

  return voltage;

}

uint8_t AD_scaleADvalue(int16_t ADdata)
{
  uint8_t    ADscaled;
  double     tmp;

  tmp       = ((ADdata + 32767)*65)/65535.0;  // Minimum AD value = 4, maximum AD value = 23
  ADscaled  = (int32_t)tmp & 0x3f;

  return ADscaled;
}

int AD_sendADtoExternal(uint8_t AD)
{
  out8(regHandle_portA, AD);

  return 0;
}

