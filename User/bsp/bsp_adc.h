#ifndef __ADC_H
#define __ADC_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"
/*adc1��ͨ��10 11 12 13����ģ����
DMA��ʽ����������Ϊ20us
*/

/*ADC����ָ��*/
extern u16* BSP_ADC1;
extern u16* BSP_ADC2;
extern u16* BSP_ADC3;
extern u16* BSP_ADC4;

extern struct _ADC ADCDEVICE;

/*EEPROM�豸*/
struct _ADC{
	  /*�豸�Ƿ��*/
	  u8 active;
	  u16 ADCDataTab[4*30];
	  u8 step;
};
//struct _ADC ADCDEVICE;
/*ADC��ʼ��*/
void BSP_ADCInit(void);

/*ADC��*/
void BSP_ADCOpen(void);

/*ADC�ر�*/
void BSP_ADCClose(void);

#endif

