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
**    Implement the GPIO Controller Class methods
**
**  Notes:
**    TODO - Consider adding a map command if it fails during init. 
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide.
**    2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "initbl.h"
#include "childmgr.h"
#include "gpio_ctrl.h"
#include "gpio.h"

/**********************/
/** Global File Data **/
/**********************/

static GPIO_CTRL_Class_t*  GpioCtrl = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/


/******************************************************************************
** Function: GPIO_CTRL_Constructor
**
** Initialize the GPIO Controller object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void GPIO_CTRL_Constructor(GPIO_CTRL_Class_t *GpioCtrlPtr, INITBL_Class_t* IniTbl)
{
   
   GpioCtrl = GpioCtrlPtr;
   
   memset(GpioCtrl, 0, sizeof(GPIO_CTRL_Class_t));
   
   if (gpio_map() < 0) // map peripherals
   {
   
      CFE_EVS_SendEvent (GPIO_CTRL_CONSTRUCTOR_EID, CFE_EVS_EventType_ERROR, "GPIO map failed");
      GpioCtrl->IsMapped = false;

   }
   else
   {
      GpioCtrl->IsMapped = true;  
   }
   
   GpioCtrl->OutPin  = INITBL_GetIntConfig(IniTbl, CFG_CTRL_OUT_PIN);
   gpio_out(GpioCtrl->OutPin);

   GpioCtrl->OnTime  = INITBL_GetIntConfig(IniTbl, CFG_CTRL_ON_TIME);
   GpioCtrl->OffTime = INITBL_GetIntConfig(IniTbl, CFG_CTRL_OFF_TIME);
   
} /* End GPIO_CTRL_Constructor() */


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
void GPIO_CTRL_ResetStatus(void)
{

   return;

} /* End GPIO_CTRL_ResetStatus() */


/******************************************************************************
** Function: GPIO_CTRL_SetOnTimeCmd
**
** Notes:
**   1. No limits placed on commanded value.
**
*/
bool GPIO_CTRL_SetOnTimeCmd(void* DataObjPtr, const CFE_SB_Buffer_t* SbBufPtr)
{
   
   const GPIO_CTRL_SetOnTimeCmdMsg *Cmd = (const GPIO_CTRL_SetOnTimeCmdMsg *) SbBufPtr;

   bool RetStatus = true;
  
   GpioCtrl->OnTime = Cmd->OnTime;
   CFE_EVS_SendEvent (GPIO_CTRL_SET_ON_TIME_EID, CFE_EVS_EventType_INFORMATION, "GPIO on time set to %u milliseconds", GpioCtrl->OnTime);
  
   return RetStatus;   
   
} /* End GPIO_CTRL_SetOnTimeCmd() */


/******************************************************************************
** Function: GPIO_CTRL_SetOffTimeCmd
**
** Notes:
**   1. No limits placed on commanded value.
**
*/
bool GPIO_CTRL_SetOffTimeCmd(void* DataObjPtr, const CFE_SB_Buffer_t* SbBufPtr)
{
   
   const GPIO_CTRL_SetOffTimeCmdMsg *Cmd = (const GPIO_CTRL_SetOffTimeCmdMsg *) SbBufPtr;

   bool RetStatus = true;
  
   GpioCtrl->OffTime = Cmd->OffTime;  
   CFE_EVS_SendEvent (GPIO_CTRL_SET_OFF_TIME_EID, CFE_EVS_EventType_INFORMATION, "GPIO off time set to %u milliseconds", GpioCtrl->OffTime);
  
   return RetStatus;   
   
} /* End GPIO_CTRL_SetOffTimeCmd() */


/******************************************************************************
** Function: GPIO_CTRL_ChildTask
**
*/
bool GPIO_CTRL_ChildTask(CHILDMGR_Class_t* ChildMgr)
{
   
   if (GpioCtrl->IsMapped)
   {
      
      gpio_set(GpioCtrl->OutPin);
      GpioCtrl->LedOn = true;
      CFE_EVS_SendEvent (GPIO_CTRL_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION, "GPIO pin %d on for %u milliseconds", GpioCtrl->OutPin, GpioCtrl->OnTime);
      OS_TaskDelay(GpioCtrl->OnTime);
    
      gpio_clr(GpioCtrl->OutPin);
      GpioCtrl->LedOn = false;
      CFE_EVS_SendEvent (GPIO_CTRL_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION, "GPIO pin %d off for %u milliseconds", GpioCtrl->OutPin, GpioCtrl->OffTime);
      OS_TaskDelay(GpioCtrl->OffTime);
   
   } /* End if mapped */
   else
   {
     
      OS_TaskDelay(2000);
   
   }
   
   return true;

} /* End GPIO_CTRL_ChildTask() */

