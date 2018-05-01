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
#include <unistd.h>
#include "recipe.h"
#include "timer.h"

// Definitions
#define TIMEVALUE (100000)

#define SERVO_NUMBER      (2)  // Number of servos available
#define SERVO_ONE         (0)
#define SERVO_TWO         (1)

#define PULSEWIDTH_POS_0  (6)
#define PULSEWIDTH_POS_1  (8)
#define PULSEWIDTH_POS_2  (10)
#define PULSEWIDTH_POS_3  (13)
#define PULSEWIDTH_POS_4  (16)
#define PULSEWIDTH_POS_5  (18)

// Function prototypes
void recipe_sm(void);
void recipe_init_servo(uint8_t servo_id);
void recipe_runOperations(uint8_t operation, uint8_t recipe_index);
void recipe_moveServo(uint8_t toPosition, uint8_t fromPosition);

// Global variables
static servo_t   servo;
static servoSM_t current_state = SM_IDLE;



uint8_t recipe0[] =
{
  MOV + 0,
  MOV + 5,
  MOV + 2,
  MOV + 4,
  MOV + 0,
  MOV + 5,
  MOV + 2,
  MOV + 4,
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
  MOV + 3,
  MOV + 5,
  MOV + 1,
  MOV + 3,
  MOV + 4,
  RECIPE_END
};  // for servo 2

uint8_t *recipe[2] = {recipe0, recipe1};


// Start

void recipe_init_servo(uint8_t servo_id)
{
  servo.servoPosition         = SERVO_POS_0;
  servo.servoState            = SS_UNKNOWN;
  servo.runUserCmd            = false;
  servo.userCmd               = CMD_NONE;
  servo.recipeEvent           = RE_MOVE;
  servo.recipeOperation       = 0;
  servo.recipeLoopIndex       = 0;
  servo.recipeLoopIteration   = 0;
  servo.recipeLoopError       = 0;
  servo.servo_id              = servo_id;
}

void recipe_moveServo(uint8_t toPosition, uint8_t fromPosition)
{
  uint8_t pulse_width;
  uint8_t i;
  uint8_t delay_count;
  servo.servoState = SS_MOVE;

  switch (toPosition)
  {
    case SERVO_POS_0:
      pulse_width = PULSEWIDTH_POS_0;
      break;
    case SERVO_POS_1:
      pulse_width = PULSEWIDTH_POS_1;
      break;
    case SERVO_POS_2:
      pulse_width = PULSEWIDTH_POS_2;
      break;
    case SERVO_POS_3:
      pulse_width = PULSEWIDTH_POS_3;
      break;
    case SERVO_POS_4:
      pulse_width = PULSEWIDTH_POS_4;
      break;
    case SERVO_POS_5:
      pulse_width = PULSEWIDTH_POS_5;
      break;
    default:
      break;
  }
  servo.servoPosition = (servoPosition_t)toPosition;
  timer_pwm_setPulseWidth(servo.servo_id, pulse_width);

  delay_count = abs(toPosition - fromPosition);
  for (i = 0; i < delay_count ; i++) // 200 millisecond delay for each movement
  {
    delay(200);
  }
}



/******************************************************************************
function   : void recipe_runOperations(uint8_t whichServo, uint8_t operation, uint8_t recipe_index)
Description: Runs the individual recipe command listed in recipe array.
Parameters :
1. whichServo: Specifies which servo (1 or 2) is operating
2. operation : Specifies which command in recipe should be ran.
               Bit 0:4 is the parameter, bit 5:7 is the opcode
               Example: MOV + 5. Bits (7 downto 0): 0010 0101
               001 is the opcode of MOV, 101 is the servo position (5) to move to.
3. recipe_index:
******************************************************************************/
void recipe_runOperations(uint8_t operation, uint8_t recipe_index)
{
  uint8_t opcode;     //operation top 3 bits
  uint8_t parameter;  //operation bottom 5 bits
  uint8_t fromPosition = 0;
  uint8_t j            = 0;

  opcode    = operation >> 5;
  parameter = operation & 0x1F;

  if(opcode == 0x1)
  {
    fromPosition = servo.servoPosition;
    recipe_moveServo(parameter, fromPosition);
  }
  else if(opcode == 0x2)  // WAIT
  {
    for (j = 1; j <= parameter + 1; j++) // will delay at least once
    {
      delay(100);
    }
  }
  else if(opcode == 0x3)  // LOOP
  {
    servo.recipeLoopIndex     = recipe_index + 1;  // set index to the first command in the loop (i.e set a save point to loop back to)
    servo.recipeLoopIteration = parameter + 1;     // Use the recipe_index from previous function to make this truly work
    servo.recipeOperation     = servo.recipeLoopIndex;
    servo.recipeLoopError++;

    if (servo.recipeLoopError > 1)
    {
      servo.recipeEvent     = RE_ERROR; //error state if hits nested loop
      servo.recipeOperation = 0;
      servo.recipeLoopIndex = 0;
      servo.recipeLoopIteration = 0;
    }
  }
  else if(opcode == 0x5)  // END_LOOP
  {
    if (servo.recipeLoopIteration <= 0)
    {
      servo.recipeLoopError = 0;
    }
    else if(servo.recipeLoopIteration > 0)
    {
      servo.recipeLoopIteration--;
      servo.recipeOperation = servo.recipeLoopIndex;
    }
  }
  else if(opcode == 0x0)  // RECIPE_END
  {
    servo.recipeOperation = 0;
    servo.recipeEvent     = RE_END;
    servo.recipeLoopError = 0;
  }
}


void recipe_sm(void)
{
  uint8_t recipe_index;

  switch (current_state)
  {
    case SM_IDLE:
      current_state = SM_RUN_RECIPE;
      break;

    case SM_RUN_RECIPE:
      if (servo.recipeEvent == RE_MOVE)
      {
        recipe_index = servo.recipeOperation;
        if (recipe_index < sizeof(recipe[servo.servo_id]))
        {
          recipe_runOperations(recipe[servo.servo_id][recipe_index], recipe_index);
          servo.recipeOperation++;
        }
      }
      current_state = SM_IDLE;
      break;

    default:
      break;
  }
}





void recipe_main(int servo_id)
{
  timer_t      timer_id   = (timer_t)0;
  unsigned int time_value = TIMEVALUE;
  int          channel_id = servo_id;

  recipe_init_servo(servo_id);
  timer_main(servo_id);

  timer_id = timer_create_pulse_timer(&channel_id);
  timer_start_timer(timer_id, 0, time_value, 0, time_value);

  printf("Timer set up success on servo %d! Now recipe starts...\n", servo.servo_id);

  while (1)
  {
    recipe_sm();
  }
//  timer_runPWM(servo_id, PULSEWIDTH_POS_0);
//  timer_pwm_setPulseWidth(servo_id, PULSEWIDTH_POS_5);
}









