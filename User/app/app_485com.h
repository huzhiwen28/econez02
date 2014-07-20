#ifndef __APP_485COM_H
#define __APP_485COM_H
#include "stm32f10x.h"

/*modbus֡��д*/

//CRCУ��ĺ���
u16 CRC16(unsigned char *puchMsg, unsigned short usDataLen);

//��ʼ��
void C485COM_Init(void);

//���豸
void C485COM_Open(void);

//�ر��豸
void C485COM_Close(void);

//д�ַ�
int C485COM_Write(u8* chardata,int len);

//����ִ��
void C485COM_Run(void *p_arg);


#endif
