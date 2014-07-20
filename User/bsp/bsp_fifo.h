#ifndef __FIFO_H
#define __FIFO_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"

/*fifo��Ϊ���������ṩ������ʹ��,fifo��С�̶�Ϊ500���ֽڳ���*/

/* FIFO�ṹ��*/
struct _fifo
{
   //fifo�ķ�ʽ��ܱ����жϲ������ݲ�ȫ������
   volatile u8 FIFO[500];
   
   //ip ָ����һ������λ
   volatile u16 ip;
   
   //op ָ���һ���ַ�
   volatile u16 op;
   
   //fifo����
   volatile u16 fifol;

};

/*��ʼ��FIFO*/
u8 BSP_InitFIFO(struct _fifo* handle);

/*����0��ʾһ������������ֵ��ʾδ������е�����ֵ*/
u16 BSP_PushinFIFO(struct _fifo* handle,u8* buff,u16 len);

/*��fifo�е�ֵȡ��������ȡ���ĳ���*/
u16 BSP_PopoutFIFO(struct _fifo* handle,u8* buff,u16 len);

/*���س���*/
u16 BSP_FIFOLen(struct _fifo* handle);

u8 BSP_ClearFIFO(struct _fifo* handle);
#endif


