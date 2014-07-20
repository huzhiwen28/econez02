#include <ucos_ii.h>
#include "app_kernelregs.h"
#include "app_encryption.h"
#include "app_usbcom.h"

//CRCУ��ĺ���
u16 CRC16(unsigned char *puchMsg, unsigned short usDataLen);
extern u8 Encryption[];

struct _App_Encryption App_Encryption;

//��ʼ��
void App_Encryption_Init(void)
{
	App_Encryption.Result.OSCnt = 0;
  	App_Encryption.Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  	App_Encryption.Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  	App_Encryption.Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
	App_Encryption.Invalidmsg = OSSemCreate(0);

}

//У���㷨
void App_Encryption_Verify(void)
{
	unsigned char string[18];
	unsigned short result = 0;
	unsigned short result2 = 0;

	string[0] = (unsigned char)(App_Encryption.Device_Serial0 >> 24);
	string[1] = (unsigned char)(App_Encryption.Device_Serial0 >> 16);
	string[2] = (unsigned char)(App_Encryption.Device_Serial0 >> 8);
	string[3] = (unsigned char)(App_Encryption.Device_Serial0 >> 0);

	string[4] = (unsigned char)(App_Encryption.Device_Serial1 >> 24);
	string[5] = (unsigned char)(App_Encryption.Device_Serial1 >> 16);
	string[6] = (unsigned char)(App_Encryption.Device_Serial1 >> 8);
	string[7] = (unsigned char)(App_Encryption.Device_Serial1 >> 0);

	string[8] = (unsigned char)(App_Encryption.Device_Serial2 >> 24);
	string[9] = (unsigned char)(App_Encryption.Device_Serial2 >> 16);
	string[10] = (unsigned char)(App_Encryption.Device_Serial2 >> 8);
	string[11] = (unsigned char)(App_Encryption.Device_Serial2 >> 0);

	string[12] = Encryption[0];
	string[13] = Encryption[1];
	string[14] = Encryption[2];
	string[15] = Encryption[3];
	string[16] = Encryption[4];
	string[17] = Encryption[5];

	result2 = (Encryption[6]<<8) | Encryption[7];
	result = CRC16(string,18);

	//����ȣ�����Ϣ
	if(result != result2)
	{
		OSSemPost(App_Encryption.Invalidmsg);
	}
}

//У���Ƿ��ǺϷ����ǷǷ�0 �Ϸ� 1�Ƿ�
char App_Encryption_Invalid(void)
{
	//��ѯ��Ϣ
	OSSemQuery(App_Encryption.Invalidmsg,&(App_Encryption.Result));
	if(App_Encryption.Result.OSCnt >= 1)
	{
		return 1;
	}
	else
	{
		return 0;
	} 
}


//�㱨cpuid����λ��
void  App_Encryption_ReportID(u8* chardata,int len)
{
	//֡��ȷ����ʶ
	char Flag = 0;

	//�������Ϊ0��������
	if(len >0 )
	{
		u8 SendData[30];

		//����Ӧ��
		//LRCУ�飬֡�Ƿ���ȷ
		if( chardata[len-1] == LRC(chardata,len-1) )
		{
			//��������ȷ����λ�����͵Ĺ�����
			if(chardata[0] == 0x71)
			{
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
			SendData[0] = 0x71;

			SendData[1] = (unsigned char)(App_Encryption.Device_Serial0 >> 24);
			SendData[2] = (unsigned char)(App_Encryption.Device_Serial0 >> 16);
			SendData[3] = (unsigned char)(App_Encryption.Device_Serial0 >> 8);
			SendData[4] = (unsigned char)(App_Encryption.Device_Serial0 >> 0);

			SendData[5] = (unsigned char)(App_Encryption.Device_Serial1 >> 24);
			SendData[6] = (unsigned char)(App_Encryption.Device_Serial1 >> 16);
			SendData[7] = (unsigned char)(App_Encryption.Device_Serial1 >> 8);
			SendData[8] = (unsigned char)(App_Encryption.Device_Serial1 >> 0);

			SendData[9] = (unsigned char)(App_Encryption.Device_Serial2 >> 24);
			SendData[10] = (unsigned char)(App_Encryption.Device_Serial2 >> 16);
			SendData[11] = (unsigned char)(App_Encryption.Device_Serial2 >> 8);
			SendData[12] = (unsigned char)(App_Encryption.Device_Serial2 >> 0);
			
			SendData[13] = LRC(SendData,13);
			//д����
			USBCOM_Write(SendData,14);
		}
		else
		{
			//modbus��֡�����ϰ�Ӧ��ʽ
			SendData[0] = 0x71|0x80;
			SendData[1] = LRC(SendData,1);
			//д����
			USBCOM_Write(SendData,2);
		}
	}
}

 //д���ܴ�
void  App_Encryption_WriteEncry(u8* chardata,int len)
{
	//֡��ȷ����ʶ
	char Flag = 0;

	//�������Ϊ0��������
	if(len >0 )
	{
		u8 SendData[20];

		//����Ӧ��
		//LRCУ�飬֡�Ƿ���ȷ
		if( chardata[len-1] == LRC(chardata,len-1) )
		{
			//��������ȷ����λ�����͵Ĺ�����
			if(chardata[0] == 0x72)
			{
				Encryption[0]=chardata[1];
				Encryption[1]=chardata[2];
				Encryption[2]=chardata[3];
				Encryption[3]=chardata[4];
				Encryption[4]=chardata[5];
				Encryption[5]=chardata[6];
				Encryption[6]=chardata[7];
				Encryption[7]=chardata[8];
				WriteEncry = 1;
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
			SendData[0] = 0x072;
			SendData[1] = LRC(SendData,1);
			//д����
			USBCOM_Write(SendData,2);
		}
		else
		{
			//modbus��֡�����ϰ�Ӧ��ʽ
			SendData[0] = 0x72|0x80;
			SendData[1] = LRC(SendData,1);
			//д����
			USBCOM_Write(SendData,2);
		}
	}
}
