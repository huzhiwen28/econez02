#include "stm32f10x.h"
#include "bsp_extsram.h"
#include "stm32f10x_fsmc.h"
/*
fsmc��i2c1���ӵ�eeprom��ͻ�� ����ͬʱʹ��
*/
static FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	 /* �ں����ڶ�������Ī������Ĵ��� */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None 
  * @retval None
  */
void FSMC_SRAM_Init(void)
{
  	FSMC_NORSRAMTimingInitTypeDef  p;
  	GPIO_InitTypeDef GPIO_InitStructure; 

	/*ʹ��FSMC����ʱ��*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
      
	/*ʹ��FSMC����ʹ�õ�GPIO��ʱ��*/  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
                         RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);


    /*FSMC������FSMC_D[0:15]��ʼ�������츴�����*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15| GPIO_Pin_14;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
  
    /*FSMC��ַ��FSMC_A[0:17]��ʼ�������츴�����*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                  GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                  GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  	/*FSMC NOE��NWE���Ի������츴�����*/  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  	/*FSMC NE1���Ի������츴�����*/  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  	/*FSMC NBL0��NBL1���Ի������츴�����*/  
  	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  	//GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
/*--------------FSMC ���� �洢����������------------------------------*/
  	p.FSMC_AddressSetupTime = 0;           //��ַ����ʱ��    
  	p.FSMC_AddressHoldTime = 0;            //��ַ����ʱ��  
  	p.FSMC_DataSetupTime = 1;              //���ݽ���ʱ��
  	p.FSMC_BusTurnAroundDuration = 0;      //���߻ָ�ʱ��
  	p.FSMC_CLKDivision = 0;                // ʱ�ӷ�Ƶ���� 
  	p.FSMC_DataLatency = 0;          	   //���ݲ���ʱ��
  	p.FSMC_AccessMode =  FSMC_AccessMode_A; //FSMC NOR������ʱ��
  	
/*--------------FSMC ���� ��������------------------------------*/
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                  //ʹ����FSMC��BANK1���Ӱ��1             
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//��ֹ��ַ�����߸���
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;           //�洢������ΪSRAM
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;  //�洢�����ݿ��Ϊ8λ
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; //�ر�ͻ��ģʽ����
    //�ȴ��ź����ȼ���ֻ����ʹ��ͻ������ģʽ����Ч
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; 
    //�ر�Wrapped burst access mode��ֻ����ʹ��ͻ������ģʽ����Ч
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;         
    //�ȴ��ź����ã�ֻ����ʹ��ͻ������ģʽ����Ч
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;     //ʹ�����BANK��д����
    //ʹ��/�رյȴ���Ϣ���ã�ֻ��ʹ��ͻ������ģʽ����Ч
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;     
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; //����Extend Mode
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;     //�ر�Write Burst Mode   
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;               //������ʱ�����
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;                   //д����ʱ�����
 	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;

  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

/*--------------ʹ��BANK1���Ӱ��1------------------------------*/
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}


/*����sram��ʼ��*/
void BSP_ExtSramInit(void)
{
  FSMC_SRAM_Init();
}


//��������ٶ�
//number��� 0��Ӧ��ʼ��� 
//speed m/s ����ٶ�
void MotorSetSpeed(int number,double speed)
{
	//����
	u8 dir = 0;
	//ÿ���׶�Ӧ����������
	u32 ppmm = 100;
	//100Mʱ��
	u32	clk = 100000000;
	uint64_t maxu32 = 4294967295;
	
	//����pwm����ֵ��Ϊ����t������ΪС�����4λ
	u32 pwm;
	u32 pwm2;
	if((speed) >= 0)
		pwm = (double)((uint64_t)maxu32/((uint64_t)1000))*speed;
	else
		pwm = (double)((uint64_t)maxu32/((uint64_t)1000))*(0-speed);

	//fpga�ڴ�ռ��Ϊ��0-3�ֽ�
 	fpga_write(0,number*12+0);
 	fpga_write(1,(u8)((pwm>>24)&0xff) );
 	fpga_write(0,number*12+1);
 	fpga_write(1,(u8)((pwm>>16)&0xff) );
 	fpga_write(0,number*12+2);
 	fpga_write(1,(u8)((pwm>>8)&0xff) );
 	fpga_write(0,number*12+3);
 	fpga_write(1,(u8)((pwm>>0)&0xff) );

	if(speed >= 0)
	{
		fpga_write(0,4);
 		dir = fpga_read(1);//ȡ���Ϸ���
		dir = dir | (0x01 << number);
		fpga_write(0,4);
 		fpga_write(1,dir);//д����
	}
	else
	{
		fpga_write(0,4);
 		dir = fpga_read(1);//ȡ���Ϸ���
		dir = dir & (~(0x01 << number));
		fpga_write(0,4);
 		fpga_write(1,dir);//д����
	}
}

//����ٶ���Ч���������Ա�֤��������ٶ�һ�µ�ˢ��
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ������ٶ�ֵˢ��
void MotorSpeedFlush(u8 bits)
{
 	//����ˢ��λ����ʾpwm��pwm2�����иı䣬ˢ�µ������Ĵ����У����߻�������ݲ�ͬ��������fpga�ڴ�ռ�õ�96�ֽ�
	fpga_write(0,96);
 	fpga_write(1,bits);
}


//���PWM�����Ч
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ������������,0��ʾ��Ӧ�����ֹͣ�������
void MotorPwmout(u8 bits)
{
	fpga_write(0,97);
 	fpga_write(1,bits);

}

//��ﵱǰ��������ˢ�£�����stm32��������������
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ�������������ˢ��
void MotorOutCntFlush(u8 bits)
{			   
	fpga_write(0,98);
 	fpga_write(1,bits);
}


//��ȡ��ﵱǰ���͵���������
//number��� 0��Ӧ��ʼ��� 
int MotorReadOutCnt(int number)
{
	int cnt = 0;
	fpga_write(0,number*12+11);
 	cnt += fpga_read(1);
 	fpga_write(0,number*12+10);
 	cnt = cnt | (fpga_read(1))<<8;
 	fpga_write(0,number*12+9);
 	cnt = cnt | (fpga_read(1))<<16;
 	fpga_write(0,number*12+8);
 	cnt = cnt | (fpga_read(1))<<24;
	return cnt;
}

void Motorpingpong(void)
{			   
	fpga_write(0,99);
  	fpga_write(1,0xff);
}

