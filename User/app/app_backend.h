#ifndef __APP_BACKEND_H
#define __APP_BACKEND_H
#include "stm32f10x.h"


struct _App_Backend
{
  	//PC���Ƿ�����
  	volatile char PCOnline;

  	//����Ӧ��ʧ�ܼ���
  	volatile char RespFailCnt;

  	volatile char GatherFlag;
	OS_EVENT *FlushParamsg; //ˢ�²�����Ϣ����֪ͨ
};

extern struct _App_Backend App_Backend;

//�����µ�����֡
void App_Backend_Init(void);
void App_Backend_NewFrame(void);

//��������
void App_HeartbeatTask_Run(void *p_arg);

//����ˢ������
void App_FlushParaTask_Run(void *p_arg);

//��������


//������������


#endif
