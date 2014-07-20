#include "app_basicdef.h"

/*
*********************************************************************************************************
*
*                                             应用程序配置文件
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  APP_TASK_START_NAME                          "Start Task"
#define  APP_TASK_232C_NAME                        		"232C"
#define  APP_TASK_USBCOM_NAME                        	"USBCOM"
#define  APP_TASK_USBGATHER_NAME                        "USBGather"
#define  APP_TASK_FLUSHPARA_NAME                        	"FlushPara"
#define  APP_TASK_HEARTBEAT_NAME                        	"HeartBeat"
#define  APP_TASK_REGS_NAME                        		"Regs"
#define  APP_TASK_485COM_NAME                        	"485COM"
#define  APP_TASK_SONIC_NAME                   			"Sonic"
#define  APP_TASK_ENC_NAME                   			"Encoder"
#define  APP_TASK_DAC_NAME                   			"DAC"
#define  APP_TASK_TIM4_NAME                   			"TIM4"
#define  APP_TASK_PID_NAME                   			"PID"

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                               	7
#define  APP_TASK_232C_PRIO                            		13
#define  APP_TASK_USBCOM_PRIO                            	14
#define  APP_TASK_USBGATHER_PRIO                            15
#define  APP_TASK_HEARTBEAT_PRIO                            17
#define  APP_TASK_FLUSHPARA_PRIO                            18
#define  APP_TASK_REGS_PRIO                            		25
#define  APP_TASK_485COM_PRIO                            	20
#define  APP_TASK_SONIC_PRIO                       			22
#define  APP_TASK_ENC_PRIO                       			23
#define  APP_TASK_DAC_PRIO                       			24
#define  APP_TASK_TIM4_PRIO                       			11
#define  APP_TASK_PID_PRIO                       			12

#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         	512
#define  APP_TASK_232C_STK_SIZE                       		100
#define  APP_TASK_USBCOM_STK_SIZE                       	200
#define  APP_TASK_USBGATHER_STK_SIZE                       	300
#define  APP_TASK_HEARTBEAT_STK_SIZE                       	300
#define  APP_TASK_FLUSHPARA_STK_SIZE                       	300
#define  APP_TASK_REGS_STK_SIZE                       		200
#define  APP_TASK_485COM_STK_SIZE                       	300
#define  APP_TASK_SONIC_STK_SIZE                   			100
#define  APP_TASK_ENC_STK_SIZE                   			100
#define  APP_TASK_DAC_STK_SIZE                   			50
#define  APP_TASK_TIM4_STK_SIZE                   			300
#define  APP_TASK_PID_STK_SIZE                   			300

#define MODBUSID State.Basic.ID
#define ControlVersion	10001
//#define TEST232
//#define TEST485
#endif
