

#include <includes.h>	/* ���ļ����������б����.h�ļ� */
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
*                                       ģ����ȫ�ֱ���
*********************************************************************************************************
*/

/* ����ÿ������Ķ�ջ�ռ䣬app_cfg.h�ļ��к궨��ջ��С */
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
*                                      ��������
*********************************************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTask232C(void *p_arg);
static void PIDRun(void *p_arg);

void printint(int num);

/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
	CPU_INT08U  err;
	/*
		SystemInit();
			
		�����ڲ�Flash�ӿڣ���ʼ��PLL������ϵͳƵ�ʡ�ϵͳʱ��ȱʡ����Ϊ72MHz��
		�� system_stm32f10x.c �ļ��ж���ϵͳʱ�� SYSCLK_FREQ_72MHz��
		���������ST���еĺ���������ʵ���� system_stm32f10x.c �ļ� (V3.4.0)
		
		startup_stm32f10x_hd.s �����ļ����Ѿ�������SystemInit()������
	*/
	
	/* ��ʼ��"uC/OS-II"�ں� */
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
	//���ܳ�ʼ��
	App_Encryption_Init();
	//����У��
	App_Encryption_Verify();
	State.Basic.OK=1;
	
	/* ��ֹ���е��ж� */
	BSP_IntDisAll();


	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	OSTaskCreateExt(AppTaskStart,	/* ����������ָ�� */
                    (void *)0,		/* ���ݸ�����Ĳ��� */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* ָ������ջջ����ָ�� */
                    APP_TASK_START_PRIO,	/* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                    APP_TASK_START_PRIO,	/* ����ID��һ����������ȼ���ͬ */
                    (OS_STK *)&AppTaskStartStk[0],/* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                    APP_TASK_START_STK_SIZE, /* ����ջ��С */
                    (void *)0,	/* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                       ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
					/*  �������£�
						OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
						OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
						OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
					*/                  

	/* ָ����������ƣ����ڵ��ԡ���������ǿ�ѡ�� */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);

	/* ����������ϵͳ������Ȩ����uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
//	uint8_t i, j;

	(void)p_arg;   /* �����ڱ���������澯����������������κ�Ŀ����� */

	/* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
	BSP_Init();

	/* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ����
	ע�⣺�˺������ӳ�100ms�ٷ��� */
#if (OS_TASK_STAT_EN > 0)
	OSStatInit();
#endif

	/* ����Ӧ�ó�������� */
	AppTaskCreate();

	/* �������壬������һ����ѭ�� */
	while (1)     
	{
		//u8 state = OSCPUUsage;
		OSTimeDlyHMSM(0, 0, 0, 200);

		//BSP_USBWrite(&state,1);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
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
*	�� �� ��: AppTask232C
*	����˵��: ��������������������USER����WAKEUP����״̬��������Ϣ���͵�
*				�û���������AppTaskUserIF��
*				������������򵥵��˲����ܣ������ο���
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTask232C(void *p_arg)
{
	(void)p_arg;		/* ����������澯 */
  	//�ӳ�300ms�ȴ�fpga׼������������epcs��Ҫʱ�䣿
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

		/* �ӳ�10ms�������ͷ�CPU����������ȼ���������� */    	
		OSTimeDlyHMSM(0, 0, 0, 1);	 /* Ҳ���Ե��� OSTimeDly() ����ʵ���ӳ� */
	}
}


static void PIDRun(void *p_arg)
{
	/* �ӳ�1000ms,�ȴ� */    	
	OSTimeDlyHMSM(0, 0, 0, 1000);	 /* Ҳ���Ե��� OSTimeDly() ����ʵ���ӳ� */
  	while(1)
	{
		//��ѯУ����
		if(App_Encryption_Invalid() == 1)
		{
			OSTimeDlyHMSM(0, 0, 0, 1);
			continue;
		}
//	KernelRegs[89]= State.Work.MSpeed;
//	KernelRegs[90]=State.Work.Dia;
//	KernelRegs[88]= State.Work.Tens;		 //1809		 //1438  //2176

	OutControl();		//����������
	 if(State.Basic.OK==1)
	{
 		App_Paramload();	//����ˢ��
   		IOInput();     //IO����ڴ���
		APortOutput();				//ģ���������
   		APortInput();	//ģ������ڴ���
	}


	}
}

/*
*********************************************************************************************************
*                                   ����Ӧ�ó����Ӻ���(HOOK)
*
* ˵�� : ��Щ������ os_cpu_c.c �й���(HOOK)���á�
*
* ���� : ��
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/* ��һ�����񴴽����ʱ��uCOS�ں˵��øú���������ʱ�ж��ǽ�ֹ״̬ */
void  App_TaskCreateHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* ��һ������ɾ����uCOS�ں˵��øú���������ʱ�ж��ǽ�ֹ״̬ */
void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* ���������idle����(CPU����ʱ)���á��û�������Ӵ�����CPU����˯��ģʽ������ʱ�ж���ʹ��״̬*/
#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/* �ú�����uC/OS-II��ͳ��������ã�ÿ�����һ�Ρ��û�������Ӷ����ͳ�ƴ��� */
void  App_TaskStatHook (void)
{
}

/* �ú����������л�ʱ�����á�����ʱ�ж��ǽ�ֹ״̬ */
#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
}
#endif

/* ��������� OS_TCBInit()���ã��ڳ�ʼ������������ƿ�ʱ���á��жϿ����ǽ�ֹ��Ҳ������ʹ�ܵ� */
#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/* ���������ÿ�εδ�ʱ�����á��жϿ����ǽ�ֹ��Ҳ������ʹ�ܵ� */
#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
}
#endif
#endif
