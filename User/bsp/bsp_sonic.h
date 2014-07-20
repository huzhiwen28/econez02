#ifndef __SONIC_H
#define __SONIC_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"


/*����������ʼ��
u8 filter :ƽ�����˲����˲��ĵ�λ����1-10��Ĭ��1
u16 timems :���뵥λ�ĳ��������Լ��ʱ�䣬ʱ����ô���80ms������ز����������
*/
void BSP_SonicInit(u8 filter,u16 timems);

/*����������*/
void BSP_SonicOpen(void);

/*���������ر�*/
void BSP_SonicClose(void);

/*��������������0.1mm��λ*/
u16 BSP_SonicRead(void);

/*�������������*/
void BSP_SonicRun(void *p_arg);

#endif

