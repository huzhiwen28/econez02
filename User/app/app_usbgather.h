#ifndef __APP_USBGATHER_H
#define __APP_USBGATHER_H
#include "stm32f10x.h"

	//��ʼ���ɼ�����
	//num ,�ɼ��ĵ�λ����
	//regs,�ɼ��ĵ�λ�������
	//mstime,�ɼ�ʱ����������
	void USB_Gather_Init(u8 num,u8* regs,int mstime);

	//���豸
	void USB_Gather_Open(void);

	//�ر��豸
	void USB_Gather_Close(void);

	//����ִ��
	void USB_Gather_Run(void *p_arg);


//Gather
struct _USB_Gather
{
	//�ɼ��ĵ�λ����
	u8 GatherNum;

	//�ɼ��ĵ�λ���飬������Ϊ�Ĵ������
	u8 GatherRegs[10];

	//�ɼ�ʱ����������
	int GatherTimeMs;

	//�Ƿ�� 0δ�� 1��
	char OpenFlag;
};


#endif

