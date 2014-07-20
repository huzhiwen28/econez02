#ifndef __APP_USBCOM_H
#define __APP_USBCOM_H
#include "stm32f10x.h"

/*
LRCУ��ֵ����
charbuff �������char����
len �������char���鳤��
����LRC������
*/

char LRC(u8 *charbuff,int len);


/*
ascii��ת��Ϊchar������"43" -> 0x43
charbuff ��Ž��
ascii ��ת����ascii
len ��ת���ĳ��ȣ�Ϊż��
*/
void ascii2char(u8* charbuff,u8* asciibuff,int len);

/*
charת��Ϊascii�룬����0x43 -> "43"
ascii ת���Ľ��ascii
charbuff ��ת��
len ��ת���ĳ���
*/
void chartoascii(u8* asciibuff,u8* charbuff,int len);

	//��ʼ��
	void USBCOM_Init(void);

	//���豸
	void USBCOM_Open(void);

	//�ر��豸
	void USBCOM_Close(void);

	//д�ַ�
	int USBCOM_Write(u8* chardata,int len);

	//����ִ��
	void USBCOM_Run(void *p_arg);

	//֪ͨ����������Ϣ����
	void USBCOM_Newmsg(void);

//���ڵĺ�̨�߳�
struct _USBCOM_Backend
{

	//����
	u8 buff[1024];

	//����λ�ã�ָ�����ݺ���Ŀհ�
	unsigned int buffend;

	//ͷλ��
	int frameheadindex;

	//֡����״̬�� 0û��ͷ 1��ͷ
	unsigned char framestate;

	//�豸�Ƿ��
	char comopen;

	//�����ʱ��
	u32 mstime;
	//ʱ���¼
	u32 oldmstime;

	OS_EVENT *Newmsg; //����Ϣ����֪ͨ
};


#endif
