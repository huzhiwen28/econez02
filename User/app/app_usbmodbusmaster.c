#include <ucos_ii.h>
#include "app_usbmodbusmaster.h"
#include "bsp_usb.h"
#include "app_usbcom.h"

struct _USB_ModbusMaster USB_ModbusMaster;

//��ʼ��
void USB_ModbusMaster_Init(void)
{
	u8 err;
	USB_ModbusMaster.msg = OSSemCreate(0);
	USB_ModbusMaster.Mutex = OSMutexCreate(2,&err);
}

//ͬ��д
//addr Ŀ����ʼλ�� cnt�Ĵ������� array�Ĵ������� ���ؽ��
u8 USB_ModbusMaster_Write(unsigned char addr,unsigned char cnt,short *array)
{
	int i;

	u8 data[1000];
	u8 err;
	//��ռ�豸����ֹ����
 	OSMutexPend(USB_ModbusMaster.Mutex,0,&err);

	//��֡
	//������
	data[0] = 0x02;
	//��ʼ��ַ
	data[1] = (char)addr;
	//�Ĵ�������
	data[2] = (char)cnt;

	//�̳�������
	for(i = 0; i < cnt;i++)
	{
		//short�ĸ�8λ
		data[3+i*2] = (char)((array[i]>>8)&0xff);

		//short�ĵ�8λ
		data[3+i*2+1] = (char)((array[i])&0xff);
	}

	//LRCλ
	data[cnt*2+3]=LRC(data,cnt*2+3);

 	USB_ModbusMaster.retval = 0;
	
	//д����
	USBCOM_Write(data,4+cnt*2);

	//�ȴ�֪ͨ,2000ms��ʱ
   	OSSemPend(USB_ModbusMaster.msg,OS_TICKS_PER_SEC*2,&err);

	if(err == OS_TIMEOUT)
	{
		//�ͷ��豸
		OSMutexPost(USB_ModbusMaster.Mutex);
		return 0;
	}
	else
	{
		if(USB_ModbusMaster.retval == 1)
		{
			//�ͷ��豸
			OSMutexPost(USB_ModbusMaster.Mutex);
			return 1;
		}
		else
		{
			//�ͷ��豸
			OSMutexPost(USB_ModbusMaster.Mutex);
			return 0;
		}
	}
}


//֪ͨmodbus master�յ�����
void USB_ModbusMaster_ProcessFrame(u8* chardata,int len)
{
	//���ȴ���0�������Ӧ��ʱ
	if(len >0 )
    {
	  	//����Ӧ��
	  	//LRCУ��
	  	if( chardata[len-1] == LRC(chardata,len-1) )
	  	{
			//��������ȷ
			if(chardata[0] == 0x02)
			{
				USB_ModbusMaster.retval = 1;
			}
      	}
		//��֪ͨ
   		OSSemPost(USB_ModbusMaster.msg);
    }
}

