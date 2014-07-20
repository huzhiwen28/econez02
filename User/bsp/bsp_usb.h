#ifndef __USB_H
#define __USB_H 

/* Includes ------------------------------------------------------------------*/	   
#include <ucos_ii.h>
#include "stm32f10x.h"
#include "bsp_fifo.h"

/*usb�豸*/
struct _usb{
      struct _fifo ReadFIFO;
	  volatile u8 active;
      OS_EVENT *WriteMutex;//д��
      OS_EVENT *ReadMutex; //����
};

/*USB���ڳ�ʼ��*/
void BSP_USBInit(void);

/*USB���ڴ�*/
void BSP_USBOpen(void);

/*USB���ڹر�*/
void BSP_USBClose(void);

/*USB���ڶ�*/
u16 BSP_USBRead(u8* buff,u16 len);

/*USB���ڶ��������г���*/
u16 BSP_USBInBuffLen(void);

/*USB����д,����0��ʾһ������,����ֵ��ʾδ������е�����ֵ*/
u16 BSP_USBWrite(u8* buff,u16 len);

/*USB����֪ͨ�������µ��ַ��յ�*/
void BSP_USBNewRecv(void);


#endif


