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

// Defines
#define CMDSIZE_MAX      (2)
#define RECIPE_END       (0x0)
#define MOV              (0x1)
#define WAIT             (0x2)
#define LOOP             (0x4)
#define END_LOOP         (0x5)


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
  CMD_CLEAR
}userCmd_t;

typedef enum servo_states
{
  STILL,
  MOVE,
  PAUSE,
  UNKNOWN
}servoState_t;

typedef enum servo_sm_states
{
  IDLE,
  RUN_RECIPE,
  RUN_USERCMD
}servoSM_t;

typedef struct servo_data
{
  servoPosition_t servoPosition;
  servoState_t    servoState;
  userCmd_t       userCmd;
  bool            runUserCmd;
  uint8_t         recipeOperation;
}servo_t;


// Function prototypes
void recipe_main(void);












#endif /* ndef __recipe_h */


