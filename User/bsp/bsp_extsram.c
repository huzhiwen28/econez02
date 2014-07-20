#include "stm32f10x.h"
#include "bsp_extsram.h"
#include "stm32f10x_fsmc.h"
/*
fsmc和i2c1连接的eeprom冲突， 不能同时使用
*/

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
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* Enable the FSMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF, ENABLE);
  
/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  /* armfly : STM32F103ZE-EK使用了A21和A22做地址译码，因此PE5,PE6需要设置为总线模式
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
 */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_3 | GPIO_Pin_4;

  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* SRAM Address lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE3 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
 	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
}


/*外扩sram初始化*/
void BSP_ExtSramInit(void)
{
  FSMC_SRAM_Init();
}

//设置马达速度
//number轴号 0对应起始马达 
//speed mm/s 马达速度
void MotorSetSpeed(int number,float speed)
{
	//每毫米对应的脉冲数量
	int ppmm = 100;
	//100M时钟
	int	clk = 100000000;
	//计算pwm设置值，为多少t，精度为小数点后4位
	int pwm = (clk/(speed*ppmm))*10000;
	int	pwm2 = pwm/2;
	//int pwm = 200;
	//int pwm2 = 100;

	//pwm整个时间，单位为t，fpga内存占用为第0-3字节
 	fpga_write(0,number*12+0);
 	fpga_write(1,(u8)((pwm>>24)&0xff) );
 	fpga_write(0,number*12+1);
 	fpga_write(1,(u8)((pwm>>16)&0xff) );
 	fpga_write(0,number*12+2);
 	fpga_write(1,(u8)((pwm>>8)&0xff) );
 	fpga_write(0,number*12+3);
 	fpga_write(1,(u8)((pwm>>0)&0xff) );

	//pwm波形中，低电平的时间，单位为t，fpga内存占用为第4-7字节
 	fpga_write(0,number*12+4);
 	fpga_write(1,(u8)((pwm2>>24)&0xff) );
 	fpga_write(0,number*12+5);
 	fpga_write(1,(u8)((pwm2>>16)&0xff) );
 	fpga_write(0,number*12+6);
 	fpga_write(1,(u8)((pwm2>>8)&0xff) );
 	fpga_write(0,number*12+7);
 	fpga_write(1,(u8)((pwm2>>0)&0xff) );
}

