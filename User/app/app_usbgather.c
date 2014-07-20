#include <ucos_ii.h>
#include "app_usbgather.h"
#include "bsp_usb.h"
//#include <stdlib.h>

#include "app_usbcom.h"
#include "app_kernelregs.h"

struct _USB_Gather USB_Gather_Task;

void USB_Gather_Write()
{
	int i;
	//�����͵Ĳɼ�����
	u8 data[50];
	u8 cnt;
	//malloc(3);

	//��֡
	//������
	data[0] = 0x03;

	//�ɼ��Ĵ�������
	data[1] = (u8)USB_Gather_Task.GatherNum;
	cnt = (u8)USB_Gather_Task.GatherNum;

	//�̳�������
	for(i = 0; i < cnt;i++)
	{
		//short�ĸ�8λ
		data[2+i*2] = (u8)((KernelRegs[USB_Gather_Task.GatherRegs[i]]>>8)&0xff);

		//short�ĵ�8λ
		data[2+i*2+1] = (u8)((KernelRegs[USB_Gather_Task.GatherRegs[i]])&0xff);
	}

	//LRCλ
	data[cnt*2+2]=LRC(data,cnt*2+2);

    //д����
    USBCOM_Write(data,3+cnt*2);
}

	//��ʼ���ɼ�����
	//num ,�ɼ��ĵ�λ����
	//regs,�ɼ��ĵ�λ�������
	//mstime,�ɼ�ʱ����������
void USB_Gather_Init(u8 num,u8* regs,int gamstime)
{
  int i = 0;
  for(i = 0;i < num;i++)
  {
     USB_Gather_Task.GatherRegs[i] = regs[i];
  }
  USB_Gather_Task.GatherNum = num;
  USB_Gather_Task.GatherTimeMs = gamstime;
  USB_Gather_Task.OpenFlag = 0;
}

//���豸
void USB_Gather_Open(void)
{
	if(USB_Gather_Task.OpenFlag == 0)
	{
		USB_Gather_Task.OpenFlag = 1;
	}
}

//�ر��豸
void USB_Gather_Close(void)
{
	if(USB_Gather_Task.OpenFlag == 1)
	{
		//����ֹͣ
		USB_Gather_Task.OpenFlag = 0;
	}
}

//����ִ��
void USB_Gather_Run(void *p_arg)
{
	while(1)
	{
		//��̨�����
		if(USB_Gather_Task.OpenFlag == 1 )
		{
	    	USB_Gather_Write();
			OSTimeDlyHMSM(0, 0, 0, USB_Gather_Task.GatherTimeMs);
		}
		OSTimeDlyHMSM(0, 0, 0, 1);
	}
}
