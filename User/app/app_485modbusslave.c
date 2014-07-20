#include <ucos_ii.h>
#include "app_485modbusslave.h"
#include "bsp.h"
#include "app_485com.h"
#include "app_kernelregs.h"
#include "app_backend.h"
#include "app_usbcom.h"

//֪ͨmodbus slave�յ�����
void C485_ModbusSlave_ProcessFrame(u8* chardata,int len)
{
	//֡��ȷ����ʶ
	char Flag = 0;

	//�������Ϊ0��������
	if(len >5 )
	{
		u8 SendData[240];

		//����Ӧ��
		//CRCУ��
		u16 crcresult = CRC16(chardata,(u16)len -2);
		if (crcresult == (chardata[len-2]<<8 | chardata[len-1] ))
		{
			//��λ�����͵Ĺ�����ƥ��
			//дreg
			if(chardata[1] == 0x06)
			{
				//�������ݣ�д�Ĵ���
				//RecvData[]:0 id 1 cmd 2-3 �Ĵ�����ַ 4-5 �Ĵ�������
				{
					short sd = 0;
					sd = chardata[4];
					sd = sd << 8;//��8λ
					sd = sd + chardata[5];//��8λ

					//�޸ı�ʶλ
					if(sd != KernelRegs[chardata[3]] 
					&& ((chardata[3]) < 80) )
					{
					  //79Ϊ����λ�����迼�ǵ��籣��
					  if(((chardata[3]) != 79))
					  {
					    RegsFlag[chardata[3]] = 1;
					  }
					  KernelRegs[chardata[3]] = sd;
					}
				}
				//Ӧ��
	
			}
			//дregs
			else if(chardata[1] == 0x10)
			{
			    int i = 0;
	
				//�������ݣ�д�Ĵ���
				//RecvData[]:0 id 1 cmd 2-3 �Ĵ�����ַ 4-5 �Ĵ������� 6�����ֽ��� 7-��β ����
				for(i = 0;i < chardata[5];i++)
				{
					short sd = 0;
					sd = chardata[7+i*2];
					sd = sd << 8;//��8λ
					sd = sd + chardata[7+i*2 + 1];//��8λ

					//�޸ı�ʶλ
					if(sd != KernelRegs[chardata[3]+i] 
					&& ((chardata[3]+i) < 80) )
					{
					  //79Ϊ����λ�����迼�ǵ��籣��
					  if(((chardata[3]+i) != 79))
					  {
					    RegsFlag[chardata[3]+i] = 1;
					  }
					  KernelRegs[chardata[3]+i] = sd;
					}
				}
				//Ӧ��
	
			}
			//��
			else if(chardata[1] == 0x03)
			{
			  //do nothing
			}
			else
			{
	
			}
			Flag = 1;
		}


		//Ӧ��PLC
		if(Flag == 1)
		{
			//дӦ��
		    if(chardata[1] == 0x06)
			{
			  u16 crc = 0;
			  //modbus��֡����ȷ��Ӧ��ʽ
			  SendData[0] = MODBUSID; //id
			  SendData[1] = 0x06; //����
			  SendData[2] = chardata[2];//��ʼ��ַ
			  SendData[3] = chardata[3];//��ʼ��ַ
			  SendData[4] = chardata[4];//�Ĵ���ֵ
			  SendData[5] = chardata[5];//�Ĵ���ֵ

			  crc = CRC16(SendData,6);
			  SendData[6] = crc>>8; //LRC
			  SendData[7] = crc; //LRC

			  //д����
			  C485COM_Write(SendData,8);

			}			//дӦ��
		    else if(chardata[1] == 0x10)
			{
			  u16 crc = 0;
			  //modbus��֡����ȷ��Ӧ��ʽ
			  SendData[0] = MODBUSID; //id
			  SendData[1] = 0x10; //����
			  SendData[2] = chardata[2];//��ʼ��ַ
			  SendData[3] = chardata[3];//��ʼ��ַ
			  SendData[4] = chardata[4];//�Ĵ�������
			  SendData[5] = chardata[5];//�Ĵ�������

			  crc = CRC16(SendData,6);
			  SendData[6] = crc>>8; //LRC
			  SendData[7] = crc; //LRC

			  //д����
			  C485COM_Write(SendData,8);

			}//��Ӧ��
			else if(chardata[1] == 0x03)
			{
     		  int i,j;
 			  u16 crc = 0;

			  //modbus��֡����ȷ��Ӧ��ʽ
			  SendData[0] = MODBUSID; //id
			  SendData[1] = 0x03; //����
			  SendData[2] = chardata[5]*2;//Ӧ���ֽ���

	          //�Ĵ�������
	          for(i = 0,j = chardata[3]; i < chardata[5];i++)
	          {
		        //short�ĸ�8λ
		        SendData[3+i*2] = (char)((KernelRegs[i+j]>>8)&0xff);

		        //short�ĵ�8λ
		        SendData[3+i*2+1] = (char)((KernelRegs[i+j])&0xff);
	          }

	          //CRCλ
 			  crc = CRC16(SendData,chardata[5]*2+3);
			  SendData[chardata[5]*2+3] = crc>>8; //LRC
			  SendData[chardata[5]*2+4] = crc; //LRC

			  //д����
			  C485COM_Write(SendData,chardata[5]*2 + 5);
			}
		}
		else
		{
		  	//u8 msg[20] = "LRCERR\n";
		  	//BSP_USBWrite(msg,7);
			//modbus��֡�����ϰ�Ӧ��ʽ
 			u16 crc = 0;
			SendData[0] = MODBUSID;//id
			SendData[1] = chardata[1] | 0x80; //������
			SendData[2] = 0x01;//����

          	//CRCλ
		  	crc = CRC16(SendData,3);
		  	SendData[3] = crc>>8; //LRC
		  	SendData[4] = crc; //LRC

			//д����
			C485COM_Write(SendData,5);
		}
	}
}




