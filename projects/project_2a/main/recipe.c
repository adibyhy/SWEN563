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
#include <string.h>
#include "UART.h"
#include "stm32l476xx.h"
#include "recipe.h"

#define SERVO_NUMBER      (2)  // Number of servos available
#define PULSEWIDTH_POS_0  (4)
#define PULSEWIDTH_POS_1  (7)
#define PULSEWIDTH_POS_2  (10)
#define PULSEWIDTH_POS_3  (13)
#define PULSEWIDTH_POS_4  (16)
#define PULSEWIDTH_POS_5  (19)

static servo_t   servo[SERVO_NUMBER];
static servoSM_t current_state = SM_IDLE;

uint8_t recipe1[] = 
{ 
  MOV + 0,
  MOV + 5,
  MOV + 2,
  RECIPE_END
};

// Function prototypes
void         recipe_sm(void);
void         recipe_init_servo(void);
userCmd_t    recipe_getUserInput(void);
bool         recipe_processUserInput(userCmd_t cmd);
void         recipe_runUserCommand(void);
void         recipe_runOperations(uint8_t whichServo, uint8_t operation);
void         recipe_moveServo(servoPosition_t toPosition, uint8_t whichServo);

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
    if (whichServo < SERVO_NUMBER)
    {
      whichServo++;
    }
    else if (whichServo > SERVO_NUMBER)
    {
      whichServo = 0;
    }
    servo[whichServo].userCmd = cmd;
  }
  
  if (cmd == CMD_CLEAR)
  {
    whichServo = 0;
    for (whichServo = 0; whichServo < SERVO_NUMBER; whichServo++)
    {
      servo[whichServo].userCmd = cmd;
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
  
  return cmd;
}

void recipe_runUserCommand(void)
{
  uint8_t   i   = 0;
  userCmd_t cmd;
  
  for (i = 0; i < SERVO_NUMBER; i++)
  {
    cmd = servo[i].userCmd;
    switch (cmd)
    {
      case CMD_NONE:
        servo[i].recipeEvent = RE_NONE;
        break;
      
      case CMD_PAUSE:
        servo[i].recipeEvent = RE_PAUSE;
        break;
      
      case CMD_LEFT:
        if (servo[i].servoPosition < SERVO_POS_5 && servo[i].recipeEvent == RE_PAUSE)
        {
          servo[i].servoPosition += 1;
          recipe_moveServo(servo[i].servoPosition, i);
        }
        break;
      
      case CMD_RIGHT:
        if (servo[i].servoPosition > SERVO_POS_0 && servo[i].recipeEvent == RE_PAUSE)
        {
          servo[i].servoPosition -= 1;
          recipe_moveServo(servo[i].servoPosition, i);
        }
        break;
      
      case CMD_BEGIN:  // restart recipe
        servo[i].recipeEvent     = RE_MOVE;
        servo[i].recipeOperation = 0;
        break;
      
      case CMD_CONTINUE:
        servo[i].recipeEvent = RE_MOVE;
        break;
      
      default:
        break;
    }
    servo[i].runUserCmd = false;  // reset flag after running the user command
  }
}

void recipe_moveServo(servoPosition_t toPosition, uint8_t whichServo)
{
  uint8_t pulse_width;
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
      pulse_width = PULSEWIDTH_POS_0;
      break;
  }
  
  timer2_pwm_setPulseWidth(whichServo, pulseWidth);
}

void recipe_init_servo(void)
{
  uint8_t i = 0;
  for (i = 0; i < SERVO_NUMBER; i++)
  {
    servo[i].servoPosition   = SERVO_POS_0;
    servo[i].servoState      = SS_UNKNOWN;
    servo[i].runUserCmd      = false;
    servo[i].userCmd         = CMD_NONE;
    servo[i].recipeOperation = 0;
    servo[i].recipeEvent     = RE_PAUSE;
  }
}

void recipe_sm(void)
{
  uint8_t whichServo = 0;
  
  switch (current_state)
  {
    case SM_IDLE:
      current_state = SM_RUN_RECIPE;
      break;
    
    case SM_RUN_RECIPE:
      if (servo[whichServo].runUserCmd == true) 
      {
        current_state = SM_RUN_USERCMD;
      }
      else
      { 
        // run recipe
        // recipe_runOperations(whichServo, operation);  // run one operation at a time
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
function   : recipe_runOperations(uint8_t whichServo, uint8_t operation)
Description: Runs the individual recipe command listed in recipe array.
Parameters : 
1. whichServo: Specifies which servo (1 or 2) is operating
2. operation : Specifies which command in recipe should be ran.
               Bit 0:4 is the parameter, bit 5:7 is the opcode
               Example: MOV + 5. Bits (7 downto 0): 0010 0101
               001 is the opcode of MOV, 101 is the servo position (5) to move to.
******************************************************************************/
void recipe_runOperations(uint8_t whichServo, uint8_t operation)
{
  
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
    recipe_sm();
  }
}

