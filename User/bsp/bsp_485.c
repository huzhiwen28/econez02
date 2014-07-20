
#include <ucos_ii.h>
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "bsp_fifo.h"
#include "bsp_485.h"
#include "bsp_fifo.h"

/*485�豸*/
struct _485{
      struct _fifo ReadFIFO;
	  struct _fifo WriteFIFO;

	  u8 DMARecvBuff[100];
	  u8 DMASendBuff[100];
	  //DMA�����ѵ�����
	  volatile u16 DMARecvUse;
	  volatile u8 active;

	  /*����״̬��0��û���ڷ��� 1��������*/
	  volatile u8 sendstate;
	 
      OS_EVENT *WriteMutex;//д��
      OS_EVENT *ReadMutex; //����
      OS_EVENT *WriteFinishMutex; //д���֪ͨ

};

struct _485 C485DEVICE;


/*ȡ��fifo�е�ֵ�����س���*/
u16 BSP_485PopoutReadFIFO(struct _fifo* handle,u8* buff,u16 len)
{
  u16 index =0;
  u16 retlen=0;
  if(len > (handle->fifol))
  {
     len = handle->fifol;
  }
  retlen = len;
  for(index = 0;index < retlen; index++)
  {
    if((handle->fifol) > 0)
	{
	  buff[index] = handle->FIFO[handle->op];
      if((handle->op) == 499)
      {
	   handle->op = 0;
	  }
	  else
	  {
	   (handle->op)++;
	  }
      DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,DISABLE);
	  (handle->fifol)--;
      DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	}
  }
  return retlen; 
}

//DMA�ж�
void DMA1_Channel3_IRQHandler(void)
{
  OSIntEnter(); 
  if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)    //
  {
    DMA_InitTypeDef DMA_InitStructure;

	//���ʶ
    DMA_ClearFlag(DMA1_FLAG_TC3);

    /* ����ʹ�� */
    //GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	/* nRE = 0 */
    GPIO_SetBits(GPIOF, GPIO_Pin_11); 	/* nRE = 0 */
    /* ���ͽ�ֹ */
    //GPIO_ResetBits(GPIOF, GPIO_Pin_10);	/* DE = 0 */ 
    GPIO_SetBits(GPIOF, GPIO_Pin_10);	/* DE = 0 */ 

	if(C485DEVICE.DMARecvUse < 100)
	{
	  BSP_PushinFIFO(&(C485DEVICE.ReadFIFO),C485DEVICE.DMARecvBuff + C485DEVICE.DMARecvUse,100-C485DEVICE.DMARecvUse);
	  C485DEVICE.DMARecvUse = 0;

      DMA_Cmd(DMA1_Channel3, DISABLE);
      DMA_DeInit(DMA1_Channel3);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(C485DEVICE.DMARecvBuff);
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  DMA_InitStructure.DMA_BufferSize = 100;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

      DMA_Cmd(DMA1_Channel3, ENABLE);
	}
	else
	{
	  C485DEVICE.DMARecvUse = 0;
      DMA_Cmd(DMA1_Channel3, DISABLE);
      DMA_DeInit(DMA1_Channel3);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(C485DEVICE.DMARecvBuff);
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  DMA_InitStructure.DMA_BufferSize = 100;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

      DMA_Cmd(DMA1_Channel3, ENABLE);
	}     
  }
  OSIntExit();
}

//DMA�ж�
void DMA1_Channel2_IRQHandler(void)
{
  OSIntEnter(); 
  if(DMA_GetITStatus(DMA1_IT_TC2) != RESET)    //
  {
      //GPIO_SetBits(GPIOF, GPIO_Pin_10);		
      //GPIO_SetBits(GPIOF, GPIO_Pin_11); 	
      GPIO_ResetBits(GPIOF, GPIO_Pin_10);		
      GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	

     DMA_ClearFlag(DMA1_FLAG_TC2);

	if(BSP_FIFOLen(&(C485DEVICE.WriteFIFO)) > 0)
	{
      DMA_InitTypeDef DMA_InitStructure;
	  u16 sendlen = 0;

	  if(BSP_FIFOLen(&(C485DEVICE.WriteFIFO)) > 100)
	  {
	    sendlen = BSP_PopoutFIFO(&(C485DEVICE.WriteFIFO),C485DEVICE.DMASendBuff,100);
	  }
	  else
	  {
	    sendlen = BSP_PopoutFIFO(&(C485DEVICE.WriteFIFO),C485DEVICE.DMASendBuff,BSP_FIFOLen(&(C485DEVICE.WriteFIFO)));
	  }

	  //ʹ��USART ����DMA
	  USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);

      DMA_Cmd(DMA1_Channel2, DISABLE);

      DMA_DeInit(DMA1_Channel2);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(C485DEVICE.DMASendBuff);
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  DMA_InitStructure.DMA_BufferSize = sendlen;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
      DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);

      DMA_Cmd(DMA1_Channel2, ENABLE);
	}
	else
	{
     DMA_Cmd(DMA1_Channel2, DISABLE);
 	 C485DEVICE.sendstate = 0;

     USART_ClearFlag(USART3, USART_FLAG_TXE);     // ���־
	 USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
  }
  OSIntExit();
}

/*******************************************************************/
/*                                                                 */
/* STM32�����жϣ�ֻ���ڷ��͵����׶β�ʹ������ж�,��֤���ݷ������ */
/* ˵��������3�ж�                                             */
/*                                                                 */
/*******************************************************************/
void USART3_IRQHandler(void)            //���жϷ�������У�����������Ӧ�ж�ʱ����֪�����ĸ��ж�Դ�����ж�������˱������жϷ�������ж��ж�Դ�����б�Ȼ��ֱ���д�����Ȼ�����ֻ�漰��һ���ж������ǲ����������б�ġ���������ʲô������������б��Ǹ���ϰ��
{
  OSIntEnter(); 

  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)    //���������ݼĴ�����
  {
      USART_ClearFlag(USART3, USART_FLAG_TXE);     // ���־
	  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      USART_ClearFlag(USART3, USART_FLAG_TC);     // ���־
	  USART_ITConfig(USART3, USART_IT_TC, ENABLE);
  }

  if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)    //���ݷ������
  {
      USART_ClearFlag(USART3, USART_FLAG_TC);     // ���־
	  USART_ITConfig(USART3, USART_IT_TC, DISABLE);

     //ע�⣺��ΪRS485�İ�˫����ʽ����RS485����������Ϻ���Ҫ��RS485�����ڽ���״̬�������ղ�������������
      //GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	
      //GPIO_ResetBits(GPIOF, GPIO_Pin_10);	 
      GPIO_SetBits(GPIOF, GPIO_Pin_10);		
      GPIO_SetBits(GPIOF, GPIO_Pin_11); 	
  	 //����֪ͨд���
	 OSSemPost(C485DEVICE.WriteFinishMutex);
}
  OSIntExit();
} 

/*******************************************************************************
	��������USART3_Configuration
	��  ��:
	��  ��:
	����˵����
	��ʼ������Ӳ���豸�������ж�
	���ò��裺
	(1)��GPIO��USART��ʱ��
	(2)����USART3�����ܽ�GPIOģʽ
	(3)����USART3���ݸ�ʽ�������ʵȲ���
	(4)ʹ��USART3�����жϹ���
	(5)���ʹ��USART3����
*/
void USART3_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

   /*�ж�*/
   NVIC_InitTypeDef NVIC_InitStructure;
  
   /* Set the Vector Table base location at 0x08000000 */
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
   /* Configure the NVIC Preemption Priority Bits */  
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				 //�жϷ�������

   /* Enable the USART2 Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       //ͨ������Ϊ����3�ж�
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	   //�ж���Ӧ���ȼ�0
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		   //���ж�
   NVIC_Init(&NVIC_InitStructure); 						   //��ʼ��

   /* Enable the DMA Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;       
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	   //�ж���Ӧ���ȼ�0
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		   //���ж�
   NVIC_Init(&NVIC_InitStructure); 						   //��ʼ��

   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;       
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	   //�ж���Ӧ���ȼ�0
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		   //���ж�
   NVIC_Init(&NVIC_InitStructure); 						   //��ʼ��

	/* ��1������GPIO��USART3������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    /* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* ��2������USART3 Tx��GPIO����Ϊ���츴��ģʽ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ��3������USART3 Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*485��������ʹ�ܺͽ���ʹ�ܽ�*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  /* ��� */
    GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* ��4��������USART3����
	    - BaudRate = 115200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;//57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	/* ��5����ʹ�� USART3�� ������� */
	USART_Cmd(USART3, ENABLE);

    /* �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
    USART_ClearFlag(USART3, USART_FLAG_TC);     // ���־

	//ʹ��USART ����DMA
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);

  /* ����ʹ�� */
  //GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	/* nRE = 0 */
  /* ���ͽ�ֹ */
  //GPIO_ResetBits(GPIOF, GPIO_Pin_10);	/* DE = 0 */ 
      GPIO_SetBits(GPIOF, GPIO_Pin_10);		
      GPIO_SetBits(GPIOF, GPIO_Pin_11); 	
      //GPIO_ResetBits(GPIOF, GPIO_Pin_10);		
      //GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	

   //����DMA��ʼ��
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(C485DEVICE.DMARecvBuff);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 100;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

  DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);

  DMA_Cmd(DMA1_Channel3, ENABLE);

}

/*485���ڳ�ʼ��*/
void BSP_485Init()
{
  u8 err = 0;
  USART3_Configuration(); 
  BSP_InitFIFO(&(C485DEVICE.ReadFIFO));
  BSP_InitFIFO(&(C485DEVICE.WriteFIFO));
  C485DEVICE.active = 0;
  C485DEVICE.sendstate = 0;
  C485DEVICE.DMARecvUse = 0;
  C485DEVICE.WriteMutex = OSMutexCreate(9,&err);//OSSemCreate(1);
  C485DEVICE.ReadMutex = OSMutexCreate(9,&err);//OSSemCreate(1); 
  C485DEVICE.WriteFinishMutex = OSSemCreate(0); 
}

/*485���ڴ�*/
void BSP_485Open()
{
  C485DEVICE.active = 1;
}

/*485���ڹر�*/
void BSP_485Close()
{
  C485DEVICE.active = 0;
}

/*485���ڶ�*/
u16 BSP_485Read(u8* buff,u16 len)
{
   u16 retval = 0;
   INT8U  err;
   OSMutexPend(C485DEVICE.ReadMutex,0,&err);
   retval = BSP_485PopoutReadFIFO(&(C485DEVICE.ReadFIFO),buff,len);
   OSMutexPost(C485DEVICE.ReadMutex);
   return retval;
}

/*485���ڶ��������г���*/
u16 BSP_485InBuffLen()
{
   u16 retval;
   INT8U  err;
   OSMutexPend(C485DEVICE.ReadMutex,0,&err);
   DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,DISABLE);
   //�������������Ѿ����ĵ�����
   if((100-DMA_GetCurrDataCounter(DMA1_Channel3)) > C485DEVICE.DMARecvUse)
   {
      u8 tt =  C485DEVICE.DMARecvUse;
	  C485DEVICE.DMARecvUse = 100-DMA_GetCurrDataCounter(DMA1_Channel3);
	  BSP_PushinFIFO(&(C485DEVICE.ReadFIFO),C485DEVICE.DMARecvBuff + tt,C485DEVICE.DMARecvUse - tt);
   }
   retval = BSP_FIFOLen(&(C485DEVICE.ReadFIFO));
   DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
   OSMutexPost(C485DEVICE.ReadMutex);
   return retval;
}

/*485����д,����0��ʾһ������,����ֵ��ʾδ������е�����ֵ*/
u16 BSP_485Write(u8* buff,u16 len)
{
 
  u16 retval = 0;
    INT8U  err;
	OSMutexPend(C485DEVICE.WriteMutex,0,&err);

  DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,DISABLE);

  retval = BSP_PushinFIFO(&(C485DEVICE.WriteFIFO),buff,len);

  //û�п�ʼ��������������
  if(C485DEVICE.sendstate == 0)
  {
      DMA_InitTypeDef DMA_InitStructure;
	  u16 sendlen = 0;
	  C485DEVICE.sendstate = 1;
 
      //GPIO_SetBits(GPIOF, GPIO_Pin_10);		
      //GPIO_SetBits(GPIOF, GPIO_Pin_11); 	
      //GPIO_SetBits(GPIOF, GPIO_Pin_10);		
      //GPIO_SetBits(GPIOF, GPIO_Pin_11); 	
      GPIO_ResetBits(GPIOF, GPIO_Pin_10);		
      GPIO_ResetBits(GPIOF, GPIO_Pin_11); 	
 
 	  if(BSP_FIFOLen(&(C485DEVICE.WriteFIFO)) > 100)
	  {
	    sendlen = BSP_PopoutFIFO(&(C485DEVICE.WriteFIFO),C485DEVICE.DMASendBuff,100);
	  }
	  else
	  {
	    sendlen = BSP_PopoutFIFO(&(C485DEVICE.WriteFIFO),C485DEVICE.DMASendBuff,BSP_FIFOLen(&(C485DEVICE.WriteFIFO)));
	  }
	  //ʹ��USART ����DMA
	  USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);

      DMA_Cmd(DMA1_Channel2, DISABLE);

      DMA_DeInit(DMA1_Channel2);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(C485DEVICE.DMASendBuff);
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  DMA_InitStructure.DMA_BufferSize = sendlen;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
      DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
      DMA_Cmd(DMA1_Channel2, ENABLE);
  }

   DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);

   //�ȴ�д���
   OSSemPend(C485DEVICE.WriteFinishMutex,0,&err);
   OSMutexPost(C485DEVICE.WriteMutex);
   return retval;
   
}

/*485�����������ڴ�ѭ����һֱ����*/
void BSP_485Run(void)
{
  if(C485DEVICE.active == 1)
  {
  }
}

void BSP_485ClearReadBuff(void)
{
   INT8U  err;
   OSMutexPend(C485DEVICE.ReadMutex,0,&err);
   BSP_ClearFIFO(&(C485DEVICE.ReadFIFO));
   OSMutexPost(C485DEVICE.ReadMutex);
}
