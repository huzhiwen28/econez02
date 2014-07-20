#ifndef __DAC_H
#define __DAC_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"
/*�ⲿDACʹ��SPI����
*/

/*DAC����ָ��*/
extern u16* BSP_DAC1;
extern u16* BSP_DAC2;
extern u16* BSP_DAC3;
extern u16* BSP_DAC4;

/*DAC�豸*/
struct _DAC{
	  /*�豸�Ƿ��*/
	  u8 active;
	  u16 mode;
	  u16 DACDataTab[4];
};

/*DAC��ʼ��*/
void BSP_DACInit(void);

/*DAC��*/
void BSP_DACOpen(void);

/*DAC�ر�*/
void BSP_DACClose(void);

/*DAC���������ʽ*/
void BSP_DACMode(u8 mode);

/*DAC��������*/
void BSP_DACRun(void *p_arg);

/*DAC���ģʽ����*/
void DacOutMode(u16 Ch ,u16 OutMode);

void  DacOutEN(u16 CHX,u16 OutX);

#endif

