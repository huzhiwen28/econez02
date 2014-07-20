#include "stm32f10x.h"
#include "bsp_adc.h"
#include "stm32f10x_dma.h"
#define ADC1_DR_Address    ((u32)0x4001244C)

 struct _ADC ADCDEVICE;

/*ADC����ָ�룬��Ϊ�ⲿ���ʵĽӿ�*/
u16* BSP_ADC1;
u16* BSP_ADC2;
u16* BSP_ADC3;
u16* BSP_ADC4;

/*******************************************************************************
	��������ADC_Configuration
	��  ��:
	��  ��:
	����˵����
*/
static void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC, ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);              //72M/6=12,ADC���ʱ�䲻�ܳ���14M

  /* Configure PC.0��PC.1��PC.2��PC.3 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//�����ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(ADCDEVICE.ADCDataTab);//�ڴ��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//dma���䷽����
  DMA_InitStructure.DMA_BufferSize = 4*30;//����DMA�ڴ���ʱ�������Ĵ�С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA���������ģʽ��һ������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//����DMA���ڴ����ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���������ֳ�
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�ڴ������ֳ�
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//����DMA�����ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//����DMA��2��memory�еı����������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//��������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ�跽ʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ������ֹ
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 4;//����ת����ͨ����
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels configuration [����ģʽͨ������]*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��10,�������˳��ֵΪ1,����ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��11,�������˳��ֵΪ2,����ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��12,�������˳��ֵΪ3,����ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5); //ADC1,ADCͨ��13,�������˳��ֵΪ4,����ʱ��Ϊ239.5����

  /* Enable ADC1 DMA [ʹ��ADC1 DMA]*/
  ADC_DMACmd(ADC1, ENABLE);
 
  /* Enable ADC1 [ʹ��ADC1]*/
  ADC_Cmd(ADC1, ENABLE); 

  /* Enable ADC1 reset calibaration register */  
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);		  //ע��˾��λ�ã������з�ӳ����Ѵ˾����ADC1���ú�У׼֮ǰ������ִ�λ����
   
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*ADC��ʼ��*/
void BSP_ADCInit(void)
{
  ADC_Configuration();

  BSP_ADC1 = ADCDEVICE.ADCDataTab;
  BSP_ADC2 = ADCDEVICE.ADCDataTab + 1;
  BSP_ADC3 = ADCDEVICE.ADCDataTab + 2;
  BSP_ADC4 = ADCDEVICE.ADCDataTab + 3;
}

/*ADC��*/
void BSP_ADCOpen(void)
{
  ADCDEVICE.active = 1;
}

/*ADC�ر�*/
void BSP_ADCClose(void)
{
  ADCDEVICE.active = 0;
}

