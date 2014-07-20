#ifndef __485_H
#define __485_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"

/*485���ڳ�ʼ��*/
void BSP_485Init(void);

/*485���ڴ�*/
void BSP_485Open(void);

/*485���ڹر�*/
void BSP_485Close(void);

/*485���ڶ�*/
u16 BSP_485Read(u8* buff,u16 len);

/*485���ڶ��������г���*/
u16 BSP_485InBuffLen(void);

/*485����д,����0��ʾһ������,����ֵ��ʾδ������е�����ֵ*/
u16 BSP_485Write(u8* buff,u16 len);

/*485�����������ڴ�ѭ����һֱ����*/
void BSP_485Run(void);


/*485���������*/
void BSP_485ClearReadBuff(void);

#endif

