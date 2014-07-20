#include <ucos_ii.h>
#include "app_usbmodbusslave.h"
#include "bsp_usb.h"
#include "app_usbcom.h"
#include "app_kernelregs.h"
#include "app_backend.h"

//modbus slave��������
void USB_ModbusSlave_ProcessFrame(u8* chardata,int len)
{
	//֡��ȷ����ʶ
	char Flag = 0;

	//�������Ϊ0��������
	if(len >0 )
	{
		u8 SendData[1024];

		//����Ӧ��
		//LRCУ�飬֡�Ƿ���ȷ
		if( chardata[len-1] == LRC(chardata,len-1) )
		{
			//��������ȷ����λ�����͵Ĺ�����
			if(chardata[0] == 0x01)
			{
			    int i = 0;
				//�������ݣ�д�Ĵ���
				for(i = 0;i < chardata[2];i++)
				{
					short sd = chardata[3+i*2];
					sd = sd << 8;//��8λ
					sd = sd + chardata[3+i*2 + 1];//��8λ
					
					//�޸ı�ʶλ
					if(sd != KernelRegs[chardata[1]+i] 
					&& ((chardata[1]+i) < 80) )
					{
					  RegsFlag[chardata[1]+i] = 1;
					  KernelRegs[chardata[1]+i] = sd;
					}
					else
					{
					  KernelRegs[chardata[1]+i] = sd;
					}
				}
			}
			else
			{
			}
			Flag = 1;
		}
		else
		{
		}

		//Ӧ��PC��
		if(Flag == 1)
		{
			//modbus��֡����ȷ��Ӧ��ʽ
			SendData[0] = 0x01;
			SendData[1] = chardata[1];
			SendData[2] = chardata[2];
			SendData[3] = LRC(SendData,3);
			//д����
			USBCOM_Write(SendData,4);
		}
		else
		{
			//modbus��֡�����ϰ�Ӧ��ʽ
			SendData[0] = 0x81;
			SendData[1] = 0x01;
			SendData[3] = LRC(SendData,2);
			//д����
			USBCOM_Write(SendData,3);
		}
		//Ӧ�ô���
   		App_Backend_NewFrame();
	}
}




