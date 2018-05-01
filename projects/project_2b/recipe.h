/*
 ********************************************
 * SWEN 563:      Real Time and Embedded Systems
 * Instructor:    Professor Richard Weil
 * Team 11:     - Muhammad Adib Yahaya
 *              - Chris Broderick
 * Date:          4/17/2018
 * Project 6:     Control 2 Servos using QNX
 * File:          recipe.h
 *
 *
 *********************************************
 */

#ifndef RECIPE_H_
#define RECIPE_H_

// Includes
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"


// Definitions
#define RECIPE_END       (0x00)
#define MOV              (0x20)
#define WAIT             (0x40)
#define LOOP             (0x60)
#define END_LOOP         (0xA0)

// Enum / struct
typedef enum servo_positions
{
  SERVO_POS_0,
  SERVO_POS_1,
  SERVO_POS_2,
  SERVO_POS_3,
  SERVO_POS_4,
  SERVO_POS_5
}servoPosition_t;

typedef enum user_cmds
{
  CMD_NONE,
  CMD_PAUSE,
  CMD_LEFT,
  CMD_RIGHT,
  CMD_BEGIN,
  CMD_CONTINUE,
  CMD_ENTER,
  CMD_CLEAR,
  CMD_ERROR
}userCmd_t;

typedef enum servo_states
{
  SS_STILL,
  SS_MOVE,
  SS_UNKNOWN
}servoState_t;

typedef enum recipe_events
{
  RE_USERCMD,
  RE_MOVE,
  RE_PAUSE,
  RE_NONE,
  RE_ERROR,
  RE_END
}recipeEvent_t;

typedef enum servo_sm_states
{
  SM_IDLE,
  SM_RUN_RECIPE,
  SM_RUN_USERCMD
}servoSM_t;

typedef struct servo_data
{
  servoPosition_t   servoPosition;
  servoState_t      servoState;
  recipeEvent_t     recipeEvent;
  userCmd_t         userCmd;
  bool              runUserCmd;
  uint8_t           recipeOperation;      // recipe index
  uint8_t           recipeLoopIndex;      // recipe loop index
  uint8_t           recipeLoopIteration;  // how many times loop should run
  uint8_t           recipeLoopError;
  uint8_t           servo_id;
}servo_t;







void recipe_main(int servo_id);



#endif /* RECIPE_H_ */
