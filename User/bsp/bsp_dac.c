#include <ucos_ii.h>
#include "stm32f10x.h"
#include "bsp_dac.h"
#include "stm32f10x_spi.h"
#include "app_kernelregs.h"
#include "app_basicdef.h"

struct _DAC DACDEVICE;
u16 DacDeviceControlCh1;
u16 DacDeviceControlCh2;

/* Defines for the SPI and GPIO pins used to drive the SPI DAC */
#define SPI_DAC                 SPI2
#define SPI_DAC_CLK             RCC_APB1Periph_SPI2
#define SPI_DAC_GPIO            GPIOB
#define SPI_DAC_GPIO_CLK        RCC_APB2Periph_GPIOB
#define SPI_DAC_PIN_SCK         GPIO_Pin_13
#define SPI_DAC_PIN_MISO        GPIO_Pin_14
#define SPI_DAC_PIN_MOSI        GPIO_Pin_15

#if defined (USE_STM3210B_EVAL)
 #define SPI_DAC_CS             GPIO_Pin_12
 #define SPI_DAC_CS_GPIO        GPIOB
 #define SPI_DAC_CS_GPIO_CLK    RCC_APB2Periph_GPIOB
#elif defined (USE_STM3210E_EVAL)
 #define SPI_DAC_CS             GPIO_Pin_12
 #define SPI_DAC_CS_GPIO        GPIOB
 #define SPI_DAC_CS_GPIO_CLK    RCC_APB2Periph_GPIOB
#endif

#define SPI_DACDRV_CS             GPIO_Pin_4
#define SPI_DACDRV_CS_GPIO        GPIOA
#define SPI_DACDRV_CS_GPIO_CLK    RCC_APB2Periph_GPIOA

//5750 PA10

/* Exported macro ------------------------------------------------------------*/
/* Select SPI DAC: Chip Select pin low  */
#define SPI_DAC_CS_LOW()       GPIO_ResetBits(SPI_DAC_CS_GPIO, SPI_DAC_CS)
/* Deselect SPI DAC: Chip Select pin high */
#define SPI_DAC_CS_HIGH()      GPIO_SetBits(SPI_DAC_CS_GPIO, SPI_DAC_CS)

/* Select SPI DAC: Chip Select pin low  */
#define SPI_DACDRV_CS_LOW()       GPIO_ResetBits(SPI_DACDRV_CS_GPIO, SPI_DACDRV_CS)
/* Deselect SPI DAC: Chip Select pin high */
#define SPI_DACDRV_CS_HIGH()      GPIO_SetBits(SPI_DACDRV_CS_GPIO, SPI_DACDRV_CS)

/* Exported functions --------------------------------------------------------*/
/*----- High layer function -----*/
void SPI_DAC_SendByte(uint8_t byte);
void SPI_DAC_Init(void);

/*----- Low layer function -----*/
void SPI_DAC_SendHalfWord(uint16_t HalfWord);


/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void SPI_FLASH_Init(void)
{

   //�������ֽ�
    u8 ch = 0;
    u32 d = 0;
//	u32 d2 = 0;

  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable SPI and GPIO clocks */
  RCC_APB1PeriphClockCmd(SPI_DAC_CLK , ENABLE);
  RCC_APB2PeriphClockCmd(SPI_DAC_GPIO_CLK | SPI_DAC_CS_GPIO_CLK | SPI_DACDRV_CS_GPIO_CLK, ENABLE);

  /* Configure SPI pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_DAC_PIN_SCK | SPI_DAC_PIN_MISO | SPI_DAC_PIN_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_50MHz;
  GPIO_Init(SPI_DAC_GPIO, &GPIO_InitStructure);

  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = SPI_DAC_CS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_DAC_CS_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_DACDRV_CS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_DACDRV_CS_GPIO, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  SPI_DAC_CS_HIGH();
  SPI_DACDRV_CS_HIGH(); 

  SPI_Cmd(SPI_DAC, DISABLE);

  /* SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI_Direction_2Lines_FullDuplex;	  //˫��ģʽ
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							  //SPI��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						  //8bit����
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							  //CLK����ʱΪ�ߵ�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							  //CLK�����ز�������Ϊ�������ǵڶ������ض���������Ҳ�������Ϊ�ڶ������ز���
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								  //Ƭѡ���������
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;	  //SPIƵ�ʣ�72M/8 = 9M
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					  //��λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;								  //crc7��stm32spi��Ӳ��ecc
  SPI_Init(SPI_DAC, &SPI_InitStructure);

  /* Enable the SPI  */
  SPI_Cmd(SPI_DAC, ENABLE);

  OSTimeDly(1);

	//All channel		DAֵ��ʼ���
    SPI_DAC_CS_LOW();
 
	d = d | (0x00000001 << 19);
	d = d | (0x00000001 << 20);
	d = d | (0x00000007 << 16);			// ����ͨ��
 	d = d | (((u32)(2500)) << 4);

 	ch = (u8)(d >> 16) & 0xff ;
    SPI_DAC_SendByte(ch);

	ch = (u8)(d >> 8) & 0xff;
    SPI_DAC_SendByte(ch);

	ch = (u8)(d) & 0xff;
    SPI_DAC_SendByte(ch);

  	OSTimeDly(1);
    SPI_DAC_CS_HIGH();
  	OSTimeDly(1);

   /*
	//All channel	   AD5624/5644�ο���ѹ����
    SPI_DAC_CS_LOW();
 
	d = d | (0x00000001 << 21);
	d = d | (0x00000001 << 19);
	d = d | (0x00000001 << 20);		//������ѹ�������� 1 1 1
	d = d | (0x00000000 << 16);	    //����ͨ��
 	d = d | 0x00000000;				//�ڲ��ο���ѹ

 	ch = (u8)(d >> 16) & 0xff ;
    SPI_DAC_SendByte(ch);

	ch = (u8)(d >> 8) & 0xff;
    SPI_DAC_SendByte(ch);

	ch = (u8)(d) & 0xff;
    SPI_DAC_SendByte(ch);

 	BSP_DelayUS(200);

    SPI_DAC_CS_HIGH();
 	BSP_DelayUS(200);

   */
 }

/*
  * @brief  Sends a byte through the SPI interface and return the byte received
  *   from the SPI bus.			   ͨ��SPIx����һ�����ݣ�ͬʱ����һ������
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
void SPI_DAC_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI_DAC, SPI_I2S_FLAG_TXE) == RESET)
	OSTimeDlyHMSM(0, 0, 0, 1);//������ͼĴ�������û�з����꣬ѭ���ȴ�

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI_DAC, byte);									 //�����ͼĴ���д��Ҫ���͵�����
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *   received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
void SPI_DAC_SendHalfWord(uint16_t HalfWord)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI_DAC, SPI_I2S_FLAG_TXE) == RESET)
	OSTimeDlyHMSM(0, 0, 0, 1);//������ͼĴ�������û�з����꣬ѭ���ȴ�

  /* Send Half Word through the SPI_FLASH peripheral */
  SPI_I2S_SendData(SPI_DAC, HalfWord);
}

/*DAC��ʼ��*/
void BSP_DACInit(void)
{
  DACDEVICE.mode = 0;
  SPI_FLASH_Init();
  DacOutMode(0,Param.AOutPort.CH1OutMode);		   //2+-10V   3: +-5V	   //��һ��������ͨ��ID��1`4)��,��2�������ʽ (0~6)
  DacOutMode(1,Param.AOutPort.CH2OutMode);		   //2+-10V   3: +-5V	   //��һ��������ͨ��ID��1`4)��,��2�������ʽ (0~6)
}

/*DAC��*/
void BSP_DACOpen(void)
{
   DACDEVICE.active = 1;
}

/*DAC�ر�*/
void BSP_DACClose(void)
{
   DACDEVICE.active = 0;
}

/*DAC���������ʽ*/
void BSP_DACMode(u8 mode)
{
   DACDEVICE.mode = mode;
}

/*DAC��������*/
void BSP_DACRun(void *p_arg)
{
  u16 DA4D,DA3D;
  static u16 CH1OutEN, CH2OutEN;
  BSP_DACInit();
  BSP_DACOpen();
  while(1)
  {
  	if((DA3D!=Port.AOut.Ch1)||(DA4D!=Port.AOut.Ch2))
	{
 	DA3D=Port.AOut.Ch1;
	DA4D=Port.AOut.Ch2;
//	DA3D=3000;
//	DA4D=2000;
	  if(DACDEVICE.active == 1)
	  {
	    u8 ch = 0;		 	    //�������ֽ�
	    u32 d = 0;		
	/*ͨ��3*/
		if(State.Out.Data>0)
		{
			SPI_DAC_CS_LOW();
		 
			d = d | (0x00000001 << 19);
			d = d | (0x00000001 << 20);	  		//����
			d = d | (0x00000000 << 16);		  //2��ʾͨ��3
		 	d = d | (DA3D << 4);		  //����
		
		 	ch = (u8)(d >> 16) & 0xff ;
		    SPI_DAC_SendByte(ch);
		
			ch = (u8)(d >> 8) & 0xff;
		    SPI_DAC_SendByte(ch);
		
			ch = (u8)(d) & 0xff;
		    SPI_DAC_SendByte(ch);
		
		  	OSTimeDly(1);
		
		    SPI_DAC_CS_HIGH();
		
		  	OSTimeDly(1);
			if(CH1OutEN==0)
			{
				DacOutEN(0,1); 		//��
				CH1OutEN=1;
			}
		}
		else
		{
			if(CH1OutEN==1)
			{
				DacOutEN(0,0); 		//�ر�
				CH1OutEN=0;
			}

		}
	
		//*ͨ��2
		if(State.Out.Data2>0)
		{
		 	 d=0;
			SPI_DAC_CS_LOW();
		 
			d = d | (0x00000001 << 19);
			d = d | (0x00000001 << 20);	  		//����
			d = d | (0x00000001 << 16);		  //2��ʾͨ��3
		 	d = d | (DA4D << 4);		  //����
		
		 	ch = (u8)(d >> 16) & 0xff ;
		    SPI_DAC_SendByte(ch);
		
			ch = (u8)(d >> 8) & 0xff;
		    SPI_DAC_SendByte(ch);
		
			ch = (u8)(d) & 0xff;
		    SPI_DAC_SendByte(ch);
		
		  	OSTimeDly(1);
		
		    SPI_DAC_CS_HIGH();
		
		  	OSTimeDly(1);
			if(CH2OutEN==0)
			{
				DacOutEN(1,1); 		//��
				CH2OutEN=1;
			}
	 	}
		else
		{
			if(CH2OutEN==1)
			{
				DacOutEN(1,0); 		//�ر�
				CH2OutEN=0;
			}

		}
 	}
	}
	OSTimeDlyHMSM(0, 0, 0, 1);
   }
}

void  DacOutEN(u16 CHX,u16 OutX)		//CHX,0:ͨ��1��1��ͨ��1  OutX��0���ر������1��
{
 u16  SPIData,ch1;
	if(	CHX==0)		//ͨ��1
	{
		if(OutX==0)	   //�ر����
		{
		 	SPIData= DacDeviceControlCh1;
		}
		else
		{
			SPIData= DacDeviceControlCh1|0x0020;  //bit5  =1;
		}

	}
	else  if(CHX==1)
	{
		if(OutX==0)	   //�ر����
		{
		 	SPIData= DacDeviceControlCh2;
		}
		else
		{
			SPIData= DacDeviceControlCh2|0x0020;  //bit5  =1;
		}
	}
	  SPI_DACDRV_CS_LOW();
	  OSTimeDly(1);
	
	  ch1 = (u8)(SPIData >> 8) & 0xff;
	  SPI_DAC_SendByte(ch1);
	  ch1 = (u8)(SPIData) & 0xff;
	  SPI_DAC_SendByte(ch1);
	  OSTimeDly(1);
	  SPI_DACDRV_CS_HIGH();
}

void DacOutMode(u16 Ch ,u16 OutMode)
{
u32  OMode,udata1;
u16   ch1;
	switch(Ch)
	{
	  case 0:ch1=0;break ; //0~5V
	  case 1:ch1=4;break ; //0~10V
	}

	switch(OutMode)
	{
	  case 0:OMode=5;break ; //0~5V
	  case 1:OMode=6;break ; //0~10V
	  case 2:OMode=8;break ; //+/-10V
	  case 3:OMode=7;break ; //+/-5V
	  case 4:OMode=0;break ; //4~20mA
	  case 5:OMode=1;break ; //0~20mA
	  case 6:OMode=3;break ; //+/-20mA
//	  case 7:OMode=5;break ; //0~5V
	}

	if((OutMode==2)||(OutMode==3)||(OutMode==6))		  //���Ϊ�������ʱ
	{
	    u8 ch = 0;		 	    //�������ֽ�
	    u32 d = 0;		
		SPI_DAC_CS_LOW();
	 
		d = d | (0x00000001 << 19);
		d = d | (0x00000001 << 20);	  		//����
		d = d | (Ch << 16);		  //2��ʾͨ��3
	 	d = d | (2048 << 4);		  //����
	
	 	ch = (u8)(d >> 16) & 0xff ;
	    SPI_DAC_SendByte(ch);
	
		ch = (u8)(d >> 8) & 0xff;
	    SPI_DAC_SendByte(ch);
	
		ch = (u8)(d) & 0xff;
	    SPI_DAC_SendByte(ch);
	
	  	OSTimeDly(1);
	
	    SPI_DAC_CS_HIGH();
	
	  	OSTimeDly(1000);				  //��ʱ����ֹ�ϵ�ʱ�������
	}
	else
	{
 	    u8 ch = 0;		 	    //�������ֽ�
	    u32 d = 0;		
		SPI_DAC_CS_LOW();
	 
		d = d | (0x00000001 << 19);
		d = d | (0x00000001 << 20);	  		//����
		d = d | (Ch << 16);		  //2��ʾͨ��3
	 	d = d | (0 << 4);		  //����
	
	 	ch = (u8)(d >> 16) & 0xff ;
	    SPI_DAC_SendByte(ch);
	
		ch = (u8)(d >> 8) & 0xff;
	    SPI_DAC_SendByte(ch);
	
		ch = (u8)(d) & 0xff;
	    SPI_DAC_SendByte(ch);
	
	  	OSTimeDly(1);
	
	    SPI_DAC_CS_HIGH();
	
	  	OSTimeDly(1);
	}

		  udata1 = 0;
	  udata1 = udata1 | (ch1 << 13);//ͨ��1
	  udata1 = udata1 | (OMode << 7); //
	  udata1 = udata1 | (0x0001 << 5);		   //�������	0��ʹ�������1ʹ��
	  udata1 = udata1 | (0x0001 << 3);	  //0�ⲿ�����裬1�ڲ�
	  if( Ch==0)
	  {
	  	DacDeviceControlCh1= udata1 ; //��¼�����֣�����������ƶ˿���
	 }
	 else if( Ch==1)
	 {
 	  	DacDeviceControlCh2= udata1 ; //��¼�����֣�����������ƶ˿���
	 }
	  SPI_DACDRV_CS_LOW();
	  OSTimeDly(1);
	
	  ch1 = (u8)(udata1 >> 8) & 0xff;
	  SPI_DAC_SendByte(ch1);
	  ch1 = (u8)(udata1) & 0xff;
	  SPI_DAC_SendByte(ch1);
	  OSTimeDly(1);
	  SPI_DACDRV_CS_HIGH();
}


