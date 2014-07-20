#include <ucos_ii.h>
#include "app_backend.h"
#include "app_kernelregs.h"
#include "app_usbgather.h"
#include "app_usbmodbusmaster.h"
#include "bsp_usb.h"

//��̨ģ��
struct _App_Backend App_Backend;

//Ӧ�ú�̨��ʼ��
void App_Backend_Init(void)
{
  	App_Backend.PCOnline = 0;
  	App_Backend.RespFailCnt = 0;
  	App_Backend.GatherFlag = 0;
	App_Backend.FlushParamsg = OSSemCreate(0);
	
  	KernelRegs[79] = 0;
}

//����ˢ������
void App_FlushParaTask_Run(void *p_arg)
{
	while(1)
	{
		u8 err;

		//�ȴ�֪ͨ
   		OSSemPend(App_Backend.FlushParamsg,0,&err);
	   if((App_Backend.PCOnline == 1))
	   {
	   		//��������,ʧ������
			if(USB_ModbusMaster_Write(0,111,KernelRegs) == 0)
			{
				//�ȴ�-����
				OSTimeDlyHMSM(0, 0, 0, 100);
				USB_ModbusMaster_Write(0,111,KernelRegs);	
			}
		}
	}
}


//��������
void App_HeartbeatTask_Run(void *p_arg)
{
	while(1)
	{
	   if((App_Backend.PCOnline == 1))
	   {
	   		if(USB_ModbusMaster_Write(80,20,&KernelRegs[80]))
			{
				App_Backend.RespFailCnt = 0;
			}
			else
			{
				App_Backend.RespFailCnt ++;

				//5�ζ�ʧ������ΪPC����
				if(App_Backend.RespFailCnt >= 5)
				{
					u8 err;

				    App_Backend.PCOnline = 0;
	
					//ֹͣ�ɼ�
					OSMutexPend(RegsWriteMutex,0,&err);
			        KernelRegs[79] = KernelRegs[79] & ~(0x0001<<1);
					OSMutexPost(RegsWriteMutex);
			        App_Backend.GatherFlag = 0;
					USB_Gather_Close();
			  	 }
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 1000);

	}
}

void App_Backend_NewFrame(void)
{
    //��һ��PC������
    if(App_Backend.PCOnline == 0)
	{
	    //���ͺ����������
	    App_Backend.PCOnline = 1;
        App_Backend.RespFailCnt = 0;
		//��֪ͨ
   		OSSemPost(App_Backend.FlushParamsg);
	}

	//1�ɼ�
    if( App_Backend.GatherFlag != (char)((KernelRegs[79]>>1) & 0x0001))
	{
		App_Backend.GatherFlag = (char)((KernelRegs[79]>>1) & 0x0001);
		//�ɼ��������
		if(App_Backend.GatherFlag == 0x01)
		{
		  //����ͨ���ռ�
		  unsigned char channelcnt = 0;
		  unsigned char channelarray[10];
		  int i = 0;

		  for(i = 0;i < 10;i++)
		  {
		    if(KernelRegs[i+100] != 0)
			{
			    channelarray[i] = (u8)KernelRegs[i+100];
				channelcnt++;
			}
			else
			{
			    break;
			}
		  }
		  if(channelcnt > 0)
		  {
		    if(KernelRegs[110] > 0)
			{
		      USB_Gather_Init(channelcnt,channelarray,KernelRegs[110]);
			  USB_Gather_Open();
			}
			else
			{
		      USB_Gather_Init(channelcnt,channelarray,1000);
			  USB_Gather_Open();
			}
		  }
		}
		else
		{
  			USB_Gather_Close();
		}
	}

	//2ˢ����
    if( (char)((KernelRegs[79]>>2) & 0x0001) == 0x01)
	{
		u8 err;
		//��֪ͨ
   		OSSemPost(App_Backend.FlushParamsg);

		//�����������
		OSMutexPend(RegsWriteMutex,0,&err);
		KernelRegs[79] = KernelRegs[79] & ~(0x0001<<2);
		OSMutexPost(RegsWriteMutex);
	}

	//3ֹͣ��PC��ϵ
    if( (char)((KernelRegs[79]>>3) & 0x0001) == 0x01)
	{
		u8 err;
        App_Backend.PCOnline = 0;
		//�����������
		OSMutexPend(RegsWriteMutex,0,&err);
		KernelRegs[79] = KernelRegs[79] & ~(0x0001<<3);
		OSMutexPost(RegsWriteMutex);
	}
}


