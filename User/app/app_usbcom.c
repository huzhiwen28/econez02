#include <ucos_ii.h>
#include "app_usbcom.h"
#include "bsp_usb.h"
#include "app_usbmodbusmaster.h"
#include "app_usbmodbusslave.h"
#include "app_usbgather.h"



#define ControlVersion  2
struct _USBCOM_Backend 	USBCOM_Backend;
u16 HaveheadtimeMS = 0;

//�㱨��������Ϣ����λ��
void  App_ReportControlInfo(u8* chardata,int len)
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
			if(chardata[0] == 0x73)
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
			SendData[0] = 0x73;

			//����������Ϣ����ProductID,ComID,CPUUsage,ControlVersion

			//ProductID
			//SendData[1] = (unsigned char)(App_Encryption.Device_Serial2 >> 24);
			//SendData[2] = (unsigned char)(App_Encryption.Device_Serial2 >> 16);
			//SendData[3] = (unsigned char)(App_Encryption.Device_Serial2 >> 8);
			//SendData[4] = (unsigned char)(App_Encryption.Device_Serial2 >> 0);

			//ComID
			SendData[5] = (unsigned char)(MODBUSID >> 8);
			SendData[6] = (unsigned char)(MODBUSID >> 0);
			
			//CPUUsage%
			SendData[7] = 0;
			SendData[8] = OSCPUUsage;

			//ControlVersion
			SendData[9] = (unsigned char)(ControlVersion >> 8);
			SendData[10] = (unsigned char)(ControlVersion >> 0);
			
			SendData[11] = LRC(SendData,11);

			//д����
			USBCOM_Write(SendData,12);
		}
		else
		{
			//modbus��֡�����ϰ�Ӧ��ʽ
			SendData[0] = 0x73|0x80;
			SendData[1] = LRC(SendData,1);
			//д����
			USBCOM_Write(SendData,2);
		}
	}
}

void printint(int num)
{
	  u8 datastring[10]="";
	  u8 zerodata = '0';
	  u8 chardata = 0;
	  u8 tendata = 0;
	  int mynum = num;

		tendata = (u8)(mynum/1000000);
		chardata = tendata+zerodata;
		datastring[0] = chardata;
		mynum = mynum%1000000;

		tendata = (u8)(mynum/100000);
		chardata = tendata+zerodata;
		datastring[1] = chardata;
		mynum = mynum%100000;

		tendata = (u8)(mynum/10000);
		chardata = tendata+zerodata;
		datastring[2] = chardata;
		mynum = mynum%10000;

		tendata = (u8)(mynum/1000);
		chardata = tendata+zerodata;
		datastring[3] = chardata;
		mynum = mynum%1000;

		tendata = (u8)(mynum/100);
		chardata = tendata+zerodata;
		datastring[4] = chardata;
		mynum = mynum%100;

		tendata = (u8)(mynum/10);
		chardata = tendata+zerodata;
		datastring[5] = chardata;
		mynum = mynum%10;

		chardata = mynum+zerodata;
		datastring[6] = chardata;
		
		datastring[7] = ' ';
		//BSP_232CWrite(datastring,8);
		BSP_USBWrite(datastring,8);

}
/*
ascii��ת��Ϊchar������"43" -> 0x43
charbuff ��Ž��
ascii ��ת����ascii
len ��ת���ĳ��ȣ�Ϊż��
*/
void ascii2char(u8* charbuff,u8* asciibuff,int len)
{
	int j,jj;//j ��Ӧascii jj��Ӧchar����

	//ascii -> char
	for (j = 0,jj = 0; j < len-1; j += 2,++jj)
	{
		//��λascii
		//��ĸ
		if (asciibuff[j] <= 70 && asciibuff[j] >= 65)
		{
			charbuff[jj] = ((asciibuff[j] - 55) << 4) & 0xf0;
		}//����
		else if (asciibuff[j] <= 57 && asciibuff[j] >= 48)
		{
			charbuff[jj] = ((asciibuff[j] - 48) << 4)& 0xf0;
		}

		//��λascii
		//��ĸ
		if (asciibuff[j+1] <= 70 && asciibuff[j+1] >= 65)
		{
			charbuff[jj] += (asciibuff[j+1] - 55) & 0x0f;
		}//����
		else if (asciibuff[j+1] <= 57 && asciibuff[j+1] >= 48)
		{
			charbuff[jj] += (asciibuff[j+1] - 48) & 0x0f;
		}
	}
}

/*
charת��Ϊascii�룬����0x43 -> "43"
ascii ת���Ľ��ascii
charbuff ��ת��
len ��ת���ĳ���
*/
void chartoascii(u8* asciibuff,u8* charbuff,int len)
{
	int j,jj;//j ��Ӧchar, jj��Ӧascii����

	//����
	for (j = 0,jj = 0; j< len; ++j,jj += 2)
	{
	   //��λ 
	   if(((charbuff[j]>>4) >= 0) && ((charbuff[j]>>4) <= 9))
	   {
	     *(asciibuff+jj) = (charbuff[j]>>4) + 48;
	   }
	   else if(((charbuff[j]>>4) >= 0x0a) && ((charbuff[j]>>4) <= 0x0f))
	   {
	     *(asciibuff+jj) = (charbuff[j]>>4) + 55;
	   }

	   //��λ
	   if(((charbuff[j] & 0x0f) >= 0) && ((charbuff[j]& 0x0f) <= 9))
	   {
	     *(asciibuff+jj+1) = (charbuff[j] & 0x0f) + 48;
	   }
	   else if(((charbuff[j]& 0x0f) >= 0x0a) && ((charbuff[j]& 0x0f) <= 0x0f))
	   {
	     *(asciibuff+jj+1) = (charbuff[j] & 0x0f) + 55;
	   }
	}
	return;
}

/*
LRCУ��ֵ����
charbuff �������char����
len �������char���鳤��
����LRC������
*/

char LRC(u8 *charbuff,int len)
{
	char retval = 0;
	int i = 0;
	for(i =0;i < len;i++)
	{
		retval += charbuff[i];
	}
	return retval;
}

/*
com��ʼ��
*/
void USBCOM_Init(void)
{
	//״̬��ʼ��
	USBCOM_Backend.framestate = 0;
	USBCOM_Backend.buffend = 0;
	USBCOM_Backend.frameheadindex = -1;
	USBCOM_Backend.Newmsg = OSSemCreate(0);
}

//���豸
void USBCOM_Open(void)
{
    USBCOM_Backend.comopen = 1;
}

//�ر��豸
void USBCOM_Close(void)
{
	if (USBCOM_Backend.comopen == 1)
	{
		USBCOM_Backend.comopen = 0;
	}
}

//����ִ��
void USBCOM_Run(void *p_arg)
{


    USBCOM_Init();
	USBCOM_Open();
    while(1)
	{
		u8 err;

		//�Ѿ���ͷ������ʱ��ȴ������������ڵȴ�
		if(USBCOM_Backend.framestate == 1)
		{
			//�ȴ�֪ͨ,100ms��ʱ
   			OSSemPend(USBCOM_Backend.Newmsg,OS_TICKS_PER_SEC/10,&err);
		}
		else
		{
			//�ȴ�֪ͨ�޳�ʱ
   			OSSemPend(USBCOM_Backend.Newmsg,0,&err);
		}
		
		//ֻҪ���ڴ򿪣����̨����
		if(USBCOM_Backend.comopen)
		{
			//��ɨ����ʼ
			int scanbegin = 0;
	
			//��ɨ������ֽں�һ��λ��
			int scanend = 0;
	
			int cnt = 0;
			int index = 0;
	
			//���֡ͷ��ʱ��Ҫ����
			if(USBCOM_Backend.framestate == 1)
			{
			   if(err == OS_TIMEOUT)
			   {
			     USBCOM_Backend.framestate = 0;
		        //���һ���ֽں�λ���޸�
		        USBCOM_Backend.buffend = 0;
	
		        //֡ͷΪ-1
		        USBCOM_Backend.frameheadindex = -1;
			   }
			}
	
			if((cnt = BSP_USBInBuffLen()) > 0 )
			{
			    
				scanbegin = USBCOM_Backend.buffend;
	
				//���ݹ�����ֻȡһ��1024������(��ԭ��������)
				if(cnt+USBCOM_Backend.buffend > 1024)
				{
					//ʵ�ʶ������ݳ��ȣ��������
					BSP_USBRead(USBCOM_Backend.buff + scanbegin,(1024-USBCOM_Backend.buffend));
					scanend = 1024;
				}
				else
				{
					//û�������ȫ����
					BSP_USBRead(USBCOM_Backend.buff + scanbegin,cnt);
					scanend = cnt + USBCOM_Backend.buffend;
				}
	
				//�����ֽں��λ�øı�
				USBCOM_Backend.buffend = scanend;
	
				//�����յ�������
				for(index = scanbegin;index < scanend;index++)
				{
					//û��ͷ
					if(USBCOM_Backend.framestate == 0)
					{
						//ͷ
						if(USBCOM_Backend.buff[index] == ':')
						{
							USBCOM_Backend.framestate = 1;
							USBCOM_Backend.frameheadindex = index;
						}
					}
	
					//��ͷ
					else if(USBCOM_Backend.framestate == 1)
					{
						//��⵽�����ַ�
						if(USBCOM_Backend.buff[index] == '\n')
						{
							//���ת�����
							u8 chardata[512];
	
							//ת���������
							int charlen = 0;
	 
							//asciiת��Ϊchar
							ascii2char(chardata,USBCOM_Backend.buff + USBCOM_Backend.frameheadindex+1 ,index - USBCOM_Backend.frameheadindex-1);
							charlen = (index - USBCOM_Backend.frameheadindex-1)/2;
	
							//��ӡ�յ����ַ�
							USBCOM_Backend.buff[index] = 0;
							//printf("�յ����ַ���%s",buff + frameheadindex+1);
	
							//���ݹ����뷢�͸���ͬ��������
							switch(chardata[0])
							{
								//PCд����������
							case 0x01:
								USB_ModbusSlave_ProcessFrame(chardata,charlen);
								break;
	
								//������дPC����
							case 0x02:
							case 0x82:
								USB_ModbusMaster_ProcessFrame(chardata,charlen);
								break;

								//��CPUID
							case 0x71:
								//App_Encryption_ReportID(chardata,charlen);
								break;

								//д���ܴ�
							case 0x72:
								//App_Encryption_WriteEncry(chardata,charlen);
								break;

								//����������Ϣ����ProductID,ComID,CPUUsage,ControlVersion
							case 0x73:
								App_ReportControlInfo(chardata,charlen);
								break;

	
							default:
								;
							}
							
							//�������״̬�ı�
							USBCOM_Backend.framestate = 0;
						}
						else
						{
							//����ټ�⵽ͷ
							if(USBCOM_Backend.buff[index] == ':')
							{
								//����ԭ����ͷ
								USBCOM_Backend.frameheadindex = index;
							}
						}
					}
				}
	
				//������ϲ��ҵ�ͷ״̬���棬����Ҫ��λ��ʹ�������ֽڶ��ǲ���������һ��֡������
				if(USBCOM_Backend.framestate == 1)
				{
				    int i = 0;
					//��Ч������ǰ�ƶ�
					for(i = 0;i < scanend - USBCOM_Backend.frameheadindex;i++ )
					{
						*(USBCOM_Backend.buff+i) = *(USBCOM_Backend.buff + USBCOM_Backend.frameheadindex+i);
					}
	
					//���һ���ֽں�λ���޸�
					USBCOM_Backend.buffend = scanend - USBCOM_Backend.frameheadindex;
	
					//֡ͷΪ0
					USBCOM_Backend.frameheadindex = 0;
				}
				else//������ϣ����Ҵ���û���ҵ�ͷ״̬���棬��Ϊ�������ݶ��������
				{
					//���һ���ֽں�λ���޸�
					USBCOM_Backend.buffend = 0;
	
					//֡ͷΪ-1
					USBCOM_Backend.frameheadindex = -1;
	
				}
				//printint(USBCOM_Backend.buffend);
			}
		}
	}
}


//д�ַ�
int USBCOM_Write(u8* chardata,int len)
{
	if(USBCOM_Backend.comopen)
	{
		u8 asciibuff[1024];

		//֡ͷ
		asciibuff[0]= ':';

		//֡����ת��Ϊascii
		chartoascii(asciibuff+1,chardata,len);

		//֡β
		asciibuff[len*2+1]= '\n';

		//���ڴ�ӡ
		asciibuff[len*2+2]= 0;

		BSP_USBWrite(asciibuff, len*2+2);
	}
	return len;
}

//֪ͨ����������Ϣ����
void USBCOM_Newmsg()
{
	if(USBCOM_Backend.comopen)
	{
	    OSSemPost(USBCOM_Backend.Newmsg);		//����֪ͨ
	}
}

