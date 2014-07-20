#ifndef __APP_USBMODBUSMASTER_H
#define __APP_USBMODBUSMASTER_H
#include "stm32f10x.h"

//��ʼ��
void USB_ModbusMaster_Init(void);

//֪ͨmodbus master��������֡
void USB_ModbusMaster_ProcessFrame(u8* chardata,int len);

//�첽д
//addr Ŀ����ʼλ�� cnt�Ĵ������� array�Ĵ�������
u8 USB_ModbusMaster_Write(unsigned char addr,unsigned char cnt,short *array);

//ModbusMaster
struct _USB_ModbusMaster
{
 	//д���ݽ�� 0ʧ�� 1�ɹ�
	char retval;

	OS_EVENT *msg; //��Ϣ����֪ͨ
	OS_EVENT *Mutex; //��ռ��
};

#endif
