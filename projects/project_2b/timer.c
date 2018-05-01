/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          4/17/2018
 * Project 6:     Control 2 Servos using QNX
 * File:          timer.c
 *
 *
 *********************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/netmgr.h>    // defines ND_LOCAL_NODE
#include <pthread.h>
#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/mman.h>     /* for mmap_device_io() */
#include <termios.h>
#include "timer.h"

// Definitions
#define BASE_ADDRESS         (0x280)
#define SERVOID_0            (0)
#define SERVOID_1            (1)
#define SERVO_MOVE_DELAY     (200000)

// Variables
static uintptr_t regHandle_portA;  // digital I/O port handle
static uintptr_t regHandle_portB;  // digital I/O port handle

// Function prototypes
static void    timer_set_system_clock_period(void);
static void    timer_setup_dioA(void);
static void    timer_setup_dioB(void);
// static timer_t timer_create_pulse_timer(int *ptr_channel_id);
// void           timer_start_timer(time_t timer_id, int timeOutSec, int timeOutNsec, int periodSec, int periodNsec);
//void           timer_runPWM(int channel_id, int dutyCycle_count, int delay);
void           timer_pwm_setPulseWidth(uint8_t servo, uint8_t pulse_width);
void           timer_runPWM(int channel_id, int dutyCycle_count);

void timer_pwm_setPulseWidth(uint8_t servo, uint8_t pulse_width)
{
  int dutyCycle_count = 0;

  if (servo == SERVOID_0)
  {
    // output to port a, channel_id = 1
    timer_runPWM(1, dutyCycle_count);
  }
  if (servo == SERVOID_1)
  {
    // output to port b, channel_id = 3
    timer_runPWM(3, dutyCycle_count);
  }
}

void timer_runPWM(int channel_id, int dutyCycle_count)
{
  struct _pulse pulse ;
  int counter;

  while (1)
  {
    MsgReceivePulse ( channel_id, &pulse, sizeof( pulse ), NULL );  // block until that timer fires
    if ( counter++ > 200 )  // wait for 20 ms for the period
    {
      counter = 0;

      if (channel_id == 1)
      {
        out8( regHandle_portA, 0xff ) ;    // Sets all the A digital output lines to 1
      }
      else
      {
        out8( regHandle_portB, 0xff ) ;    // Sets all the B digital output lines to 1
      }
    }
    else
    {
      if ( counter > dutyCycle_count )    // set output high for the duty cycle.
      {
        if (channel_id == 1)
        {
          out8( regHandle_portA, 0 ) ;    // Sets all the A digital output lines to 0
        }
        else
        {
          out8( regHandle_portB, 0 ) ;    // Sets all the B digital output lines to 0
        }
      }
    }

  }
}

static void timer_set_system_clock_period(void)
{
  struct _clockperiod clkper;
  // set clock period to 100 microseconds.
  clkper.nsec       = 100000;    // 100K nanoseconds is 100 microseconds.
  clkper.fract      = 0;
  ClockPeriod ( CLOCK_REALTIME, &clkper, NULL, 0  );   // set it now.
}

// Sets ALL digital I/O ports to be output ports and get a handle to Digital I/O Port A
static void timer_setup_dioA(void)
{
  //Digital I/O and D/A Control register at offset 11
  uintptr_t regHandle_iocontrol = mmap_device_io( 1, BASE_ADDRESS +0xb ) ;

  out8( regHandle_iocontrol, 0 ) ;  // sets the the DIR (direction bits) to outputs.

  regHandle_portA = mmap_device_io( 1, BASE_ADDRESS + 0x8 ) ;  // Digital I/O Port A
}

// Sets ALL digital I/O ports to be output ports and get a handle to Digital I/O Port B
static void timer_setup_dioB(void)
{
  //Digital I/O and D/A Control register at offset 11
  uintptr_t regHandle_iocontrol = mmap_device_io( 1, BASE_ADDRESS +0xb ) ;

  out8( regHandle_iocontrol, 0 ) ;  // sets the the DIR (direction bits) to outputs.

  regHandle_portB = mmap_device_io( 1, BASE_ADDRESS + 0x9 ) ;  // Digital I/O Port B
}

// Starts a periodic or one time timer depending on the values we use.
void timer_start_timer( time_t timer_id, int timeOutSec, int timeOutNsec, int periodSec, int periodNsec)
{
  struct itimerspec timer_spec;

  timer_spec.it_value.tv_sec = timeOutSec;
  timer_spec.it_value.tv_nsec = timeOutNsec;
  timer_spec.it_interval.tv_sec = periodSec;
  timer_spec.it_interval.tv_nsec = periodNsec;
  timer_settime( timer_id, 0, &timer_spec, NULL );
}

// This creates a channel that is returned via the ptr_channel_id parameter.
// We use this channel when we wait for a pulse from the timer.
// Note that this returns the channel ID created by this function.
timer_t timer_create_pulse_timer( int *ptr_channel_id )
{
  timer_t         timer_id = 0 ;
  int             pulse_id = 1234;  // make permanent storage to be safe
  struct sigevent event;

  *ptr_channel_id = ChannelCreate( 0 ) ;

  // from QNX example code in Programmers Guide
  // Set up the timer and timer event.
  event.sigev_notify            = SIGEV_PULSE;
  event.sigev_coid              = ConnectAttach ( ND_LOCAL_NODE, 0, *ptr_channel_id, 0, 0 );
  event.sigev_priority          = getprio(0) ;    // use our current priority.
  event.sigev_code              = 1023;      // arbitrary number to identify this pulse source
  event.sigev_value.sival_int   = pulse_id ;  // value to pass with the pulse.

  timer_create(  CLOCK_REALTIME, &event, &timer_id ) ;  // create but do not start the timer.
  return timer_id ;
}

// Loop forever getting a pulse based on what we set for the timer.
// This demonstrates a crude form of PWM output.
// Channel id 1 = servo 0
// Channel id 3 = servo 1
// Channel id might change on different helios
//void timer_runPWM(int channel_id, int dutyCycle_count, int delay)
//{
//  struct _pulse       pulse ;
//  // int                 count             = 0 ;
//  static unsigned int pwm_pulse_counter = 0 ;
//  static unsigned int counter           = 0 ;
//  static int          delay_counter     = 0;
//
//  while (delay_counter++ < delay)
//  {
//    // The idea here is that we set up the timer to fire once every 100 microseconds (the fastest
//    // we can go due to the system clock resolution). We then control the counting of the number
//    // of pulses that have occurred (count the number of cycles on and the number off)
//    MsgReceivePulse ( channel_id, &pulse, sizeof( pulse ), NULL );  // block until that timer fires
//    //printf( "Got pulse number %d and got value %d\n", ++count, pulse.value.sival_int) ;
//    if ( counter++ > 200 )  // wait for 20 ms for the period
//    {
//      counter = 0 ;
//
//      if ( pwm_pulse_counter++ > 200 )  // count the number of pulses since last reset -- reset every 4 seconds
//      {
//        pwm_pulse_counter = 0 ;
//      }
//      if (channel_id == 1)
//      {
//        out8( regHandle_portA, 0xff ) ;    // Sets all the A digital output lines to 1
//      }
//      else
//      {
//        out8( regHandle_portB, 0xff ) ;    // Sets all the B digital output lines to 1
//      }
//    }
//    else
//    {
//      // code to force three positions to make it fun to watch.
//      //      int change_count = 5 ;
//      //      if ( pwm_pulse_counter > 50 && pwm_pulse_counter < 100 )
//      //        change_count = 10 ;
//      //      else if ( pwm_pulse_counter >= 100 )
//      //        change_count = 15 ;
//
//      if ( counter > dutyCycle_count )    // set output high for the duty cycle.
//      {
//        if (channel_id == 1)
//        {
//          out8( regHandle_portA, 0 ) ;    // Sets all the A digital output lines to 0
//        }
//        else
//        {
//          out8( regHandle_portB, 0 ) ;    // Sets all the B digital output lines to 0
//        }
//      }
//    }
//  }
//}

void timer_setup(void)
{
  timer_set_system_clock_period();    // set to 100 microseconds clock tick.
}


int timer_main(int servo_id)
{
  int privity_err;

  /* Give this thread root permissions to access the hardware */
  privity_err = ThreadCtl(_NTO_TCTL_IO, NULL);
  if (privity_err == -1)
  {
    fprintf( stderr, "can't get root permissions\n" );
    return -1;
  }

  if (servo_id == SERVOID_0)
  {
    timer_setup_dioA();
  }
  else
  {
    timer_setup_dioB();
  }

  return 0;
}
