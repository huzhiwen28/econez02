#include <ucos_ii.h>
#include "stm32f10x.h"
#include "bsp_sonic.h"

/*�������*/
struct _SONIC{
      //�������ʱ��
	  volatile u16 timems;

	  //�˲�����
	  volatile u8 filter;

	  //�豸�Ƿ��
	  volatile u8 active;

      //�����õ�������
	  u16 databuff[10];

	  //ָ����һ�����ݲɼ�λ��
	  volatile u8 nextindex;

	  //������ಽ��
	  volatile u8 step;

  	  //���в���
	  volatile u8 runstep;

		//�����ʱ��
		u32 mstime;
		//ʱ���¼
		u32 oldmstime;

};

struct _SONIC SONICDEVICE;
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

static void StartTimer(unsigned int time)
{
	SONICDEVICE.mstime = time;
	SONICDEVICE.oldmstime = OSTimeGet();
}

static bool CheckTimer()
{
	if((OSTimeGet() - SONICDEVICE.oldmstime) > SONICDEVICE.mstime)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*************************************************
����: void Timer6_Configuration(void)
����: TIM6 ����
����: ��
����: ��
��ʱ���㣺(1 /(72 / (72 - 1 + 1)))us
**************************************************/
void Timer6_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	    //��TIM5��ʱ����ʱ��
    
  TIM_DeInit(TIM6);		                                    //TIMx�Ĵ�������Ϊȱʡֵ
  
  TIM_TimeBaseStructure.TIM_Period = 60000;		            //�Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Prescaler=72 - 1;               //TIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  
  //TIM_ARRPreloadConfig(TIM5, ENABLE);                       //�����Զ���װ�ؼĴ�����ARR��
  //TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);	                //����TIM5����ж�
  
  //TIM_Cmd(TIM5, ENABLE);	                                //TIM5����ʱ��
}

void Sonic_Configure(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;  

  /* ��1������GPIO��AFIO������ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);


  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //��ռʽ���ȼ�������Ϊ����ռ���ȼ�


  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;          //ָ���ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //ָ����Ӧ���ȼ���1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //ʹ���ⲿ�ж�ͨ��3
  NVIC_Init(&NVIC_InitStructure);

  /*����PA5Ϊ����*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  /*����PE5Ϊ���*/
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Configure EXTI Line5 to generate an interrupt on falling and rising edge */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;                   //�ⲿ�ж�ͨ��3
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	   //˫�ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;					   //ʹ��
  EXTI_Init(&EXTI_InitStructure);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);  //��PA5���ӵ��ⲿ�ж�ͨ��5
}


/**********************************************************************
* ��    �ƣ�EXTI9_5_IRQHandler()
* ��    �ܣ��ⲿ�ж�ͨ��5�ж�
* ��ڲ����� 
* ���ڲ�����
***********************************************************************/
void EXTI9_5_IRQHandler (void)
{
  OSIntEnter(); 
if(EXTI_GetITStatus(EXTI_Line5) != RESET) 
{
  if(SONICDEVICE.step == 1)
  {
    TIM_SetCounter(TIM6,0);
    TIM_Cmd(TIM6, ENABLE);
	SONICDEVICE.step = 2;
  }
  else if(SONICDEVICE.step == 2)
  {
    SONICDEVICE.databuff[SONICDEVICE.nextindex] = TIM_GetCounter(TIM6);
	if(SONICDEVICE.nextindex == 9)
	{
	  SONICDEVICE.nextindex = 0;
	}
	else
	{
	  SONICDEVICE.nextindex ++;
	}
    TIM_Cmd(TIM6, DISABLE);
	SONICDEVICE.step = 0;
  }
  EXTI_ClearFlag(EXTI_Line5);			       //����жϱ�־�����룩
  EXTI_ClearITPendingBit(EXTI_Line5);
 }
  OSIntExit();
}

/*����������ʼ��
u8 filter :ƽ�����˲����˲��ĵ�λ����1-10��Ĭ��1
u16 timems :���뵥λ�ĳ��������Լ��ʱ�䣬ʱ����ô���80ms������ز����������
*/
void BSP_SonicInit(u8 filter,u16 timems)
{
  int i = 0;
  Sonic_Configure();
  Timer6_Configuration();
  for(i = 0; i < 10; i++)
  {
     SONICDEVICE.databuff[i] = 0;
  }
   
  SONICDEVICE.active = 0;
  SONICDEVICE.nextindex = 0;

  if(filter > 10)
  {
    SONICDEVICE.filter = 10;
  }
  else
  {
    SONICDEVICE.filter = filter;
  }

  SONICDEVICE.timems = timems;

  SONICDEVICE.step = 0;
  SONICDEVICE.runstep = 0;

}

/*����������*/
void BSP_SonicOpen(void)
{
  SONICDEVICE.active = 1;
}

/*���������ر�*/
void BSP_SonicClose(void)
{
  SONICDEVICE.active = 0;
}

/*��������������0.1mm��λ*/
u16 BSP_SonicRead(void)
{
   u32 sum = 0;
   u8 cnt = 0;
   u8 index = 0;

   if(SONICDEVICE.nextindex == 0)
   {
     index = 9;
   }
   else
   {
     index = SONICDEVICE.nextindex -1;
   }
   for(cnt = 0; cnt < SONICDEVICE.filter;cnt ++)
   {
     sum += SONICDEVICE.databuff[index];
	 if(index == 0)
	 {
	   index = 9;
	 }
	 else
	 {  index --;
	 }
   }
   
   return (sum* 17/(SONICDEVICE.filter*10)) ;
}

/*�������������*/
void BSP_SonicRun(void *p_arg)
{
    BSP_SonicInit(1,100);
	BSP_SonicOpen();
    while(1)
	{
		 if(SONICDEVICE.active == 1)
		 {
		   if(SONICDEVICE.runstep == 0)
		   {
//		 	 GPIO_WriteBit(GPIOE, GPIO_Pin_5,(BitAction)1);
		     StartTimer(1);
			 SONICDEVICE.runstep = 1;
			 SONICDEVICE.step = 1;
		   }
		   else if(SONICDEVICE.runstep == 1)
		   {
		     if(CheckTimer() == TRUE)
			 {
//		       GPIO_WriteBit(GPIOE, GPIO_Pin_5,(BitAction)0);
			   SONICDEVICE.runstep = 2;
			   StartTimer(SONICDEVICE.timems);
			 }
		   }
		   else if(SONICDEVICE.runstep == 2)
		   {
		     if(CheckTimer() == TRUE)
			 {
			   SONICDEVICE.runstep = 0;
			 }
		   }
		 }
		OSTimeDlyHMSM(0, 0, 0, 1);
 	}
}

