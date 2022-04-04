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
**    Implement the GPIO Demo application
**
**  Notes:
**    1. See gpio_demo_app.h for details.
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide.
**    2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "gpio_demo_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ    (&(GpioDemo.IniTbl))
#define  CMDMGR_OBJ    (&(GpioDemo.CmdMgr))
#define  CHILDMGR_OBJ  (&(GpioDemo.ChildMgr))
#define  GPIO_CTRL_OBJ (&(GpioDemo.GpioCtrl))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommands(void);
static void SendHousekeepingPkt(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  


/*****************/
/** Global Data **/
/*****************/

GPIO_DEMO_Class_t  GpioDemo;


/******************************************************************************
** Function: GPIO_DEMO_AppMain
**
*/
void GPIO_DEMO_AppMain(void)
{

   uint32 RunStatus = CFE_ES_RunStatus_APP_ERROR;


   CFE_EVS_Register(NULL, 0, CFE_EVS_NO_FILTER);

   if (InitApp() == CFE_SUCCESS) /* Performs initial CFE_ES_PerfLogEntry() call */
   {  
   
      RunStatus = CFE_ES_RunStatus_APP_RUN;
      
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      RunStatus = ProcessCommands(); /* Pends indefinitely & manages CFE_ES_PerfLogEntry() calls */

   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("GPIO_DEMO App terminating, err = 0x%08X\n", RunStatus);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(GPIO_DEMO_EXIT_EID, CFE_EVS_EventType_CRITICAL, "GPIO_DEMO App terminating, err = 0x%08X", RunStatus);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of GPIO_DEMO_AppMain() */


/******************************************************************************
** Function: GPIO_DEMO_NoOpCmd
**
*/

bool GPIO_DEMO_NoOpCmd(void* ObjDataPtr, const CFE_SB_Buffer_t* SbBufPtr)
{

   CFE_EVS_SendEvent (GPIO_DEMO_NOOP_EID, CFE_EVS_EventType_INFORMATION,
                      "No operation command received for GPIO_DEMO App version %d.%d.%d",
                      GPIO_DEMO_MAJOR_VER, GPIO_DEMO_MINOR_VER, GPIO_DEMO_PLATFORM_REV);

   return true;


} /* End GPIO_DEMO_NoOpCmd() */


/******************************************************************************
** Function: GPIO_DEMO_ResetAppCmd
**
** Notes:
**   1. No need to pass an object reference to contained objects becuase they
**      already have a reference from when they were constructed
**
*/

bool GPIO_DEMO_ResetAppCmd(void* ObjDataPtr, const CFE_SB_Buffer_t* SbBufPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   CHILDMGR_ResetStatus(CHILDMGR_OBJ);
   
   GPIO_CTRL_ResetStatus();
	  
   return true;

} /* End GPIO_DEMO_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
void SendHousekeepingPkt(void)
{
   
   GpioDemo.HkPkt.ValidCmdCnt   = GpioDemo.CmdMgr.ValidCmdCnt;
   GpioDemo.HkPkt.InvalidCmdCnt = GpioDemo.CmdMgr.InvalidCmdCnt;

   /*
   ** Controller 
   */ 
   
   GpioDemo.HkPkt.CtrlIsMapped = GpioDemo.GpioCtrl.IsMapped;
   GpioDemo.HkPkt.CtrlOutPin   = GpioDemo.GpioCtrl.OutPin;
   
   GpioDemo.HkPkt.CtrlLedOn    = GpioDemo.GpioCtrl.LedOn;
   GpioDemo.HkPkt.CtrlSpare    = 5;
         
   GpioDemo.HkPkt.CtrlOnTime   = GpioDemo.GpioCtrl.OnTime;
   GpioDemo.HkPkt.CtrlOffTime  = GpioDemo.GpioCtrl.OffTime;
   
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(GpioDemo.HkPkt.TlmHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(GpioDemo.HkPkt.TlmHeader), true);
   
} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = OSK_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit_t ChildTaskInit;
   
   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&GpioDemo.IniTbl, GPIO_DEMO_INI_FILENAME, &IniCfgEnum))
   {
   
      GpioDemo.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_PERF_ID);
      GpioDemo.CmdMid    = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_MID));
      GpioDemo.SendHkMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_SEND_HK_MID));
      
      CFE_ES_PerfLogEntry(GpioDemo.PerfId);

      /* Constructor sends error events */    
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CHILD_PERF_ID);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, 
                                    ChildMgr_TaskMainCallback,
                                    GPIO_CTRL_ChildTask, 
                                    &ChildTaskInit); 
  
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      GPIO_CTRL_Constructor(GPIO_CTRL_OBJ, &GpioDemo.IniTbl);

      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&GpioDemo.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(GpioDemo.CmdMid,    GpioDemo.CmdPipe);
      CFE_SB_Subscribe(GpioDemo.SendHkMid, GpioDemo.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, GPIO_DEMO_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, GPIO_DEMO_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, GPIO_CTRL_SET_ON_TIME_CMD_FC,  GPIO_CTRL_OBJ, GPIO_CTRL_SetOnTimeCmd,  GPIO_CTRL_SET_ON_TIME_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, GPIO_CTRL_SET_OFF_TIME_CMD_FC, GPIO_CTRL_OBJ, GPIO_CTRL_SetOffTimeCmd, GPIO_CTRL_SET_OFF_TIME_CMD_DATA_LEN);
      
      CFE_MSG_Init(CFE_MSG_PTR(GpioDemo.HkPkt.TlmHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_HK_TLM_MID)), GPIO_DEMO_TLM_HK_LEN);
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(GPIO_DEMO_INIT_APP_EID, CFE_EVS_EventType_INFORMATION,
                        "GPIO_DEMO App Initialized. Version %d.%d.%d",
                        GPIO_DEMO_MAJOR_VER, GPIO_DEMO_MINOR_VER, GPIO_DEMO_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static int32 ProcessCommands(void)
{

   int32  RetStatus = CFE_ES_RunStatus_APP_RUN;
   int32  SysStatus;

   CFE_SB_Buffer_t* SbBufPtr;
   CFE_SB_MsgId_t   MsgId = CFE_SB_INVALID_MSG_ID;
   

   CFE_ES_PerfLogExit(GpioDemo.PerfId);
   SysStatus = CFE_SB_ReceiveBuffer(&SbBufPtr, GpioDemo.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(GpioDemo.PerfId);

   if (SysStatus == CFE_SUCCESS)
   {
      
      SysStatus = CFE_MSG_GetMsgId(&SbBufPtr->Msg, &MsgId);
   
      if (SysStatus == CFE_SUCCESS)
      {
  
         if (CFE_SB_MsgId_Equal(MsgId, GpioDemo.CmdMid)) 
         {
            
            CMDMGR_DispatchFunc(CMDMGR_OBJ, SbBufPtr);
         
         } 
         else if (CFE_SB_MsgId_Equal(MsgId, GpioDemo.SendHkMid))
         {

            SendHousekeepingPkt();
            
         }
         else
         {
            
            CFE_EVS_SendEvent(GPIO_DEMO_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                              "Received invalid command packet, MID = 0x%08X",
                              CFE_SB_MsgIdToValue(MsgId));
         } 

      }
      else
      {
         
         CFE_EVS_SendEvent(GPIO_DEMO_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                           "CFE couldn't retrieve message ID from the message, Status = %d", SysStatus);
      }
      
   } /* Valid SB receive */ 
   else 
   {
   
         CFE_ES_WriteToSysLog("GPIO_DEMO software bus error. Status = 0x%08X\n", SysStatus);   /* Use SysLog, events may not be working */
         RetStatus = CFE_ES_RunStatus_APP_ERROR;
   }  
      
   return RetStatus;

} /* End ProcessCommands() */

