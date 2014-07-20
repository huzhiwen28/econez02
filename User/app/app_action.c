
/*++++++++++++++++++++++++++++++++++++++
Action
��ѭ��
����
IO�˿ڣ�ģ�����˿ڣ�ͨ�Ų���   ��ͣ���㶯�����ᡢ�˲��ȹ���
 +++++++++++++++++++++++++++++++++++++++*/
#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "app_action.h"
#include "app_inout.h"
#include "bsp_dac.h"
#include "bsp_adc.h"
#include "app_kernelregs.h"
#include "bsp_led.h"
#include "bsp_enc.h"
#include "app_Port.h"


//��ʼ����
void StartAction()
{
	if(Param.Basic.Out==1)			//Ť��ģʽ
	{
		State.Action.Start=0;
		State.Action.StartFlg=1;					//������ʱ֮��תΪPID�Զ����
		State.Action.Stop=0;						//�����ͣ����ֹͣͣ��
		State.Action.StopFlg=0;
		State.Basic.Run=1;
		RunLedOn
		if(State.Basic.OutCh==0)			
		{
			State.Out.Mode=3;							//�Զ�����Ԥ���ģʽ
		}
		else
		{
			State.Out.Mode2=3;	
		}
		InitPIDOut();				//PID��ʼ����ʹPID�����I���(���֣������ڵ�ǰ��������ٶ�ģʽ��û�У�Ť���±����
		State.Basic.DisMode=0;		//ˢ�½���
	}
	else if(Param.Basic.Out==2)				//�ٶ�ģʽ
	{
		State.Basic.Run=2;			//��׼PID����
		RunLedOn

		if(State.Basic.OutCh==0)	//A��		
		{
			State.Out.Mode=1;     //PID�Զ��������ģʽ
		}
		else						//B��
		{
		 	State.Out.Mode2=1;     //PID�Զ��������ģʽ
		}
		PID_StateInit();   //�ٶ�ģʽ��������ٶ������ٶ�֮��ı�ֵ����ʼ��
	}
	KernelRegs[66]=1;	//�ϵ����
	RegsFlag[66] =1;  	//	EEPROM SAVE
}											
	
//ֹͣ����
void StopAction()
{
	if(Param.Basic.Out==1)			//Ť��ģʽ
	{
		State.Action.Stop=0;
		State.Basic.DisMode=0;		//ˢ�½���
		if(State.Basic.Run==1)		//����ʱ
		{
			State.Action.Start=0;
			State.Action.StartFlg=0;					//
			State.Basic.Run=0;
			RunLedOff
			if(State.Basic.OutCh==0)			//A�����ģʽ
			{
				State.Out.Mode2=0;
				if(State.Basic.OutSave==0)
				{
					State.Out.Mode=8;		//����Ԥ���
				}
				else
				{
					State.Out.Mode=9;		//��������Ԥ���
				}
			}
			else 	if(State.Basic.OutCh==1) 	//b�����ģʽ
			{
				if(State.Basic.OutSave==0)
				{
					State.Out.Mode2=8;		//����Ԥ���
				}
				else
				{
					State.Out.Mode2=9;		//��������Ԥ���	
				}
				State.Out.Mode=0;
			}
		}
		else if((State.Basic.Run==2)||(State.Basic.Run==4)||(State.Basic.Run==5))			//PID����ʱ,���ٶ�PID ���ٶ�PID
		{
			State.Action.StopFlg=1;
			State.Basic.PidInit=0;
			if(State.Basic.OutSave==1)			//�������
			{
				if(State.Basic.OutCh==0)			
				{
					Param.Basic.SaveOut=State.Out.Data;		//�Զ��������
				}
				else
				{
					Param.Basic.SaveOut=State.Out.Data2;		//�Զ��������
				}
				//huzhiwen comment SaveSaveOut();
			}
			State.Basic.Run=3;		//ͣ��PID
		}
	}
	else if(Param.Basic.Out==2)			//�ٶ�ģʽ
	{
		State.Action.StopFlg=1;
		State.Basic.Run=0;			//��ͣ��  ����״̬
		RunLedOff
		State.Out.Mode=0;     //A�᲻���
		State.Out.Mode2=0;     //B�᲻���
//		State.Basic.DisMode=0;		//ˢ�½���
		if(Param.Basic.AutoClearDia==1)	  //�Զ����
		{
		 	DaiClear();	  //��������0
		}
		else if(Param.Basic.AutoClearDia==2)	  //  �Զ������
		{
			DiaSave();
		}
	}
 	KernelRegs[66]=0;	//�ϵ������0
	RegsFlag[66] =1;  	//	EEPROM SAVE
}
//�㶯����
void JogAction()
{
	if(State.Basic.Run==0)				//�����ڴ���ʱ��Ч
	{
		if(State.Basic.OutCh==0)			//ͨ��1	
		{
				State.Out.Mode=5;					//�㶯���ģʽ
				State.Basic.Jog=1;
		}
		else							 //ͨ��2
		{

				State.Out.Mode2=5;				//�㶯���ģʽ
				State.Basic.Jog=1;
		}
		State.Basic.DisMode=0;		//ˢ�½���
		JOGLedOn
	}
}

void JogStop()	  //�㶯ֹͣ
{
	if(State.Basic.Run==0)				//�����ڴ���ʱ��Ч
	{
			if(State.Basic.OutCh==0)	//A��	
			{
				if(State.Out.Mode==5)
				{
					if(Param.Basic.Out==1)			//Ť��ģʽ
					{
						if(State.Basic.OutSave==1)			//�������
						{
							State.Out.Mode=9;				//�������
						}
						else
						{
							State.Out.Mode=8;				//����Ԥ���ģʽ
						}
					}
					else 		//�ٶ�ģʽ
					{
						 State.Out.Mode=0;     //A�᲻���
					}
		//			State.Basic.DisMode=0;		//ˢ�½���
				}
			}
			else
			{
				if(State.Out.Mode2==5)
				{
					if(Param.Basic.Out==1)			//Ť��ģʽ
				 	{
						if(State.Basic.OutSave==1)			//�������
						{
							State.Out.Mode2=9;				//�������
						}
						else
						{
							State.Out.Mode2=8;				//����Ԥ���ģʽ
						}
					}
					else  	//�ٶ�ģʽ
					{
						 State.Out.Mode2=0;     //B�᲻���
					}
				}
			}
			JOGLedOff
	}
}

void ChangAction(uint S)			//�����ź�
{
	if(S==1)
	{
		if(State.Basic.OutCh==0)			//�������ģʽA��B
		{
			State.Work.RollDT=0;		//����ת����0
			State.Work.MainDT=0;		//����ת����0
			State.Work.DiaPer=0;		//������0
			State.Work.Dia=0;
			if(Param.Dai.Way==4)		//��ֵ��2   ģ������ֵ�� 
			{
		   		if(Param.Basic.MachMode==1)			//�ž�
				{
				   	State.Work.Dia=Param.Dai.MostDia;
				}
				else  if(Param.Basic.MachMode==2)			//�վ�
				{
				 	State.Work.Dia=Param.Dai.LeastDia;
				}
			}

			(* BSP_ENC_CNT)=0;
			State.Basic.OutCh=1;   		//B�����  
			State.Basic.DisMode=0;		//ˢ�½���
			if((State.Basic.Run!=0)&&(State.Basic.Run!=3))		//���������״̬
			{
				State.Action.BChange=0;	
				State.Action.BChangeFlg=1;		//B�������	 /A TO B   B����
				State.Action.AChangeStop=0;
				State.Action.AChangeStopFlg=1;	//A�����ƶ����	   A TO B  A�ƶ�
				State.Out.Mode=7;				//A�����ƶ����ģʽ	
				State.Out.Mode2=6;				//B�������ģʽ	
			}
			else
			{
				State.Out.Mode2=State.Out.Mode;
				State.Action.AChangeStopFlg=1;	//A�����ƶ����
				State.Action.AChangeStop=0;
				State.Out.Mode=7;

			}	
			if(State.Action.BChangeStopFlg==1)		//�ջ�ΪA��,B���ƶ������
			{
				State.Action.BChangeStopFlg=0;
				State.Action.BChangeStop=0;
			}
		}
	}
	else 
	{
		if(State.Basic.OutCh==1)			//�������ģʽB��A
		{
			State.Work.RollDT2=0;		//����ת����0
			State.Work.MainDT=0;		//����ת����0
			State.Work.DiaPer2=0;
			State.Work.Dia2=0;
			if(Param.Dai.Way==4)		//��ֵ��2   ģ������ֵ�� 
			{
		   		if(Param.Basic.MachMode==1)			//�ž�
				{
				   	State.Work.Dia2=Param.Dai.MostDia;
				}
				else  if(Param.Basic.MachMode==2)			//�վ�
				{
				 	State.Work.Dia2=Param.Dai.LeastDia;
				}
			}
			(* BSP_ENC_CNT)=0;
			State.Basic.OutCh=0;
			State.Basic.DisMode=0;		//ˢ�½���
			if((State.Basic.Run!=0)&&(State.Basic.Run!=3))		//���������״̬
			{
				State.Action.AChange=0;	
				State.Action.AChangeFlg=1;		//A�������
				State.Action.BChangeStop=0;
				State.Action.BChangeStopFlg=1;	//B�����ƶ����
				State.Out.Mode=6;				//A�������ģʽ	
				State.Out.Mode2=7;				//B�����ƶ����ģʽ		
			}
			else
			{
				State.Out.Mode=State.Out.Mode2;
				State.Action.BChangeStopFlg=1;	//B�����ƶ����
				State.Out.Mode2=7;
			}

			if(State.Action.AChangeStopFlg==1)		//�ջ�ΪB��,A���ƶ������
			{
				State.Action.AChangeStopFlg=0;
				State.Action.AChangeStop=0;
			}			
		}
	}
}


void IOJog(uint S)
{
	if(S==1)
	{
		JogAction();
	}
	else 
	{
		State.Basic.Jog=0;
		JogStop();		
	}
}
//A������ٶ�
void RollASpeed()
{
	if(State.Basic.OutCh==0)			//A�����ģʽ
	{
		if(Param.Dai.RollAWay==1)	   // A������������IO��
		{
			State.Work.RollDT++;		//����ת��,����2������
		}
		if(Param.Dai.Way==3)		//��ֵ��1
		{
			State.Work.RollDSpeed=State.Work.RollDS*Param.Dai.RollD;		//תһȦ��ʱ��
			State.Work.RollDS=0;
		}
	}
}

void RollBSpeed()	  //B������ٶ�
{
	if(State.Basic.OutCh==1)			//B�����ģʽ
	{
		if(Param.Dai.RollBWay==1)	   // B������������IO��
		{
			State.Work.RollDT2++;		//����ת��
		}
		if(Param.Dai.Way==3)		//��ֵ��1
		{
			State.Work.RollDSpeed=State.Work.RollDS2*Param.Dai.RollD;
			State.Work.RollDS2=0;
		}
	}
}

void MainSpeed() //��ǣ���ٶ�
{
	State.Work.MainDT++;		//����ת��
	if(Param.Dai.Way==3)		//��ֵ��1
	{
		State.Work.MainDSpeed=State.Work.RollDS*Param.Dai.MainD;
		State.Work.MainDS=0;
	}
}

void RunSwitch( uint RunS)	 //�����л�
{
	if(RunS==1)			//������ź�
	{
		if((State.Basic.Run==0)||(State.Basic.Run==3))	//����ڴ�����ͣ��״̬
		{							//����
			StartAction();
		}
	}
	else if(RunS==0)			//������ź�
	{
		if(State.Basic.Run!=0)		//�����ֹͣ״̬
		{							//����
			StopAction();
		}
	}
}

void  MainMotFR(uint RunS)
{
	if(RunS==0)			//������ź�  	//��ת
	{
		if(State.Basic.Run==0)				//�����ڴ���ʱ��Ч
		{						
			Param.Basic.MMFR=0;				//0����ת��1��ת
		}
	}
	else if(RunS==1)			//������ź�
	{
		if(State.Basic.Run==0)				//�����ڴ���ʱ��Ч
		{
 			Param.Basic.MMFR=1;				//0����ת��1��ת
		}
	}
}

void  DaiClear()	  //��������0
{
	State.Work.Dia=0;
	State.Work.DiaPer=0;
	State.Work.RollDT=0;
	(* BSP_ENC_CNT)=0;
	State.Work.RollDT2=0;		//����ת����0
	State.Work.MainDT=0;		//����ת����0
	State.Work.DiaPer2=0;
	State.Work.Dia2=0;
	if(Param.Dai.Way==4)		//��ֵ��2   ģ������ֵ�� 
	{
		 if(Param.Basic.MachMode==1)			//�ž�
		{
			State.Work.Dia2=Param.Dai.MostDia;
			State.Work.Dia=Param.Dai.MostDia;
		}
		else  if(Param.Basic.MachMode==2)			//�վ�
		{
			State.Work.Dia2=Param.Dai.LeastDia;
			State.Work.Dia=Param.Dai.LeastDia;
		}
	}
	KernelRegs[65]=0;  //
	RegsFlag[65] =1;
}

void DiaSave()
{
	u16 DiaDataSave;
//	if(State.Basic.OutCh==0)  //A��
	DiaDataSave=1+State.Work.DiaPer/5;	   //��	DiaPer��0��1000ת����0~200,�Ա��ܹ���8λ(0~256������

 //   else  if(State.Basic.OutCh==0)  //B��
	DiaDataSave=DiaDataSave+(((State.Work.DiaPer2/5)+1)<<8);
	KernelRegs[65]=DiaDataSave;
	RegsFlag[65] =1;	 //��EEPROM��д
}

void IOInput()			//IO����
{
uint8_t PS1,PS2,PS3,PS4,PS5,PS6,PS7,PS8,PS9;

		PS1=(1-GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5));
		PS2=(1-GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6));
		PS3=(1-GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7));
		PS4=(1-GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_8));
		PS5=(1-GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9));
		PS6=(1-GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12));
		PS7=(1-GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));	 //���������1
		PS8=(1-GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7));	  //���������3
		PS9=(1-GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5));	  //���������3
		Port.DIn.Bits.S7=PS7;
		Port.DIn.Bits.S8=PS8;
		Port.DIn.Bits.S9=PS9;

		if(Port.DIn.Bits.S1!=PS1)		  //�����ź�
		{
			Port.DIn.Bits.S1=PS1;			//���ź�
			RunSwitch(Port.DIn.Bits.S1);
		}

		if(Port.DIn.Bits.S2!=PS2)		 	//	�㶯����
		{
			Port.DIn.Bits.S2=PS2;			//���ź�
			IOJog(Port.DIn.Bits.S2);
		}
		if(Port.DIn.Bits.S3!=PS3)		 	//	����ת����
		{
			Port.DIn.Bits.S3=PS3;			//���ź�
			MainMotFR(Port.DIn.Bits.S3);		   	//������������ת����
		}
		if(Port.DIn.Bits.S4!=PS4)		  	//	���ѡ��0:A��,1:B��
		{
			Port.DIn.Bits.S4=PS4;			//���ź�
			ChangAction(Port.DIn.Bits.S4);
		}

		if(Port.DIn.Bits.S5!=PS5)	 //������������
		{
			Port.DIn.Bits.S5=PS5;			//���ź�
			if(Param.Dai.RollMWay==1)	   // M������������IO��5
			{
				if(Param.Dai.Way==3)		//��ֵ��1
				{
					if(Port.DIn.Bits.S5==1)		//�����ٶ�
					{
				//		MainSpeed();
						State.Work.MainDT++;		//����������			
					}
				}
			}
			else if(Param.Dai.RollBWay==1)	   // B������������IO��5
			{
				if(State.Basic.OutCh==1)			//B�����ģʽ
				{
					if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
					{

						if(Port.DIn.Bits.S5==1)		//������+1
						{
							State.Work.RollDT2=State.Work.RollDT2+1;
						}
					}
					if(Param.Dai.Way==3)		//��ֵ��1
					{
						if(Port.DIn.Bits.S5==1)		//�����ٶ�
						{
						//	RollBSpeed();
							State.Work.RollDT2=State.Work.RollDT2+1;
						}			
					}
				}
			}		
		}
		if(Port.DIn.Bits.S6!=PS6)  //������������
		{
			Port.DIn.Bits.S6=PS6;			//���ź�

			if(Param.Dai.RollAWay==1)	   // A������������IO��6
			{
				if(State.Basic.OutCh==0)			//A�����ģʽ
				{
					if(Param.Dai.Way==3)		//��ֵ��1
					{
						if(Port.DIn.Bits.S6==1)		//�����ٶ�
						{
						//	RollASpeed();
							State.Work.RollDT=State.Work.RollDT+1;
						}			
					}
					if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
					{
						if(Port.DIn.Bits.S6==1)		//����Ȧ����1
						{
							State.Work.RollDT=State.Work.RollDT+1;
						}
					}
				}
			}		
		}
}



