/*
**  Copyright 2022 Open STEMware Foundation
**  All Rights Reserved.
**
**  This program is free software; you can modify and/or redistribute it under
**  the terms of the GNU Affero General Public License as published by the Free
**  Software Foundation; version 3 with attribution addendums as found in the
**  LICENSE.txt
**
**  This program is distributed in the hope that it will be useful, but WITHOUT
**  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
**  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
**  details.
**  
**  This program may also be used under the terms of a commercial or enterprise
**  edition license of cFSAT if purchased from the copyright holder.
**
**  Purpose:
**    Define GPIO Controller class
**
**  Notes:
**    TODO - Consider adding a map command if it fails during init. 
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide.
**    2. cFS Application Developer's Guide.
**
*/

#ifndef _gpio_ctrl_
#define _gpio_ctrl_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define GPIO_CTRL_CONSTRUCTOR_EID  (GPIO_CTRL_BASE_EID + 0)
#define GPIO_CTRL_SET_ON_TIME_EID  (GPIO_CTRL_BASE_EID + 1)
#define GPIO_CTRL_SET_OFF_TIME_EID (GPIO_CTRL_BASE_EID + 2)
#define GPIO_CTRL_CHILD_TASK_EID   (GPIO_CTRL_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/

/* TODO - Remove after EDS complete
typedef struct
{

   CFE_MSG_CommandHeader_t CmdHeader;
   
   uint32  OnTime;                      // Time (milliseconds) to keep LED on
   
} GPIO_CTRL_SetOnTimeCmdMsg;
#define GPIO_CTRL_SET_ON_TIME_CMD_DATA_LEN  (sizeof(GPIO_CTRL_SetOnTimeCmdMsg) - sizeof(CFE_MSG_CommandHeader_t))


typedef struct
{

   CFE_MSG_CommandHeader_t CmdHeader;
   
   uint32  OffTime;                      // Time (milliseconds) to keep LED off
   
} GPIO_CTRL_SetOffTimeCmdMsg;
#define GPIO_CTRL_SET_OFF_TIME_CMD_DATA_LEN  (sizeof(GPIO_CTRL_SetOffTimeCmdMsg) - sizeof(CFE_MSG_CommandHeader_t))
*/

/******************************************************************************
** Telmetery Packets
*/


/******************************************************************************
** GPIO_CTRL_Class
*/

typedef struct
{

   /*
   ** Framework References
   */
   
   INITBL_Class_t*  IniTbl;


   /*
   ** Class State Data
   */

   bool    IsMapped;
   bool    LedOn;
   uint8   OutPin;
   uint32  OnTime;    /* Time in Milliseconds */
   uint32  OffTime;   /* Time in Milliseconds */ 
   
   /*
   ** Telemetry Packets
   */
   
   
} GPIO_CTRL_Class_t;



/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: GPIO_CTRL_Constructor
**
** Initialize the GPIO Controller object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void GPIO_CTRL_Constructor(GPIO_CTRL_Class_t *FilePtr, INITBL_Class_t* IniTbl);


/******************************************************************************
** Function: GPIO_CTRL_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void GPIO_CTRL_ResetStatus(void);


/******************************************************************************
** Function: GPIO_CTRL_SetOnTimeCmd
**
*/
bool GPIO_CTRL_SetOnTimeCmd(void* DataObjPtr, const CFE_SB_Buffer_t* SbBufPtr);


/******************************************************************************
** Function: GPIO_CTRL_SetOffTimeCmd
**
*/
bool GPIO_CTRL_SetOffTimeCmd(void* DataObjPtr, const CFE_SB_Buffer_t* SbBufPtr);


/******************************************************************************
** Function: GPIO_CTRL_ChildTask
**
*/
bool GPIO_CTRL_ChildTask(CHILDMGR_Class_t* ChildMgr);


#endif /* _gpio_ctrl_ */
