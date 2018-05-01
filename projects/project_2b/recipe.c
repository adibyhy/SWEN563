/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          4/17/2018
 * Project 6:     Control 2 Servos using QNX
 * File:          recipe.c
 *
 *
 *********************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include "recipe.h"
#include "timer.h"

// Definitions
#define TIMEVALUE (100000)

#define SERVO_NUMBER      (2)  // Number of servos available
#define SERVO_ONE         (0)
#define SERVO_TWO         (1)

// Function prototypes
void recipe_sm(void);


// Global variables
static servo_t   servo[SERVO_NUMBER];
static servoSM_t current_state = SM_IDLE;

uint8_t recipe0[] =
{
  MOV + 0,
  MOV + 5,
  MOV + 2,
  MOV + 4,
  RECIPE_END
};  // for servo 1

uint8_t recipe1[] =
{
  MOV + 3,
  MOV + 5,
  MOV + 1,
  MOV + 3,
  MOV + 4,
  RECIPE_END
};  // for servo 2

uint8_t *recipe[2] = {recipe0, recipe1};


// Start

void recipe_sm(void)
{

}





void recipe_main(int servo_id)
{
  timer_t      timer_id   = (timer_t)0;
  unsigned int time_value = TIMEVALUE;

  timer_id = timer_create_pulse_timer(&servo_id);
  timer_start_timer(timer_id, 0, time_value, 0, time_value);

  timer_main(servo_id);
  printf("Timer set up success! Now recipe starts...\n");
  while (1)
  {

  }
}









