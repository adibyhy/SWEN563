/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          recipe.c
*
*
*********************************************
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "stm32l476xx.h"
#include "recipe.h"
#include "timer2.h"
#include "timer5.h"
#include "LED.h"

#define SERVO_NUMBER      (2)  // Number of servos available
#define SERVO_ONE         (0)
#define SERVO_TWO         (1)

static servo_t   servo[SERVO_NUMBER];
static servoSM_t current_state = SM_IDLE;
uint8_t recipe0[] = 
{ 
  MOV + 0,
  MOV + 5,
  MOV + 2,
  LOOP + 1,  // loop twice
  LOOP + 1,  // loop twice
  MOV + 0,
  MOV + 1,
  MOV + 3,
  END_LOOP,
  WAIT + 31,
  WAIT + 31,
  WAIT + 31,
  MOV + 4,
  MOV + 5,
  MOV + 0,
  MOV + 4,
  WAIT + 1,
  RECIPE_END
};  // for servo 1

uint8_t recipe1[] = 
{ 
  MOV + 3,
  MOV + 5,
  MOV + 1,
  LOOP + 1,  // loop twice
  MOV + 3,
  MOV + 2,
  MOV + 0,
  END_LOOP,
  WAIT + 3,
  MOV + 4,
  MOV + 1,
  MOV + 3,
  MOV + 4,
  WAIT + 3,
  RECIPE_END
};  // for servo 2

uint8_t *recipe[2] = {recipe0, recipe1};

// Function prototypes
void         recipe_sm(void);
void         recipe_init_servo(void);
userCmd_t    recipe_getUserInput(void);
bool         recipe_processUserInput(userCmd_t cmd);
void         recipe_runUserCommand(void);
void         recipe_runOperations(uint8_t whichServo, uint8_t operation, uint8_t recipe_index);
void         recipe_moveServo(uint8_t toPosition, uint8_t whichServo, uint8_t fromPosition);

bool recipe_processUserInput(userCmd_t cmd)
{
  bool             result       = false;
  static uint8_t   whichServo   = 0;
  
  if ((cmd == CMD_ENTER) && (whichServo >= 2))
  {
    result = true;
  }
  
  if ((cmd > CMD_NONE) && cmd < CMD_ENTER)
  {
    if (whichServo >= SERVO_NUMBER)
    {
      whichServo = 0;
    }
    servo[whichServo].userCmd = cmd;
    whichServo++;
  }
  
  if (cmd == CMD_CLEAR)
  {
    whichServo = 0;
    for (whichServo = 0; whichServo < SERVO_NUMBER; whichServo++)
    {
      servo[whichServo].userCmd = CMD_NONE;
      servo[whichServo].recipeOperation = 0;
    }
  }
  if(cmd == CMD_ERROR)
  {
    whichServo = 0;
    for (whichServo = 0; whichServo < SERVO_NUMBER; whichServo++)
    {
      servo[whichServo].userCmd = CMD_ERROR;
    }
  }
  
  return result;
}

userCmd_t recipe_getUserInput(void)
{
  uint32_t   rxByte;
  userCmd_t cmd = CMD_NONE;
  memset(&rxByte, 0, sizeof(rxByte));  // clear array
  rxByte = USART_Read(USART2);
  
  if (rxByte > 0)
  {
    USART_Write(USART2, (uint8_t *)&rxByte, sizeof(rxByte));
  }
  if (rxByte == 'l' || rxByte == 'L')
  {
    cmd = CMD_LEFT;
  }
  else if (rxByte == 'r' || rxByte == 'R')
  {
    cmd = CMD_RIGHT;
  }
  else if (rxByte == 'p' || rxByte == 'P')
  {
    cmd = CMD_PAUSE;
  }
  else if (rxByte == 'x' || rxByte == 'X')
  {
    cmd = CMD_CLEAR;
  }
  else if (rxByte == 'n' || rxByte == 'N')
  {
    cmd = CMD_NONE;
  }
  else if (rxByte == 'b' || rxByte == 'B')
  {
    cmd = CMD_BEGIN;
  }
  else if (rxByte == 'c' || rxByte == 'C')
  {
    cmd = CMD_CONTINUE;
  }
  else if (rxByte == '\r')
  {
    cmd = CMD_ENTER;
    USART_Write(USART2, (uint8_t *)"\r\n>", 3);
  }
  else if (rxByte > 0)
  {
    cmd = CMD_ERROR;
  }
  
  return cmd;
}

void recipe_runUserCommand(void)
{
  uint8_t   i            = 0;
  uint8_t   fromPosition = 0;
  userCmd_t cmd;
  
  for (i = 0; i < SERVO_NUMBER; i++)
  {
    cmd = servo[i].userCmd;
    if (cmd == CMD_NONE)
    {
      servo[i].recipeEvent = RE_NONE;
    }
    else if (cmd == CMD_PAUSE)
    {
      servo[i].recipeEvent = RE_PAUSE;
      Green_LED_Off();
      Red_LED_Off();
    }  
    else if (cmd == CMD_LEFT)
    {
      if (servo[i].servoPosition < SERVO_POS_5 && servo[i].recipeEvent == RE_PAUSE)
      {
        fromPosition = servo[i].servoPosition;
        servo[i].servoPosition += 1;
        recipe_moveServo(servo[i].servoPosition, i, fromPosition);
      }
    }
    else if (cmd == CMD_RIGHT)
    {
      if (servo[i].servoPosition > SERVO_POS_0 && servo[i].recipeEvent == RE_PAUSE)
      {
        fromPosition = servo[i].servoPosition;
        servo[i].servoPosition -= 1;
        recipe_moveServo(servo[i].servoPosition, i, fromPosition);
      }
    }
    else if (cmd == CMD_BEGIN)  // restart recipe
    {
      servo[i].recipeEvent     = RE_MOVE;
      servo[i].recipeOperation = 0;
    }
    else if (cmd == CMD_CONTINUE)
    {
      servo[i].recipeEvent = RE_MOVE;
    }
    else if (cmd == CMD_ERROR)
    {
      Red_LED_On();
      Green_LED_Off();
    }
    servo[i].runUserCmd = false;  // reset flag after running the user command
  }
}

void recipe_moveServo(uint8_t toPosition, uint8_t whichServo, uint8_t fromPosition)
{
  uint8_t pulse_width;
  uint8_t i;
  uint8_t delay;
  servo[whichServo].servoState = SS_MOVE;
  
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
  servo[whichServo].servoPosition = (servoPosition_t)toPosition;
  timer2_pwm_setPulseWidth(whichServo, pulse_width);

  delay = abs(toPosition - fromPosition);
  for (i = 0; i < delay*2 ; i++)  // timer5_delay is 100 millisecond, multiple by 2 to get 200ms for each servo movement
  {
    timer5_delay();
  }
}

void recipe_init_servo(void)
{
  uint8_t i = 0;
  for (i = 0; i < SERVO_NUMBER; i++)
  {
    servo[i].servoPosition         = SERVO_POS_0;
    servo[i].servoState            = SS_UNKNOWN;
    servo[i].runUserCmd            = false;
    servo[i].userCmd               = CMD_NONE;
    servo[i].recipeEvent           = RE_PAUSE;
    servo[i].recipeOperation       = 0;
    servo[i].recipeLoopIndex       = 0;
    servo[i].recipeLoopIteration   = 0;
    servo[i].recipeLoopError       = 0;
    
    
  }
}

void recipe_sm(void)
{
  uint8_t whichServo   = 0;
  uint8_t recipe_index;
  
  switch (current_state)
  {
    case SM_IDLE:
      current_state = SM_RUN_RECIPE;
      break;
    
    case SM_RUN_RECIPE:
      if ((servo[whichServo].runUserCmd == true) || (servo[whichServo+1].runUserCmd == true))
      {
        current_state = SM_RUN_USERCMD;
      }
      else
      { 
        for (whichServo = 0; whichServo < SERVO_NUMBER; whichServo++)  // whichServo is not incrementing
        {
          if (servo[whichServo].recipeEvent == RE_MOVE)
          {
            recipe_index = servo[whichServo].recipeOperation;
            if (recipe_index < sizeof(recipe))
            {
              recipe_runOperations(whichServo, recipe[whichServo][recipe_index], recipe_index); // run one operation at a time
              servo[whichServo].recipeOperation++;
              Green_LED_On();
              Red_LED_Off();
            }
          }
        }

        current_state = SM_IDLE;
      }
      break;
    
    case SM_RUN_USERCMD:
      recipe_runUserCommand();
      current_state = SM_IDLE;
      break;
    
    default:
      break;
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
void recipe_runOperations(uint8_t whichServo, uint8_t operation, uint8_t recipe_index)
{
  uint8_t opcode;     //operation top 3 bits
  uint8_t parameter;  //operation bottom 5 bits
  uint8_t fromPosition = 0;
  uint8_t j            = 0;
  
  opcode    = operation >> 5;
  parameter = operation & 0x1F;
  
  if(opcode == 0x1)
  {
    fromPosition = servo[whichServo].servoPosition;
    recipe_moveServo(parameter, whichServo, fromPosition);
  }
  
  else if(opcode == 0x2)  // WAIT
  {
    for (j = 0; j < parameter + 1; j++) // will delay at least once
    {
      timer5_delay();
    }
  }
  
  else if(opcode == 0x3)  // LOOP
  {
    servo[whichServo].recipeLoopIndex     = recipe_index + 1;  // set index to the first command in the loop (i.e set a save point to loop back to)
    servo[whichServo].recipeLoopIteration = parameter + 1;     // Use the recipe_index from previous function to make this truly work
    servo[whichServo].recipeOperation     = servo[whichServo].recipeLoopIndex;
    servo[whichServo].recipeLoopError++;
    
    if (servo[whichServo].recipeLoopError > 1)
    {
      servo[whichServo].recipeEvent     = RE_ERROR; //error state if hits nested loop
      servo[whichServo].recipeOperation = 0;
      servo[whichServo].recipeLoopIndex = 0;
      servo[whichServo].recipeLoopIteration = 0;
      Red_LED_On();
      Green_LED_On();
    }
  }

  else if(opcode == 0x5)  // END_LOOP
  {
    if (servo[whichServo].recipeLoopIteration <= 0)
    {
      servo[whichServo].recipeLoopError = 0;
    }
    else if(servo[whichServo].recipeLoopIteration > 0)
    {
      servo[whichServo].recipeLoopIteration--;
      servo[whichServo].recipeOperation = servo[whichServo].recipeLoopIndex;
    }
  }
  
  else if(opcode == 0x0)  // RECIPE_END
  {
    servo[whichServo].recipeOperation = 0;
    servo[whichServo].recipeEvent     = RE_END;
    servo[whichServo].recipeLoopError = 0;
  }
}

void recipe_main(void)
{
  userCmd_t cmd;
  bool      runUserInput = false;
  uint8_t   i = 0;
  
  recipe_init_servo();
  
  while (1)
  {
    cmd          = recipe_getUserInput();
    runUserInput = recipe_processUserInput(cmd);
    if (runUserInput)
    {
      for (i = 0; i < 2; i++)
      {
        servo[i].runUserCmd = true;
      }
    }
    timer5_delay();
    recipe_sm();
  }
}

