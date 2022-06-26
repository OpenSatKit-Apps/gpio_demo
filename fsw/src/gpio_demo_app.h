/*
**  Copyright 2022 bitValence, Inc.
**  All Rights Reserved.
**
**  This program is free software; you can modify and/or redistribute it
**  under the terms of the GNU Affero General Public License
**  as published by the Free Software Foundation; version 3 with
**  attribution addendums as found in the LICENSE.txt
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Affero General Public License for more details.
**
**  Purpose:
**    Define the GPIO Demo application
**
**  Notes:
**    1. This demo is intentionally kept simple so it can be used as a
**       starting point for soemone that wants to write an app for controlling
**       a Raspberry Pi interface using a hardware library like mipea. 
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide.
**    2. cFS Application Developer's Guide.
**
*/

#ifndef _gpio_demo_app_
#define _gpio_demo_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "childmgr.h"
#include "initbl.h"
#include "gpio_ctrl.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define GPIO_DEMO_INIT_APP_EID    (GPIO_DEMO_BASE_EID + 0)
#define GPIO_DEMO_NOOP_EID        (GPIO_DEMO_BASE_EID + 1)
#define GPIO_DEMO_EXIT_EID        (GPIO_DEMO_BASE_EID + 2)
#define GPIO_DEMO_INVALID_MID_EID (GPIO_DEMO_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

/* TODO - Remove once EDS verified
typedef struct 
{

   CFE_MSG_TelemetryHeader_t TlmHeader;
   
   // Framework Status
   
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;


   // Controller  
   
   uint8    CtrlIsMapped;
   uint8    CtrlOutPin;
   uint8    CtrlLedOn;
   uint8    CtrlSpare;
   
   uint32   CtrlOnTime;
   uint32   CtrlOffTime;
   

} GPIO_DEMO_HkPkt;
#define GPIO_DEMO_TLM_HK_LEN sizeof (GPIO_DEMO_HkPkt)
*/

/******************************************************************************
** GPIO_DEMO_Class
*/
typedef struct 
{

   /* 
   ** App Framework
   */ 
   
   INITBL_Class_t     IniTbl; 
   CFE_SB_PipeId_t    CmdPipe;
   CMDMGR_Class_t     CmdMgr;
   CHILDMGR_Class_t   ChildMgr;   
   
   /*
   ** Telemetry Packets
   */
   
   GPIO_DEMO_HkTlm_t  HkTlm;

   /*
   ** App State & Objects
   */ 
       
   uint32             PerfId;
   CFE_SB_MsgId_t     CmdMid;
   CFE_SB_MsgId_t     SendHkMid;
   
   GPIO_CTRL_Class_t  GpioCtrl;
 
} GPIO_DEMO_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern GPIO_DEMO_Class_t  GpioDemo;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: GPIO_DEMO_AppMain
**
*/
void GPIO_DEMO_AppMain(void);


/******************************************************************************
** Function: GPIO_DEMO_NoOpCmd
**
*/
bool GPIO_DEMO_NoOpCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: GPIO_DEMO_ResetAppCmd
**
*/
bool GPIO_DEMO_ResetAppCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _gpio_demo_app_ */
