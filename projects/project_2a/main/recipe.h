#ifndef __recipe_h
#define __recipe_h

/*
********************************************
* SWEN 563:      Real Time and Embedded Systems
* Instructor:    Professor Richard Weil
* Team 11:     - Muhammad Adib Yahaya
*              - Chris Broderick
* Date:          2/20/2018
* Project 02a:   Servo control
* File:          recipe.h
*
*
*********************************************
*/

#include <stdbool.h>

// Defines
#define CMDSIZE_MAX      (2)
#define RECIPE_END       (0x00)
#define MOV              (0x20)
#define WAIT             (0x40)
#define LOOP             (0x60)
#define END_LOOP         (0xA0)

#define PULSEWIDTH_POS_0  (6)
#define PULSEWIDTH_POS_1  (8)
#define PULSEWIDTH_POS_2  (10)
#define PULSEWIDTH_POS_3  (13)
#define PULSEWIDTH_POS_4  (16)
#define PULSEWIDTH_POS_5  (18)

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
  servoPosition_t servoPosition;
  servoState_t    servoState;
  recipeEvent_t   recipeEvent;
  userCmd_t       userCmd;
  bool            runUserCmd;
  uint8_t         recipeOperation;      // recipe index
  uint8_t         recipeLoopIndex;      // recipe loop index
  uint8_t         recipeLoopIteration;  // how many times loop should run
  uint8_t         recipeLoopError;
}servo_t;

// Function prototypes
void recipe_main(void);


#endif /* ndef __recipe_h */

