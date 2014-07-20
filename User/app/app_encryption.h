#ifndef __APP_ENCRYPTION_H
#define __APP_ENCRYPTION_H
#include "stm32f10x.h"
#include <ucos_ii.h>


struct _App_Encryption
{
	OS_EVENT *Invalidmsg; //�Ƿ���Ϣ
	OS_SEM_DATA Result;
  	unsigned int Device_Serial0;
	unsigned int Device_Serial1;
	unsigned int Device_Serial2;
};

extern struct _App_Encryption App_Encryption;

//��ʼ��
void App_Encryption_Init(void);

//У��
void App_Encryption_Verify(void);

//���ǷǷ���0 �Ϸ� 1�Ƿ�
char App_Encryption_Invalid(void);

//�㱨cpuid����λ��
void  App_Encryption_ReportID(u8* chardata,int len);

//д���ܴ�
void  App_Encryption_WriteEncry(u8* chardata,int len);

#endif
