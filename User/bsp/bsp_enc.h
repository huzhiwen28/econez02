#ifndef __ENC_H
#define __ENC_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"
/*
*/
extern int64_t* BSP_ENC_CNT;

/*ENCODER�豸*/
struct _ENC{
	  /*�豸�Ƿ��*/
	  u8 active;
	  int64_t currentCount;
};

/*ENC��ʼ��*/
void BSP_ENCInit(void);

/*ENC��*/
void BSP_ENCOpen(void);

/*ENC�ر�*/
void BSP_ENCClose(void);

/*ENC��������*/
void BSP_ENCRun(void *p_arg);

#endif

