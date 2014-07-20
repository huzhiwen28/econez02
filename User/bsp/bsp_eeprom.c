#include <ucos_ii.h>
#include "stm32f10x.h"
#include "bsp_eeprom.h"


#define ADDR_24LC08		0xA0    //������ַ
#define I2C_PAGESIZE	4		/* 24C01/01Aҳ������4�� */

/* Private define ------------------------------------------------------------*/
#define SCL_H         GPIO_SetBits(GPIOB , GPIO_Pin_6)
#define SCL_L         GPIO_ResetBits(GPIOB , GPIO_Pin_6)
   
#define SDA_H         GPIO_SetBits(GPIOB , GPIO_Pin_7)
#define SDA_L         GPIO_ResetBits(GPIOB , GPIO_Pin_7)

#define SDA_read      GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7)
#define  uint   u16;

struct _EEPROM EEPROMDEVICE;

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : EEPROM�ܽ�����
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void I2C_Configuration(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

   /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		
}

void SDA_R_Set(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			  //����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	/* PB7 */	
}

void SDA_W_Set(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
   GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*EEPROM��ʼ��*/
void BSP_EEPROMInit(void)
{
  u8 err = 0;
  I2C_Configuration();
  EEPROMDEVICE.active = 0;
  SCL_H ;
  SDA_H; 
  EEPROMDEVICE.IICMutex = OSMutexCreate(9,&err);
}

/*EEPROM��*/
void BSP_EEPROMOpen(void)
{
  EEPROMDEVICE.active = 1;
}

/*EEPROM�ر�*/
void BSP_EEPROMClose(void)
{
  EEPROMDEVICE.active = 0;
}

void I2C_delay(void)
{
	//OSTimeDly(1);	
   	uint8_t i=60;     
   	while(i) 
   	{ 
    	i--; 
   	} 

}


u8 I2C_Start(void)	 //I2C��ʼλ
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)
	  return 0;	                    //SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) 
	  return 0;	                    //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;							//SCLΪ�ߵ�ƽʱ��SDA���½��ر�ʾֹͣλ
	I2C_delay();
	return 1;
}


void I2C_Stop(void)			   //I2Cֹͣλ
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;					   //SCLΪ�ߵ�ƽʱ��SDA�������ر�ʾֹͣλ
	I2C_delay();
}


static void I2C_Ack(void)		//I2C��Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}


static void I2C_NoAck(void)		//I2C����Ӧλ
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}


u8 I2C_WaitAck(void) 	  //I2C�ȴ�Ӧ��λ
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return 0;
	}
	SCL_L;
	return 1;
}


 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ���ݴӸ�λ����λ
* Input          : - SendByte: ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_SendByte(u8 SendByte) 
{
    u8 i;

    for(i = 0;i < 8; i++) 
    {
      SCL_L;
      I2C_delay();
      if(SendByte & 0x80)
        SDA_H;  			  //��SCLΪ�͵�ƽʱ������SDA���ݸı�
      else 
        SDA_L;   
      SendByte <<= 1;
      I2C_delay();
      SCL_H;
      I2C_delay();
    }
    SCL_L;
}


/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : ���ݴӸ�λ����λ
* Input          : None
* Output         : None
* Return         : I2C���߷��ص�����
*******************************************************************************/
u8 I2C_ReceiveByte(void)  
{ 
    u8 i,ReceiveByte = 0;

    SDA_H;				
    for(i = 0;i < 8; i++) 
    {
      ReceiveByte <<= 1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)				   //��SCLΪ�ߵ�ƽʱ��SDA�ϵ����ݱ��ֲ��䣬���Զ�����
      {
        ReceiveByte |= 0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}


/*******************************************************************************
* Function Name  : BSP_EEPROMWriteByte
* Description    : дһ�ֽ�����
* Input          : - SendByte: ��д������
*           	   - WriteAddress: ��д���ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
*******************************************************************************/           
u8 BSP_EEPROMWriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
{		
   INT8U  err;
   OSMutexPend(EEPROMDEVICE.IICMutex,0,&err);

    if(!I2C_Start())
	{
	  OSMutexPost(EEPROMDEVICE.IICMutex);
	  return 0;
	 }
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE); //���ø���ʼ��ַ+������ַ

    if(!I2C_WaitAck())
	{
	  I2C_Stop(); 
	  OSMutexPost(EEPROMDEVICE.IICMutex);
	  return 0;
	}

    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();
	   
    I2C_Stop(); 
 	
	OSTimeDlyHMSM(0, 0, 0, 10);//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)

	OSMutexPost(EEPROMDEVICE.IICMutex);
    return 1;
}									 

/*******************************************************************************
* Function Name  : BSP_EEPROMReadByte
* Description    : ��ȡһ������
* Input          : - pBuffer: ��Ŷ�������
*           	   - length: ����������
*                  - ReadAddress: ��������ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�����,=0ʧ��
*******************************************************************************/          
u8 BSP_EEPROMReadByte(u8* pBuffer,   u16 length,   u16 ReadAddress,  u8 DeviceAddress)
{		
   INT8U  err;
   OSMutexPend(EEPROMDEVICE.IICMutex,0,&err);
    if(!I2C_Start())
	{
	  OSMutexPost(EEPROMDEVICE.IICMutex);
	  return 0;
	}

    I2C_SendByte(((ReadAddress & 0x0700) >> 7) | DeviceAddress & 0xFFFE); //���ø���ʼ��ַ+������ַ

    if(!I2C_WaitAck())
	{
	  I2C_Stop(); 
	  OSMutexPost(EEPROMDEVICE.IICMutex);
	  return 0;
	}

    I2C_SendByte((u8)(ReadAddress & 0x00FF));   //���õ���ʼ��ַ   
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress | 0x0001);
    I2C_WaitAck();

    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)
	    I2C_NoAck();
      else 
	    I2C_Ack(); 
      pBuffer++;
      length--;
    }

    I2C_Stop();

	OSMutexPost(EEPROMDEVICE.IICMutex);
    return 1;
}


/**********************************************************************************8  
����
time:2009,04,13
��EEPROM�ж�д����
*************************************************************************************/

//extern uint ReadDataEEPROM(uint Adss);
//extern char SendWordEEPROM(uint Adss,uint SData);

//extern bit SendDataEEPROM(uint Adss,uint Data)
/***************************************************************************************8
��ʱ������IIC����
**************************************************************************************/
void IICDelay()
{
	I2C_delay();
}

void IICStart()								/*����I2C���ߵĺ�������SCLΪ�ߵ�ƽʱʹSDA����һ��������*/ 
{
//	IICWP=0;
    IICDelay();
    IICDelay();
//	AD1CON1bits.ADON = 0;
//	IEC0bits.AD1IE=0;
	SDA_W_Set();		    //IO����Ϊд
	SDA_H;
    SCL_H;
	IICDelay();
  	IICDelay();									//
	SDA_L;									//
  	IICDelay();
    IICDelay();
    SCL_L;
  	IICDelay();
    IICDelay();

}

void IICStop(void)		 				/*��ֹI2C���ߣ���SCLΪ�ߵ�ƽʱʹSDA����һ��������*/
{
	SDA_W_Set();		    //IO����Ϊд
     SCL_H;
	 IICDelay();
	 SDA_L;	
	 IICDelay();
     SCL_H;
     IICDelay();
	 SDA_H;	
     IICDelay();
//   SCL=0;								//����ʱȫ������
//		AD1CON1bits.ADON = 1;
     IICDelay();
	  IICDelay();
	 IICDelay();

//		IICWP=1;

     
} 

char Check_Acknowledge(void)	        /*������һ���ֽں�����豸��Ӧ���ź�*/
{
	u16 Ack;
   // SDA=1;
	SDA_R_Set();		    //IO����Ϊ��
    SCL_L;
	IICDelay();
    SCL_H;
   	IICDelay();
	Ack=SDA_read;
	IICDelay();
    SCL_L;
    IICDelay();
	SDA_W_Set();		    //IO����Ϊд
	SDA_H;
    if(Ack==1)
	{
          return(0);				//�޼�⵽Ӧ��
	}
	else
	{
        return(1);					//��⵽Ӧ��
    } 
}


/***********************************************************************************************
��24C04����һ���ֽ�
���������SData	��д���� 8bit	  
�������������ֵ   0:����ʧ��   1:�ɹ�
***********************************************************************************************/
u16 IICSend(u16 IICData) 			/*��I2C����дһ���ֽ�*/
{
       
     u16 i;
	u16 b;
//	TRISGbits.TRISG3=0;
     for(i=0;i<8;i++)
	{
   		 SCL_L;
    	IICDelay();
		b=(IICData<<i)&0x80;
    	 if(b==0x80)
		{
   		  	SDA_H;
		}
         else
		{
          	SDA_L;    
		}
		IICDelay();
		SCL_H;
    	IICDelay();
		IICDelay();
	}
	SCL_L;
//	SDA_H;
//	IICDelay();
	i=Check_Acknowledge();
	 return(i);				//0:����ʧ��   1:�ɹ�
}

/***********************************************************************************************
��24C04����һ����
���������SData	��д���� 8bit	  
�������������ֵ   0:����ʧ��   1:�ɹ�
***********************************************************************************************/
u16 IICSendWord(u16 IICData) 			/*��I2C����дһ���ֽ�*/
{
       
     u16 i;
	u16 b;
//	TRISGbits.TRISG3=0;
     for(i=0;i<16;i++)
	{
		SCL_L;
    	IICDelay();
		b=(IICData<<i)&0x8000;
    	 if(b==0x8000)
		{
   		  	SDA_H;
		}
         else
		{
          	SDA_L;    
		}
		IICDelay();
		SCL_H;
    	IICDelay();
		IICDelay();
	}
	SCL_L;
  	SDA_H;
	IICDelay();
	i=Check_Acknowledge();
	 return(i);				//0:����ʧ��   1:�ɹ�
}

void Ack()				//������ʱ��Ӧ��
{
   	SDA_L; 
   	IICDelay();
	SCL_H;
	IICDelay();
	IICDelay();
	SCL_L;
    IICDelay();			
}

void NoAck()			//������ʱ��Ӧ��
{
   	SDA_H; 
   	IICDelay();
	SCL_H;
	IICDelay();
	IICDelay();
	SCL_L;
   	SDA_L; 
    IICDelay();
}



u16 IICRead() 			/*��I2C���߶�һ���ֽ�*/
{
       
     u16 i;
	u16 Data=0;
	u16 SDAIO;
	Data=0;
	SDA_R_Set();		    //IO����Ϊ��
     for(i=0;i<8;i++)				//��λ������
	{
		SCL_L;
    	IICDelay();
		SCL_H;
    	IICDelay();
		SDAIO=SDA_read;
    	if(SDAIO==1)
		{
   		  	Data=Data+(0x80>>i);
		}
		IICDelay();
		SCL_L;
		IICDelay();
	}
	SDA_W_Set();		    //IO����Ϊ��
	return(Data);
}


/***********************************************************************************************
��24C04дһ���ֽ�
���������Adss,��д��ַ
		  SData	��д���� 8bit
����������� 
***********************************************************************************************/
u16 SendDataEEPROM(u16 Adss,u16 SData)
{
   INT8U  err;
   OSMutexPend(EEPROMDEVICE.IICMutex,0,&err);
	IICStart();

	if(IICSend(0xa0))			// 24C04��д����
	{
		if(IICSend(Adss))		//д��ַ
			{
				if(IICSend(SData))			//д����
				{
					IICStop();
				  OSMutexPost(EEPROMDEVICE.IICMutex);
					return(1);				//�ɹ�
				}
				else
				{
				  OSMutexPost(EEPROMDEVICE.IICMutex);
					IICStop();
					return(0);

				}
			}
			else
			{
				IICStop();
			  OSMutexPost(EEPROMDEVICE.IICMutex);
				return(0);
			}	
		}
	else
	{
		IICStop();
	  OSMutexPost(EEPROMDEVICE.IICMutex);
		return(0);
	}
  OSMutexPost(EEPROMDEVICE.IICMutex);
}


/***********************************************************************************************
��24C04дһ����
���������Adss,��д��ַ
		  SData	��д���� 16bit ��λ��ǰ
����������� 
***********************************************************************************************/
u8 SendWordEEPROM(u16 Adss,u16 SData)
{	

 	u16 DataH;
	u16 DataL;
	DataH=SData>>8;
  	DataL=SData&0x00ff;
	while(!BSP_EEPROMWriteByte(DataH,Adss, 0xA0))
	OSTimeDlyHMSM(0, 0, 0, 1);

	while(!BSP_EEPROMWriteByte(DataL, Adss+1, 0xA0))
	OSTimeDlyHMSM(0, 0, 0, 1);
	return(1);
}


/***********************************************************************************************
��һ24C04��һ����
���������Adss,������ַ
����������������ݷ���ֵ 
***********************************************************************************************/
u16 ReadWordEEPROM(u16 Adss)
{
//	StopINT();
   INT8U  err;
	u16 ReadData;

	u16 ADDH;
	u16 ADDH2;
	u16 ADDL;
   OSMutexPend(EEPROMDEVICE.IICMutex,0,&err);
	if(Adss>0xff)
	{
		ADDH=0xa2;
		ADDH2=0xa3;
		ADDL=Adss&0x00ff;
	}
	else
	{
		ADDH=0xa0;
		ADDH2=0xa1;
		ADDL=Adss&0x00ff;
	}

	IICStart();

	if(IICSend(ADDH))					
	{
//		Ack();
		if(IICSend(ADDL))									//д��ַ
		{
//			Ack();
			IICStart();
			if(IICSend(ADDH2))
			{
//				Ack();
				ReadData=IICRead();							//����λ
				Ack();										//Ӧ��
				ReadData=ReadData<<8;									
				ReadData=IICRead()+ReadData;				//����λ
				NoAck();									//��Ӧ��
				IICStop();
			}
			else
			{
				ReadData=0xffff;
				IICStop();
			}
		}
		else
		{
			ReadData=0xffff;
			IICStop();
		}	
	}
	else
	{
		ReadData=0xffff;
		IICStop();
	}
	SDA_H;
	return(ReadData);

//	StartINT();
	  OSMutexPost(EEPROMDEVICE.IICMutex);
}


/***********************************************************************************************
��;:��һ24C04��һ���ֽ�
���������Adss,������ַ
����������������ݷ���ֵ 
***********************************************************************************************/
u16 ReadDataEEPROM(u16 Adss)
{
	u16 ReadData;

	u16 ADDH;
	u16 ADDH2;
	u16 ADDL;
   INT8U  err;
   OSMutexPend(EEPROMDEVICE.IICMutex,0,&err);
	if(Adss>0xff)
	{
		ADDH=0xa2;
		ADDH2=0xa3;
		ADDL=Adss&0x00ff;
	}
	else
	{
		ADDH=0xa0;
		ADDH2=0xa1;
		ADDL=Adss&0x00ff;
	}


	IICStart();

	if(IICSend(ADDH))
	{
		if(IICSend(ADDL))				//д��ַ
		{
			IICStart();
			if(IICSend(ADDH2))
			{
				ReadData=IICRead();
				NoAck();	
				IICStop();
			}
			else
			{
				ReadData=0xffff;
				IICStop();
			}
		}
		else
		{
			ReadData=0xffff;
			IICStop();
		}	
	}
	else
	{
		ReadData=0xffff;
		IICStop();
	}
	OSMutexPost(EEPROMDEVICE.IICMutex);
	return(ReadData);
}




