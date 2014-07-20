#ifndef __232C_H
#define __232C_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"


/*232C���ڳ�ʼ��*/
void BSP_232CInit(void);

/*232C���ڴ�*/
void BSP_232COpen(void);

/*232C���ڹر�*/
void BSP_232CClose(void);

/*232C���ڶ�*/
u16 BSP_232CRead(u8* buff,u16 len);

/*232C���ڶ��������г���*/
u16 BSP_232CInBuffLen(void);

/*232C����д,����0��ʾһ������,����ֵ��ʾδ������е�����ֵ*/
u16 BSP_232CWrite(u8* buff,u16 len);

/*232C�����������ڴ�ѭ����һֱ����*/
void BSP_232CRun(void);

/*485���������*/
void BSP_232CClearReadBuff(void);


#endif

