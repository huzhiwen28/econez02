

#include <includes.h>	/* 该文件包含了所有必需的.h文件 */
#include <stdio.h>
#include <stdio.h>
#include "bsp_232c.h"
#include "bsp_485.h"
#include "bsp_usb.h"
#include "bsp_eeprom.h"
#include "bsp_dac.h"
#include "bsp_GPIO.h"
#include "bsp_sonic.h"
#include "bsp_enc.h"
#include "bsp_adc.h"
#include "bsp_TimesInt.h"
#include "app_kernelregs.h"
#include "app_usbcom.h"
#include "app_backend.h"
#include "app_usbgather.h"
#include "app_usbmodbusmaster.h"
#include "app_usbmodbusslave.h"
#include "app_485com.h"
#include "app_485modbusslave.h"
#include "app_OutControl.h"
#include "app_inout.h"
#include "app_action.h"
#include "app_pid.h"
#include "app_Port.h"
#include "app_encryption.h"

/*
*********************************************************************************************************
*                                       模块内全局变量
*********************************************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTask232CStk[APP_TASK_232C_STK_SIZE];
static OS_STK AppTaskUSBCOMStk[APP_TASK_USBCOM_STK_SIZE];
static OS_STK AppTaskUSBGatherStk[APP_TASK_USBGATHER_STK_SIZE];
static OS_STK AppTaskFlushParaStk[APP_TASK_FLUSHPARA_STK_SIZE];
static OS_STK AppTaskHeartBeatStk[APP_TASK_HEARTBEAT_STK_SIZE];
static OS_STK AppTaskRegsStk[APP_TASK_REGS_STK_SIZE];
static OS_STK AppTask485COMStk[APP_TASK_485COM_STK_SIZE];
static OS_STK AppTaskSonicStk[APP_TASK_SONIC_STK_SIZE];
static OS_STK AppTaskENCStk[APP_TASK_ENC_STK_SIZE];
static OS_STK AppTaskDACStk[APP_TASK_DAC_STK_SIZE];
static OS_STK AppTaskTIM4Stk[APP_TASK_TIM4_STK_SIZE];
static OS_STK AppTaskPIDStk[APP_TASK_PID_STK_SIZE];


/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTask232C(void *p_arg);
static void PIDRun(void *p_arg);

void printint(int num);

/*
*********************************************************************************************************
*                                      函数定义
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
	CPU_INT08U  err;
	/*
		SystemInit();
			
		配置内部Flash接口，初始化PLL，配置系统频率。系统时钟缺省配置为72MHz，
		在 system_stm32f10x.c 文件中定义系统时钟 SYSCLK_FREQ_72MHz。
		这个函数是ST库中的函数，函数实体在 system_stm32f10x.c 文件 (V3.4.0)
		
		startup_stm32f10x_hd.s 启动文件中已经调用了SystemInit()函数。
	*/
	
	/* 初始化"uC/OS-II"内核 */
	OSInit();

	SystemInit();
   	//BSP_ExtSramInit();
	BSP_232CInit();
	BSP_232COpen();
	BSP_485Init();
	BSP_485Open();
	BSP_USBInit();
	BSP_USBOpen();
	BSP_ADCInit();
	BSP_ADCOpen();
	BSP_EEPROMInit();
	BSP_EEPROMOpen();
	BSP_ENCInit();
  	BSP_ENCOpen();



	App_Backend_Init();
	USB_ModbusMaster_Init();
	Timer5_Configuration(0);   
	Timer4_Configuration();
	GPIO_Configuration();
	App_RegsInit();
	App_Paramload();
	//加密初始化
	App_Encryption_Init();
	//加密校验
	App_Encryption_Verify();
	State.Basic.OK=1;
	
	/* 禁止所有的中断 */
	BSP_IntDisAll();


	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                    (void *)0,		/* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                    (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                       （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
					/*  定义如下：
						OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
						OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
						OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
					*/                  

	/* 指定任务的名称，用于调试。这个函数是可选的 */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);

	/* 启动多任务系统，控制权交给uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
//	uint8_t i, j;

	(void)p_arg;   /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */

	/* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
	BSP_Init();

	/* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率
	注意：此函数会延迟100ms再返回 */
#if (OS_TASK_STAT_EN > 0)
	OSStatInit();
#endif

	/* 创建应用程序的任务 */
	AppTaskCreate();

	/* 任务主体，必须是一个死循环 */
	while (1)     
	{
		//u8 state = OSCPUUsage;
		OSTimeDlyHMSM(0, 0, 0, 200);

		//BSP_USBWrite(&state,1);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	CPU_INT08U      err;


	OSTaskCreateExt(AppTask232C,
                    (void *)0,
                    (OS_STK *)&AppTask232CStk[APP_TASK_232C_STK_SIZE - 1],
                    APP_TASK_232C_PRIO,
                    APP_TASK_232C_PRIO,
                    (OS_STK *)&AppTask232CStk[0],
                    APP_TASK_232C_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_232C_PRIO, APP_TASK_232C_NAME, &err);

	OSTaskCreateExt(App_RgesRun,
                    (void *)0,
                    (OS_STK *)&AppTaskRegsStk[APP_TASK_REGS_STK_SIZE - 1],
                    APP_TASK_REGS_PRIO,
                    APP_TASK_REGS_PRIO,
                    (OS_STK *)&AppTaskRegsStk[0],
                    APP_TASK_REGS_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_REGS_PRIO, APP_TASK_REGS_NAME, &err);

	OSTaskCreateExt(App_HeartbeatTask_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskHeartBeatStk[APP_TASK_HEARTBEAT_STK_SIZE - 1],
                    APP_TASK_HEARTBEAT_PRIO,
                    APP_TASK_HEARTBEAT_PRIO,
                    (OS_STK *)&AppTaskHeartBeatStk[0],
                    APP_TASK_HEARTBEAT_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskNameSet(APP_TASK_HEARTBEAT_PRIO, APP_TASK_HEARTBEAT_NAME, &err);

	OSTaskCreateExt(App_FlushParaTask_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskFlushParaStk[APP_TASK_FLUSHPARA_STK_SIZE - 1],
                    APP_TASK_FLUSHPARA_PRIO,
                    APP_TASK_FLUSHPARA_PRIO,
                    (OS_STK *)&AppTaskFlushParaStk[0],
                    APP_TASK_FLUSHPARA_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskNameSet(APP_TASK_FLUSHPARA_PRIO, APP_TASK_FLUSHPARA_NAME, &err);

	OSTaskCreateExt(USBCOM_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskUSBCOMStk[APP_TASK_USBCOM_STK_SIZE - 1],
                    APP_TASK_USBCOM_PRIO,
                    APP_TASK_USBCOM_PRIO,
                    (OS_STK *)&AppTaskUSBCOMStk[0],
                    APP_TASK_USBCOM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USBCOM_PRIO, APP_TASK_USBCOM_NAME, &err);

	OSTaskCreateExt(USB_Gather_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskUSBGatherStk[APP_TASK_USBGATHER_STK_SIZE - 1],
                    APP_TASK_USBGATHER_PRIO,
                    APP_TASK_USBGATHER_PRIO,
                    (OS_STK *)&AppTaskUSBGatherStk[0],
                    APP_TASK_USBGATHER_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USBGATHER_PRIO, APP_TASK_USBGATHER_NAME, &err);

	OSTaskCreateExt(C485COM_Run,
                    (void *)0,
                    (OS_STK *)&AppTask485COMStk[APP_TASK_485COM_STK_SIZE - 1],
                    APP_TASK_485COM_PRIO,
                    APP_TASK_485COM_PRIO,
                    (OS_STK *)&AppTask485COMStk[0],
                    APP_TASK_485COM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_485COM_PRIO, APP_TASK_USBCOM_NAME, &err);

	OSTaskCreateExt(BSP_SonicRun,
                    (void *)0,
                    (OS_STK *)&AppTaskSonicStk[APP_TASK_SONIC_STK_SIZE - 1],
                    APP_TASK_SONIC_PRIO,
                    APP_TASK_SONIC_PRIO,
                    (OS_STK *)&AppTaskSonicStk[0],
                    APP_TASK_SONIC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_SONIC_PRIO, APP_TASK_SONIC_NAME, &err);


	OSTaskCreateExt(BSP_ENCRun,
                    (void *)0,
                    (OS_STK *)&AppTaskENCStk[APP_TASK_ENC_STK_SIZE - 1],
                    APP_TASK_ENC_PRIO,
                    APP_TASK_ENC_PRIO,
                    (OS_STK *)&AppTaskENCStk[0],
                    APP_TASK_ENC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_ENC_PRIO, APP_TASK_ENC_NAME, &err);

	OSTaskCreateExt(BSP_DACRun,
                    (void *)0,
                    (OS_STK *)&AppTaskDACStk[APP_TASK_DAC_STK_SIZE - 1],
                    APP_TASK_DAC_PRIO,
                    APP_TASK_DAC_PRIO,
                    (OS_STK *)&AppTaskDACStk[0],
                    APP_TASK_DAC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_DAC_PRIO, APP_TASK_DAC_NAME, &err);

	OSTaskCreateExt(TIM4_ProcessRun,
                    (void *)0,
                    (OS_STK *)&AppTaskTIM4Stk[APP_TASK_TIM4_STK_SIZE - 1],
                    APP_TASK_TIM4_PRIO,
                    APP_TASK_TIM4_PRIO,
                    (OS_STK *)&AppTaskTIM4Stk[0],
                    APP_TASK_TIM4_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_TIM4_PRIO, APP_TASK_TIM4_NAME, &err);

	OSTaskCreateExt(PIDRun,
                    (void *)0,
                    (OS_STK *)&AppTaskPIDStk[APP_TASK_PID_STK_SIZE - 1],
                    APP_TASK_PID_PRIO,
                    APP_TASK_PID_PRIO,
                    (OS_STK *)&AppTaskPIDStk[0],
                    APP_TASK_PID_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_PID_PRIO, APP_TASK_PID_NAME, &err);
}



/*
*********************************************************************************************************
*	函 数 名: AppTask232C
*	功能说明: 按键检测任务。这个任务检测USER键和WAKEUP键的状态，并将消息发送到
*				用户界面任务：AppTaskUserIF。
*				这个按键检测带简单的滤波功能，仅供参考。
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTask232C(void *p_arg)
{
	(void)p_arg;		/* 避免编译器告警 */
  	//延迟300ms等待fpga准备就绪，串行epcs需要时间？
 	//OSTimeDlyHMSM(0, 0, 0, 300);
	while (1) 
	{
	    u8 buff[30];
		u16 l = 0;
		int passclk,iii;
		if(BSP_232CInBuffLen()>0)
		{
			l = BSP_232CRead(buff,BSP_232CInBuffLen());
			BSP_232CWrite(buff,l);
		}
 		//if(BSP_485InBuffLen()>0)
		//{
		//	l = BSP_485Read(buff,BSP_485InBuffLen());
		//	BSP_485Write(buff,l);
		//}

		//if(BSP_USBInBuffLen()>0)
		//{
		//	l = BSP_USBRead(buff,BSP_USBInBuffLen());
		//	BSP_USBWrite(buff,l);
		//}
		//printint(BSP_SonicRead());
		//printint(*BSP_ADC1);

		/* 延迟10ms。必须释放CPU，否则低优先级任务会阻塞 */    	
		OSTimeDlyHMSM(0, 0, 0, 1);	 /* 也可以调用 OSTimeDly() 函数实现延迟 */
	}
}


static void PIDRun(void *p_arg)
{
	/* 延迟1000ms,等待 */    	
	OSTimeDlyHMSM(0, 0, 0, 1000);	 /* 也可以调用 OSTimeDly() 函数实现延迟 */
  	while(1)
	{
		//查询校验结果
		if(App_Encryption_Invalid() == 1)
		{
			OSTimeDlyHMSM(0, 0, 0, 1);
			continue;
		}
//	KernelRegs[89]= State.Work.MSpeed;
//	KernelRegs[90]=State.Work.Dia;
//	KernelRegs[88]= State.Work.Tens;		 //1809		 //1438  //2176

	OutControl();		//主控制流程
	 if(State.Basic.OK==1)
	{
 		App_Paramload();	//参数刷新
   		IOInput();     //IO输入口处理
		APortOutput();				//模拟量输出口
   		APortInput();	//模拟理入口处理
	}


	}
}

/*
*********************************************************************************************************
*                                   定义应用程序钩子函数(HOOK)
*
* 说明 : 这些函数被 os_cpu_c.c 中钩子(HOOK)调用。
*
* 返回 : 无
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/* 当一个任务创建完成时，uCOS内核调用该函数。调用时中断是禁止状态 */
void  App_TaskCreateHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* 当一个任务被删除后，uCOS内核调用该函数。调用时中断是禁止状态 */
void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* 这个函数被idle任务(CPU空闲时)调用。用户可以添加代码让CPU进入睡眠模式。调用时中断是使能状态*/
#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/* 该函数被uC/OS-II的统计任务调用，每秒调用一次。用户可以添加额外的统计代码 */
void  App_TaskStatHook (void)
{
}

/* 该函数在任务切换时被调用。调用时中断是禁止状态 */
#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
}
#endif

/* 这个函数被 OS_TCBInit()调用，在初始化最后的任务控制块时调用。中断可能是禁止的也可能是使能的 */
#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/* 这个函数在每次滴答时被调用。中断可能是禁止的也可能是使能的 */
#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
}
#endif
#endif
