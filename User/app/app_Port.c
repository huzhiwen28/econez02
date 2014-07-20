
/*++++++++++++++++++++++++++++++++++++++

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


#define  Ain1MSpeedZero     0      
				//����ģ����2����ڵ�0V���룬

//static u8 err;
void APortInput()				//ģ���������
{
	//����
	if(Port.AIn.Ch1>=Param.Tens.Zero)
	{
		State.Work.InData=Port.AIn.Ch1-Param.Tens.Zero;
	}
	else
	{
	 	State.Work.InData=0;
	}
	State.Work.Tens=State.Work.InData*5;		  //����ֵ
	//�����ٶ�
	if(Param.Basic.MainSpeedWay==1)		//
	{
		if(Port.AIn.Ch2>= Ain1MSpeedZero)	 //�����м��ѹ������Ϊ��
		{
			if(Port.AIn.Ch2>=Param.Basic.MainSpeedZero)
			{
				State.Work.MSpeed = (Port.AIn.Ch2-Param.Basic.MainSpeedZero)*5;			 //���ٶȰٷֱ�

			}
			else
			{
				State.Work.MSpeed=0;
			}
		}
		else	 //����Ϊ��
		{
			if(Port.AIn.Ch2<=Param.Basic.MainSpeedZero)
			{
				State.Work.MSpeed =( Param.Basic.MainSpeedZero-Port.AIn.Ch2)*5;			 //���ٶȰٷֱ�

			}
			else
			{
				State.Work.MSpeed=0;
			}

		}
	}
	if(Param.Basic.SetWay==1)		//�����ź�������ģ����4

	{
		Param.Auto.SetAim=Port.AIn.Ch4*5;
	}
	if(Param.Dai.Way==1)			//���ź�������ģ����3
	{
  	  	State.Work.DiaPer =Port.AIn.Ch3/2;		  //   ���ٷֱ�	 0.1%��λ,  1000= 100%��1 =0.1%
	}

}


void APortOutput()				//ģ���������
{
	u32 APortOutputLong;
	if((Param.AOutPort.CH1OutMode==0)||(Param.AOutPort.CH1OutMode==1)||(Param.AOutPort.CH1OutMode==4)||(Param.AOutPort.CH1OutMode==5))
	{ 
		if(Param.AOutPort.CH1OutMode==0)    //0~5V
		{   
			if((State.Out.Data<=5000)&&(State.Out.Data>=0))		//5V����
			{
				APortOutputLong=State.Out.Data*4095;
    			Port.AOut.Ch1=APortOutputLong/5000;
			}
			else
			{
			   Port.AOut.Ch1=4095;
			}
		}
		else								////0~10V
		{
			if(State.Out.Data<=10000)		//10V����
			{
				APortOutputLong=State.Out.Data*4095;
    			Port.AOut.Ch1=APortOutputLong/10000;
			}
			else
			{
			   Port.AOut.Ch1=4095;
			}
		}
	}
	else
	{
	 	if(Param.Basic.MMFR==0)		  //��ת
		{
			if(Param.AOutPort.CH1OutMode==3)		//-5~+5v
			{   
				if(State.Out.Data<=5000)		//5V����
				{
					APortOutputLong=State.Out.Data*2047;
		    		Port.AOut.Ch1=2048+APortOutputLong/5000;
				}
				else
				{
					Port.AOut.Ch1=2048+2047;
				}
			}
			else
			{
				if(State.Out.Data<=10000)		//5V����
				{
					APortOutputLong=State.Out.Data*2047;
		    		Port.AOut.Ch1=2048+APortOutputLong/10000;
				}
				else
				{
					Port.AOut.Ch1=2048+2047;
				}

			}
		}
		else
		{
			if(Param.AOutPort.CH1OutMode==3)		//-5~+5v
			{   
				if(State.Out.Data<=5000)		//5V����
				{
					APortOutputLong=State.Out.Data*2047;
		    		Port.AOut.Ch1=2048-APortOutputLong/5000;
				}
				else
				{
					Port.AOut.Ch1=2048-2047;
				}
			}
			else
			{
				if(State.Out.Data<=10000)		//5V����
				{
					APortOutputLong=State.Out.Data*2047;
		    		Port.AOut.Ch1=2048-APortOutputLong/10000;
				}
				else
				{
					Port.AOut.Ch1=2048-2047;
				}
			}
		}
	}
////*********************************ͨ��2****************************************************88
	if((Param.AOutPort.CH2OutMode==0)||(Param.AOutPort.CH2OutMode==1)||(Param.AOutPort.CH2OutMode==4)||(Param.AOutPort.CH2OutMode==5))
	{ 
		if(Param.AOutPort.CH2OutMode==0)    //0~5V
		{   
			if(State.Out.Data2<=5000)		//5V����
			{
				APortOutputLong=State.Out.Data2*4095;
    			Port.AOut.Ch2=APortOutputLong/5000;
			}
			else
			{
			   Port.AOut.Ch2=4095;
			}
		}
		else								////0~10V
		{
			if(State.Out.Data2<=10000)		//10V����
			{
				APortOutputLong=State.Out.Data2*4095;
    			Port.AOut.Ch2=APortOutputLong/10000;
			}
			else
			{
			   Port.AOut.Ch2=4095;
			}
		}
	}
	else
	{
	 	if(Param.Basic.MMFR==0)		  //��ת
		{
			if(Param.AOutPort.CH2OutMode==3)		//-5~+5v
			{   
				if(State.Out.Data2<=5000)		//5V����
				{
					APortOutputLong=State.Out.Data2*2047;
		    		Port.AOut.Ch2=2048+APortOutputLong/5000;
				}
				else
				{
					Port.AOut.Ch2=2048+2047;
				}
			}
			else
			{
				if(State.Out.Data2<=10000)		//10V����
				{
					APortOutputLong=State.Out.Data2*2047;
		    		Port.AOut.Ch2=2048+APortOutputLong/10000;
				}
				else
				{
					Port.AOut.Ch2=2048+2047;
				}

			}
		}
		else
		{
			if(Param.AOutPort.CH2OutMode==3)		//-5~+5v
			{   
				if(State.Out.Data2<=5000)		//5V����
				{
					APortOutputLong=State.Out.Data2*2047;
		    		Port.AOut.Ch2=2048-APortOutputLong/5000;
				}
				else
				{
					Port.AOut.Ch2=2048-2047;
				}
			}
			else
			{
				if(State.Out.Data2<=10000)		//10V����
				{
					APortOutputLong=State.Out.Data2*2047;
		    		Port.AOut.Ch2=2048-APortOutputLong/10000;
				}
				else
				{
					Port.AOut.Ch2=2048-2047;
				}
			}
		}
	}
}

uint  InputFilter(uint *InAddress)				//����˲�
{
uint Filter,FTiems;
uint InAll;
	InAll=0;
	Filter=Param.Basic.ShowFilter/10;
	Filter=Filter+1;
	if(Filter>10)
	{
		Filter=10;						//��1��10
	}
	for(FTiems=1;FTiems<=Filter;FTiems++)
	{
		InAll=InAll+(*InAddress);
		InAddress++;
	}
	InAll=InAll/Filter;
	return(InAll);
}

uint  OutFilter(u16 ChAB)				//����˲�
{
static u16 OldOutFilter1,OldOutFilter2;
uint Filter,FilterData;
u16 OldOut;
uint OutD,OutData;
uint OUTDAT[10];
unsigned long OutSum;
	if(ChAB==1)
	{
		for(OutD=0;OutD<10;OutD++)
		{
			OUTDAT[OutD]=OUTDATA[OutD];
	//		OutD++;
		}
		OldOut=OldOutFilter1;
	}
	else
	{
		for(OutD=0;OutD<10;OutD++)
		{
			OUTDAT[OutD]=OUTDATB[OutD];

		}
		OldOut=OldOutFilter2;
	}
	Filter=Param.Basic.OutFilter;			//��ʱ
		OutSum=((long)OUTDAT[9])*0x00000064+((long)OUTDAT[8])*0x0000004b +((long)OUTDAT[7])*0x00000041+((long)OUTDAT[6])*0x0000003c +((long)OUTDAT[5])*0x00000037+((long)OUTDAT[4])*0x00000032+((long)OUTDAT[3])*0x00000028+((long)OUTDAT[2])*0x0000001e +((long)OUTDAT[1])*15+OUTDAT[0]*10;	//30��  100/500��20��
		OutD=OutSum/500;

		FilterData=101-Filter;

	    if(OutD>=OldOut)
		{
	   		if((OutD-OldOut)>FilterData)
			{
			 	OutData=OldOut+FilterData;
			}
			else
			{
			  	OutData=OutD;
			}
		}
		else
		{
 	   		if((OldOut-OutD)>FilterData)
			{
				if(OldOut>FilterData)
				{
				 	OutData=OldOut-FilterData;
				}
				else
				{
				  	OutData=0;
				}
			}
			else
			{
			  	OutData=OutD;
			}
		}
		if(ChAB==1)
		{
			OldOutFilter1=OutData;
		}
		else
		{
			OldOutFilter2=OutData;		
		}	
		return(OutData);
}

//****************************************����У��********************************************
void ParamCheck()
{
	u16 ParamE=0;
	if(Param.Dai.Test==1)	 //����
	{
		if(Param.Dai.Way==0)   //�޷���
		{
			 ParamE=1;
			 State.Basic.ParamERRID=1;
		}
		if(Param.Dai.MostDia< Param.Dai.LeastDia)
		{
			 ParamE=1;
			 State.Basic.ParamERRID=2;	
		}
	}
	if(	Param.Basic.ZDLine==1)	  //׶��
	{
		if(Param.Dai.Test==0)	 //������
		{
			 ParamE=1;
			 State.Basic.ParamERRID=3;	 //��׶��һ���ü���
		}
		if((100<Param.ZDZ.ZDZ10)||(Param.ZDZ.ZDZ10<Param.ZDZ.ZDZ20)||(Param.ZDZ.ZDZ20<Param.ZDZ.ZDZ30)||(Param.ZDZ.ZDZ30<Param.ZDZ.ZDZ40)||(Param.ZDZ.ZDZ40<Param.ZDZ.ZDZ50))
		{
			 ParamE=1;
			 State.Basic.ParamERRID=4;	   //׶��ֵһ���Ǿ�ԽС���ٷֱ�Խ��
		}
		if((Param.ZDZ.ZDZ50<Param.ZDZ.ZDZ60)||(Param.ZDZ.ZDZ60<Param.ZDZ.ZDZ70)||(Param.ZDZ.ZDZ70<Param.ZDZ.ZDZ80)||(Param.ZDZ.ZDZ80<Param.ZDZ.ZDZ90)||(Param.ZDZ.ZDZ90<Param.ZDZ.ZDZ100))
		{
			  ParamE=1;
			 State.Basic.ParamERRID=4;	  //׶��ֵһ���Ǿ�ԽС���ٷֱ�Խ��
		}
	}
	if(ParamE==1)
	{
	  State.Basic.ParamERR=1;
	}
	else  if(ParamE==0)
	{
	  State.Basic.ParamERR=0;
	}  

}

void   App_Paramload(void)
{
	static u16  StartRead;       //���ֲ���ֻ������ʱ��һ��
	union  PLCCommandUnion	PLC_Command_Data;
	u16 LedOnN;				//ָʾ��

	union  PLCCommandUnion2	PLC_Command_Data2;
	u16 IDIO1,IDIO2,IDIO3,IDIO4;     //ID���õ�IO��״̬

	KernelRegs[64]=0;

   Param.Auto.OpenOut=KernelRegs[0];				//Ԥ���ֵ
   Param.Auto.StartOut=KernelRegs[1];				//��������
   Param.Auto.StartDelayTime=KernelRegs[2];			//������ʱ���
   Param.Auto.StopOut=KernelRegs[3];				//ͣ������
   Param.Auto.StopDelayTime=KernelRegs[4];			//ͣ����ʱʱ��
//5 �㶯���ֵ ��9�ط���ɾ
	Param.Auto.ChangeOut=KernelRegs[6];
	Param.Auto.ChangeTime=KernelRegs[7];
	Param.Auto.ChangeStopTime=KernelRegs[8];

   Param.Auto.JogOut=KernelRegs[9];					//�㶯���ֵ	

   Param.Basic.ZeroTest=KernelRegs[10]*10;	 			//Ƿ�������ֵ
   Param.Basic.ZeroTestTiems=KernelRegs[11];	 	//Ƿ�������ʱ��
   Param.Basic.OverTest=KernelRegs[12]*10;	 			//���������ֵ
   Param.Basic.OverTestTiems=KernelRegs[13];	 	//���������ʱ��

   Param.Basic.MaxMainSpeedPor=KernelRegs[14];		//������Сʱ��Ӧ���ٶȵ�������ʣ���������ٶȵ��������ٶȱ��ʣ�

 // =KernelRegs[15];     				//
   Param.Basic.Mode=KernelRegs[16];  				//1������PID ����2��ģ��PID ����3������������4������������ģ��PID ����
   Param.Basic.MachMode=KernelRegs[17];				 //��е����
   Param.Basic.MostOut=KernelRegs[18];				//������ֵ
   Param.Basic.LeastOut=KernelRegs[19];				//��С���ֵ
	Param.Basic.SetOut=KernelRegs[78];				//�����ֶ�ģʽ�µ����ֵ
   Param.Basic.ZDLine=KernelRegs[20];				//0:��׶��,1:��׶��
 	Param.Basic.OutFilter=KernelRegs[21];// ����˲������
   Param.Dai.Test=KernelRegs[22];				//0"����⣬1���
   	if(KernelRegs[23]==1)			////��������ź�1/2   2��Ť���źţ�1 �ٶ��ź�
   	{
  		 Param.Basic.Out=2;	 				//��������ź�1��2 1��Ť���źţ�2 �ٶ��ź�
	}
	else  if(KernelRegs[23]==2)
	{
   		 Param.Basic.Out=1;	 				//��������ź�1��2 1��Ť���źţ�2 �ٶ��ź�
	}

   Param.Basic.Chang=KernelRegs[24]; 				//1:���ᣬ2��˫��
   Param.Basic.KPid=KernelRegs[25]; 				//PID΢�����ʣ����տͻ���
   Pid.Pol=KernelRegs[26];	    					//PID����
   Pid.CycT=KernelRegs[27];	 //PID����}
   Pid.KP=KernelRegs[28];	 //PID  KP����
   Pid.KI=KernelRegs[29];	 //PID����
   Pid.KD=KernelRegs[30];	 //PID����
   Pid.KB= KernelRegs[31];	 //PID����������
   Pid.KS= KernelRegs[32];	 //PID��������
   Pid.Die= KernelRegs[33];	 //PID������Χ
   Pid.Over= KernelRegs[34];	 //PID����������Χ
   Pid.Change= KernelRegs[35];		//PID���������ʣ� 0~100% 40��

   Param.Dai.Way=KernelRegs[36];	//����ⷽ��
   Param.Dai.Ply=KernelRegs[37];	 //���Ϻ��
   Param.Dai.RollD=KernelRegs[38];	 //����������
   Param.Dai.MainD=KernelRegs[39];	 //����������
   Param.Dai.LeastDia=KernelRegs[40];	 //��С��
   Param.Dai.MostDia=KernelRegs[41];	 //����
   Param.Dai.RollMWay=KernelRegs[42];	 //���������ź�������	0����  1��IO��5   2����������
   Param.Dai.Ratio=KernelRegs[43];	 //	//����ֱ��/��������

    Pid.KPV=KernelRegs[46];	 //PID  KP����
    Pid.KPD=KernelRegs[47];	 //PID  KP����

    Param.Dai.RollAWay=  KernelRegs[48]; 	  //����A�����ź�������	0����  1��IO��6   2����������
    Param.Dai.RollBWay =KernelRegs[49]; 	//����b�����ź�������	0����  1��IO��5   2����������

   Param.Basic.MainSpeedWay=KernelRegs[50];  //���ٶ�������		  1������ģ�������2��������ͨ��3 �����ڱ�����4 ������IO ��6
   Param.Basic.SetWay=KernelRegs[51];  		//�����ź�������  1��ģ������2ͨ��

   Param.ZDZ.ZDZ10=KernelRegs[52];  		//10%��ʱ��  ׶��
   Param.ZDZ.ZDZ20=KernelRegs[53];  		//20%��ʱ��  ׶��
   Param.ZDZ.ZDZ30=KernelRegs[54];  		//30%��ʱ��  ׶��
   Param.ZDZ.ZDZ40=KernelRegs[55];  		//40%��ʱ��  ׶��
   Param.ZDZ.ZDZ50=KernelRegs[56];  		//50%��ʱ��  ׶��
   Param.ZDZ.ZDZ60=KernelRegs[57];  		//60%��ʱ��  ׶��
   Param.ZDZ.ZDZ70=KernelRegs[58];  		//70%��ʱ��  ׶��
   Param.ZDZ.ZDZ80=KernelRegs[59];  		//80%��ʱ��  ׶��
   Param.ZDZ.ZDZ90=KernelRegs[60];  		//90%��ʱ��  ׶��
   Param.ZDZ.ZDZ100=KernelRegs[61];  		//100%��ʱ��  ׶

   Param.Basic.MSPAF=KernelRegs[62];		  //�ٶ�ǰ��
   Param.Basic.AutoClearDia=KernelRegs[68];	 //��RUN�ź���1��0ʱ���Զ��Ѿ�ֵ���

    if(StartRead==0)						//����4������ֻ������ʱ��һ��
   {
   		Param.AOutPort.CH1OutMode=KernelRegs[44];	 //ģ���������1ģʽ
   		Param.AOutPort.CH2OutMode=KernelRegs[45];	 //ģ���������1ģʽ
   		Param.AOutPort.CH3OutMode=KernelRegs[46];	 //ģ���������1ģʽ
   		Param.AOutPort.CH4OutMode=KernelRegs[47];	 //ģ���������1ģʽ

		Param.Tens.Zero=KernelRegs[63];//	KernelRegs[63];			  //�������
		Param.Basic.MainSpeedZero=KernelRegs[64];// KernelRegs[64];		//�ٶ�	���

//		KernelRegs[67]		  67	�������ȴ���
		StartRead=1;
		IDIO1=1-GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
		IDIO2=1-GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
		IDIO3=1-GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
		IDIO4=1-GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9);
		State.Basic.ID=(IDIO1+(IDIO2<<1)+(IDIO3<<2)+(IDIO4<<3))+1;
		if(State.Basic.ID<=16)					//С��10������Ч
		{
		   	State.Basic.ID=State.Basic.ID;
		}
		else
		{
	   		State.Basic.ParamERR=1;			//��������
			State.Basic.ParamERRID=5;	  //���뿪�ش���
		}

		if(Param.Basic.AutoClearDia==0)		 //ͣ���ֶ����棬�ϵ��ֶ��ָ�
		{
			if((KernelRegs[65]>=1))	//������Ч	,���б���ľ�ֵ
			{
			 	State.ToPLC.Bit.Save=1;
			}
		}
		else if(Param.Basic.AutoClearDia==2)		 //ͣ���Զ����棬�ϵ��Զ��ָ�
		{
			if((KernelRegs[65]>=1))			   //����Ѽ���
			{
				State.Work.DiaPer=(KernelRegs[65]&0x00ff-1)*5;  //A����ٷֱ�
				State.Work.Dia=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer)/1000+Param.Dai.LeastDia;
				
				State.Work.DiaPer2=(((KernelRegs[65]&0xff00)>>8)-1)*5;  //B����ٷֱ�
				State.Work.Dia2=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2)/1000+Param.Dai.LeastDia;
						//	KernelRegs[65]=0;

				if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
				{
					if(State.Work.DiaPer>0)
					{
						if(Param.Basic.MachMode==1)				//�ž�
						{
							State.Work.RollDT=((Param.Dai.MostDia-State.Work.Dia)*10)/(Param.Dai.Ply*2);
							State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
						}
						else if(Param.Basic.MachMode==2)				//�վ�
						{
 								State.Work.RollDT=((State.Work.Dia-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
						}
					}
					if	(State.Work.DiaPer2>0)
					{
  						if(Param.Basic.MachMode==1)				//�ž�
						{
							State.Work.RollDT2=((Param.Dai.MostDia-State.Work.Dia2)*10)/(Param.Dai.Ply*2);
							State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
						}
						else if(Param.Basic.MachMode==2)				//�վ�
						{
							State.Work.RollDT2=((State.Work.Dia2-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
							State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
						}
					}
				}
			}
		}
		if(KernelRegs[66]==1) //���쳣ͣ��
		{
		 	KernelRegs[66]=0;	 //����ϴ��쳣ͣ��
			State.ToPLC.Bit.Break=1;         //0:������1���ϴ��з�����ͣ�������û�ȷ��
		}
	}

   	if(Param.Basic.SetWay==2)	  			//  ����ֵ       2��������ͨ��
	{
   		Param.Auto.SetAim=KernelRegs[69]*10;		  //�趨Ŀ��ֵ	   //(0~1000,��Ӧ0��10V)
	}
 
	if(Param.Basic.MainSpeedWay==2)	  			//   ���ٶ�2��������ͨ��
	{
		if(KernelRegs[75]<=1000)
		{
	 		State.Work.MSpeed=(KernelRegs[75]*10);          //(0~1000,��Ӧ0��10V)
		}
	}
	if(Param.Dai.Way==6)	 				//���ź�������ͨ��
	{
		if(KernelRegs[76]<=100)			//0%~100%  ������Ч
		{
			if(State.Basic.OutCh==0)			//A�����ģʽ
			{
	    		State.Work.DiaPer=KernelRegs[76]*10; 
				State.Work.Dia=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer)/1000+Param.Dai.LeastDia;
			}
			else
			{
	    		State.Work.DiaPer2=KernelRegs[76]*10; 
				State.Work.Dia2=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2)/1000+Param.Dai.LeastDia;
			}
		}
	}

//**************************״̬����λ*************************************************************
	  PLC_Command_Data.All=KernelRegs[79]; 	  //��ȡ����
	  PLC_Command_Data2.All=KernelRegs[73]; 
	  if(PLC_Command_Data.All!= State.PLC_Command.All)		  //�Ƚ��Ƿ��б仯
	  {				   											//��λ�Ƚ�			   											//��λ�Ƚ�
			if(PLC_Command_Data.Bit.Run!=State.PLC_Command.Bit.Run)		 
			{
			    RunSwitch(PLC_Command_Data.Bit.Run);              //����RUN���ơ�
	//			PLC_Command_Data.Bit.Run=0;		//��0����ʾ����������
			}
			if(PLC_Command_Data.Bit.Sampling!=State.PLC_Command.Bit.Sampling)
			{
	//		  	PLC_Command_Data.Bit.Sampling!=State.PLC_Command.Bit.Sampling
			}
			if(PLC_Command_Data.Bit.Refurbish!=State.PLC_Command.Bit.Refurbish)
			{

			}
			if(PLC_Command_Data.Bit.Off_Line!=State.PLC_Command.Bit.Off_Line)
			{

			}
			if(PLC_Command_Data.Bit.JOG!=State.PLC_Command.Bit.JOG)
			{

				IOJog(PLC_Command_Data.Bit.JOG);
		//		PLC_Command_Data.Bit.JOG
			}
			if(PLC_Command_Data.Bit.XZFX!=State.PLC_Command.Bit.XZFX)
			{
				MainMotFR(PLC_Command_Data.Bit.XZFX);		   	//������������ת����				
			}
			if(PLC_Command_Data.Bit.ERR!=State.PLC_Command.Bit.ERR)
			{

			}
			if(PLC_Command_Data.Bit.Reset!=State.PLC_Command.Bit.Reset)	   //�ϵ㸴λ
			{
				if(PLC_Command_Data.Bit.Reset==1)
				{
				 	State.ToPLC.Bit.Break=0;         //0:������1���ϴ��з�����ͣ�������û�ȷ��
					PLC_Command_Data.Bit.Reset=0;
				}
			}
			if(PLC_Command_Data.Bit.Save!=State.PLC_Command.Bit.Save)	   //������
			{
		   		if(PLC_Command_Data.Bit.Save==1)
				{
				 	PLC_Command_Data.Bit.Save=0;
					DiaSave();
				}
			}
			if(PLC_Command_Data.Bit.ClearSave!=State.PLC_Command.Bit.ClearSave)	  //���������
			{
				if(PLC_Command_Data.Bit.ClearSave==1)
				{
				 	State.ToPLC.Bit.Save=0;
					KernelRegs[65]=0;
					RegsFlag[65] =1;	 //��EEPROM��д
				//	State.Work.DiaPer=0;
					PLC_Command_Data.Bit.ClearSave=0;
				}
			}
			if(PLC_Command_Data.Bit.UseSave!=State.PLC_Command.Bit.UseSave)
			{
				if(PLC_Command_Data.Bit.UseSave==1)
				{
				 	 if((KernelRegs[65]>=1))			   //����Ѽ���
					 {
							State.Work.DiaPer=(KernelRegs[65]&0x00ff-1)*5;  //A����ٷֱ�
						    State.Work.Dia=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer)/1000+Param.Dai.LeastDia;
				
							State.Work.DiaPer2=(((KernelRegs[65]&0xff00)>>8)-1)*5;  //B����ٷֱ�
						    State.Work.Dia2=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2)/1000+Param.Dai.LeastDia;
						//	KernelRegs[65]=0;
							if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
							{
								if(State.Work.DiaPer>0)
								{
									if(Param.Basic.MachMode==1)				//�ž�
									{
										State.Work.RollDT=((Param.Dai.MostDia-State.Work.Dia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
									}
									else if(Param.Basic.MachMode==2)				//�վ�
									{
 										State.Work.RollDT=((State.Work.Dia-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
									}

								}
								if	(State.Work.DiaPer2>0)
								{
  									if(Param.Basic.MachMode==1)				//�ž�
									{
										State.Work.RollDT2=((Param.Dai.MostDia-State.Work.Dia2)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
									}
									else if(Param.Basic.MachMode==2)				//�վ�
									{
										State.Work.RollDT2=((State.Work.Dia2-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
									}
								}
							}
					 }
					PLC_Command_Data.Bit.UseSave=0;
				}
			}
			if(PLC_Command_Data.Bit.ClearDia!=State.PLC_Command.Bit.ClearDia)
			{
				if(PLC_Command_Data.Bit.ClearDia==1)			//����0
				{
					DaiClear();	  //��������0
					PLC_Command_Data.Bit.ClearDia=0;	   		//	 //��0����ʾ����������
				}

			}
			if(PLC_Command_Data.Bit.TensZero!=State.PLC_Command.Bit.TensZero)
			{
				if(PLC_Command_Data.Bit.TensZero==1)
				{
				 	Param.Tens.Zero=Port.AIn.Ch1;	  		//���õ�ǰ�˿�����ֵΪ�������
					 KernelRegs[63]=  Param.Tens.Zero;
					 PLC_Command_Data.Bit.TensZero=0;		  //��0����ʾ����������
				}
			}
			if(PLC_Command_Data.Bit.MainSpeedZero!=State.PLC_Command.Bit.MainSpeedZero)
			{
				if(PLC_Command_Data.Bit.MainSpeedZero==1)
				{
					Param.Basic.MainSpeedZero=Port.AIn.Ch2;	  		//���õ�ǰ�˿�����ֵΪ�����ٶ����
					KernelRegs[64]=	Param.Basic.MainSpeedZero;
					PLC_Command_Data.Bit.MainSpeedZero=0;	 //��0����ʾ����������
				}
			}
			if(PLC_Command_Data.Bit.ABChange!=State.PLC_Command.Bit.ABChange)
			{
				ChangAction(PLC_Command_Data.Bit.ABChange);
			}
			if(PLC_Command_Data.Bit.DiaSet!=State.PLC_Command.Bit.DiaSet)
			{
			 	if(PLC_Command_Data.Bit.DiaSet==1)
				{
					if(State.Basic.OutCh==0)			//A�����ģʽ	
					{
						State.Work.DiaPer=KernelRegs[74]*10; 
						State.Work.Dia=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer)/1000+Param.Dai.LeastDia;
						if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
						{
								if(State.Work.DiaPer>0)
								{
									if(Param.Basic.MachMode==1)				//�ž�
									{
										State.Work.RollDT=((Param.Dai.MostDia-State.Work.Dia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
									}
									else if(Param.Basic.MachMode==2)				//�վ�
									{
 										State.Work.RollDT=((State.Work.Dia-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT=State.Work.RollDT*Param.Dai.RollD;
									}

								}
						}
					}
					else   if(State.Basic.OutCh==1)			//B�����ģʽ	
					{
					    State.Work.DiaPer2=KernelRegs[74]*10; 
						State.Work.Dia2=((Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2)/1000+Param.Dai.LeastDia;
						if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
						{
 								if	(State.Work.DiaPer2>0)
								{
  									if(Param.Basic.MachMode==1)				//�ž�
									{
										State.Work.RollDT2=((Param.Dai.MostDia-State.Work.Dia2)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
									}
									else if(Param.Basic.MachMode==2)				//�վ�
									{
										State.Work.RollDT2=((State.Work.Dia2-Param.Dai.LeastDia)*10)/(Param.Dai.Ply*2);
										State.Work.RollDT2=State.Work.RollDT2*Param.Dai.RollD;
									}
								}
						}
					}
				}
				PLC_Command_Data.Bit.DiaSet=0;
			}
			//OSMutexPend(RegsWriteMutex,0,&err);
		KernelRegs[79]=	PLC_Command_Data.All;	//���أ���ʾ���������ѱ����� //��Э�飬���践��
			//OSMutexPost(RegsWriteMutex);																				 
			State.PLC_Command.All=PLC_Command_Data.All;	
	  
  }
   if(PLC_Command_Data2.All!= State.PLC_Command2.All)		  //�Ƚ��Ƿ��б仯
   {
 		if(PLC_Command_Data2.Bit.InitParam!=State.PLC_Command2.Bit.InitParam)	//	  //�ָ���������
		{
		 	if(	PLC_Command_Data2.Bit.InitParam==1)
			{
				ParamInit();					//�ָ���������ֵ
				PLC_Command_Data2.Bit.InitParam=0;
			}

		}
		if(PLC_Command_Data2.Bit.ADCZero!=State.PLC_Command2.Bit.ADCZero)	//	 Bit1:1 ģ��������У��
		{
		 	if(PLC_Command_Data2.Bit.ADCZero==1)
			{
				ADCZeroS();
				PLC_Command_Data2.Bit.ADCZero=0;
			}
		}
		if(PLC_Command_Data2.Bit.ADCTest!=State.PLC_Command2.Bit.ADCTest)	//	 Bit2:1 ģ��������У׼
		{
		 	if(PLC_Command_Data2.Bit.ADCTest==1)
			{
			 	ADCTsetS();
				PLC_Command_Data2.Bit.ADCTest=0;
			}
		}
 		State.PLC_Command2.All=PLC_Command_Data2.All;	
		KernelRegs[73]=PLC_Command_Data2.All;
   }
	 if(KernelRegs[77]==4001)		  //����ADC����ڣ�����ֵ����ʱ��
	 {
		 ADCZeroS();
		if(State.Out.Mode==2)	
		{
	   		State.Out.Mode=0;
		}
	}
	else  if(KernelRegs[77]==4002)
	{
			ADCTsetS();
		if(State.Out.Mode==2)	
		{
	   		State.Out.Mode=0;
		}
	}
	else  if((KernelRegs[77]>1000)&&(KernelRegs[77]<=2000))
	{
		if(State.Out.Mode==0)	
		{
	   		State.Out.Mode=2;
		}
	}

	if((KernelRegs[78]>1000)&&(KernelRegs[78]<=2000))
	{
		if(State.Out.Mode2==0)	
		{
	   		State.Out.Mode2=2;
		}
	}
	else
	{
		if(State.Out.Mode2==2)	
		{
	   		State.Out.Mode2=0;
		}
	}
//**********************״̬�ϱ���PLC********************************************************
	if(State.Basic.Run!=0)
	{
   		State.ToPLC.Bit.Run=1;
	}
	else
	{
   		State.ToPLC.Bit.Run=0;
	}
   	State.ToPLC.Bit.ZeroFLG= State.Basic.ZeroFLG;
   	State.ToPLC.Bit.OverFLG= State.Basic.OverFLG;
   	State.ToPLC.Bit.JOG= State.Basic.Jog;
   	State.ToPLC.Bit. XZFX=Param.Basic.MMFR;				//0����ת��1��ת;    //�����ת����0��10V��0��0��-10V:1
	State.ToPLC.Bit.ERR=State.Basic.Alarm;
	State.ToPLC.Bit.OK=State.Basic.OK;			//�Ѿ�����
//	State.ToPLC.Bit.Save=0;		    //δ����
//    State.ToPLC.Bit.Break=0;         //0:������1���ϴ��з�����ͣ�������û�ȷ��
	State.ToPLC.Bit.A_B=State.Basic.OutCh ;  //0:A�ᣬ1��B
	State.ToPLC.Bit.EepromErr= State.Work.EEPROMERR;
	State.ToPLC.Bit.DiaErr=	State.Work.DIAERR;
//	State.ToPLC.Bit.ParamC
	State.ToPLC.Bit.ParamErr= State.Basic.ParamERR;
	State.ToPLC.Bit.USBState= State.Basic.USBState;

	State.ToPLC.Bit.DiaOK=State.Basic.DiaOK;

	KernelRegs[80]=	State.ToPLC.All;
	KernelRegs[81]=	State.Work.Tens/10;		//0~10000	 ʵ������
	KernelRegs[82]=	PidOut/10;		//0~10000	 PID�������ֵ
	KernelRegs[83]=	(Pid.SetD)/10;	  //PIDĿ��ֵ  =����ֵ*׶��%
	KernelRegs[84]=	Param.Auto.SetAim/10;	 //	����ֵ
	KernelRegs[85]=	State.Work.MSpeed/10;		//���ٶ�ֵ
	if(State.Basic.OutCh==0)			//A�����ģʽ	
	{
		KernelRegs[86]=	State.Work.Dia;
	}
	else  if(State.Basic.OutCh==1)			//B�����ģʽ	
	{
		KernelRegs[86]=	State.Work.Dia2;
	}
	KernelRegs[87]=(* BSP_ENC_CNT);
	KernelRegs[88]=((* BSP_ENC_CNT)&0xffff0000)>>16;
//	KernelRegs[89]=((* BSP_ENC_CNT)&0xffff00000000)>>32;
	KernelRegs[90]=Port.DIn.All;		//IO �������
	KernelRegs[91]=Port.AIn.Ch1;	   //1940
	KernelRegs[92]=Port.AIn.Ch2;		   //927  870
	KernelRegs[93]=Port.AIn.Ch3;							//1790
	KernelRegs[94]=Port.AIn.Ch4;						   //1800
	KernelRegs[99]=	State.Basic.ID;

	ParamCheck();			//����У��

/*		if(Param.Basic.MachMode==1)		 //�ž�
		{
		 	Param.Basic.MMFR=0;				//0����ת��1��ת
		}
		else
		{
	   		Param.Basic.MMFR=1;				//0����ת��1��ת
		}
   */
											//���ֵ��Сָʾ��
	if(State.Basic.OutCh==1)			//B�����ģʽ
	{
		if(State.Out.Data2>0)
		{
			LedOnN=((State.Out.Data2-(Param.Basic.MostOut/10))/(Param.Basic.MostOut*12))+1;
		}
		else
		{
		  	LedOnN=0;
		}
	}
	else
	{
		if(State.Out.Data>0)
		{
			LedOnN=((State.Out.Data-(Param.Basic.MostOut/10))/(Param.Basic.MostOut*12))+1;
		}
		else
		{
		  	LedOnN=0;
		}
	}
  	bsp_LedDisplay(LedOnN);
 //*************************************************************************************
}

void AInPortAdjudt()
{
	u16 IICSaveS,SDF;
	u16  CHHH1K,CHHH2K,CHHH3K,CHHH4K,CHHH2Z;
	CHHH1K=2130;   //2053  1940
 	CHHH2K=1236;
	CHHH3K=1810;	  //1790
 	CHHH4K=1790;
 	CHHH2Z=1354;	//927
	IICSaveS=SendWordEEPROM(400,CHHH1K);	   //��һͨ��ϵ������
	while(IICSaveS==0);
	OSTimeDly(1);
	SDF=~  CHHH1K;
	IICSaveS=SendWordEEPROM(500,SDF);
	while(IICSaveS==0);
	OSTimeDly(1);

	IICSaveS=SendWordEEPROM(402,CHHH2K);	   //��2ͨ��ϵ������
	while(IICSaveS==0);
	OSTimeDly(1);
	SDF=~  CHHH2K;
	IICSaveS=SendWordEEPROM(502,SDF);
	while(IICSaveS==0);
	OSTimeDly(1);

	IICSaveS=SendWordEEPROM(404,CHHH3K);	   //��3ͨ��ϵ������
	while(IICSaveS==0);
	OSTimeDly(1);
	SDF=~  CHHH3K;
	IICSaveS=SendWordEEPROM(504,SDF);
	while(IICSaveS==0);
	OSTimeDly(1);

	IICSaveS=SendWordEEPROM(406,CHHH4K);	   //��4ͨ��ϵ������
	while(IICSaveS==0);
	OSTimeDly(1);
	SDF=~CHHH4K;
	IICSaveS=SendWordEEPROM(506,SDF);
	while(IICSaveS==0);
	OSTimeDly(1);
// #if NEWBROD

	IICSaveS=SendWordEEPROM(408,CHHH2Z);	   //��4ͨ��ϵ������
	while(IICSaveS==0);
	OSTimeDly(1);
	SDF=~  CHHH2Z;
	IICSaveS=SendWordEEPROM(508,SDF);
	while(IICSaveS==0);
	OSTimeDly(1);
//#endif
}

void UsbState()
{

}


void ADCZeroS()
{
	u16 	Ch2M;
	u16 IICSaveS,SDF;
	Ch2M= (State.ADC.Ch2Buf[0]+State.ADC.Ch2Buf[1]+State.ADC.Ch2Buf[2]+State.ADC.Ch2Buf[3]+State.ADC.Ch2Buf[4])/5;
	if((Ch2M>900)&&(Ch2M<1300))
	{
		Param.Basic.CH2Z=Ch2M;
	 	IICSaveS=SendWordEEPROM(408,Ch2M);	   //��2ͨ����������ʱ��0V�м������
		while(IICSaveS==0);
		OSTimeDly(1);
		SDF=~  Ch2M;
		IICSaveS=SendWordEEPROM(508,SDF);
		while(IICSaveS==0);
		OSTimeDly(1);
	}
	else
	{
		State.Basic.ParamERR=1;			//��������
	}
}


void ADCTsetS()	   //IO�ڵ���5V��ʱ�����
{
	u16 IICSaveS,SDF;
	u16  CHHH1K,CHHH2K,CHHH3K,CHHH4K;
//	Port.AIn.Ch1=(State.ADC.Ch1Buf[0]+State.ADC.Ch1Buf[1]+State.ADC.Ch1Buf[2]+State.ADC.Ch1Buf[3]+State.ADC.Ch1Buf[4]-Max-Min)/3;
	//Port.AIn.Ch1=Port.AIn.Ch1*2000/Param.Basic.CH1K;
	//5V  ʱ  ��Ӧ1000==CH1* 2000/X   X=CH1*2000/1000
	CHHH1K=((State.ADC.Ch1Buf[0]+State.ADC.Ch1Buf[1]+State.ADC.Ch1Buf[2]+State.ADC.Ch1Buf[3]+State.ADC.Ch1Buf[4])/5)*2;   //2053  1940
//			Port.AIn.Ch2=(Port.AIn.Ch2-Param.Basic.CH2Z)*2000/Param.Basic.CH2K;
//			1000=(CH2-CH2Z)*2000/X
 	CHHH2K=(((State.ADC.Ch2Buf[0]+State.ADC.Ch2Buf[1]+State.ADC.Ch2Buf[2]+State.ADC.Ch2Buf[3]+State.ADC.Ch2Buf[4])/5)-Param.Basic.CH2Z)*2;
	CHHH3K=((State.ADC.Ch3Buf[0]+State.ADC.Ch3Buf[1]+State.ADC.Ch3Buf[2]+State.ADC.Ch3Buf[3]+State.ADC.Ch3Buf[4])/5)*2;	  //1790
 	CHHH4K=((State.ADC.Ch4Buf[0]+State.ADC.Ch4Buf[1]+State.ADC.Ch4Buf[2]+State.ADC.Ch4Buf[3]+State.ADC.Ch4Buf[4])/5)*2;

	if((CHHH1K>1700)&&(CHHH1K<2300))
	{
		Param.Basic.CH1K=CHHH1K;
		IICSaveS=SendWordEEPROM(400,CHHH1K);	   //��һͨ��ϵ������
		while(IICSaveS==0);
		OSTimeDly(1);
		SDF=~  CHHH1K;
		IICSaveS=SendWordEEPROM(500,SDF);
		while(IICSaveS==0);
		OSTimeDly(1);
	}
	else
	{
		State.Basic.ParamERR=1;			//��������
	}
	if((CHHH2K>700)&&(CHHH2K<1300))
	{
		Param.Basic.CH2K=CHHH2K;
		IICSaveS=SendWordEEPROM(402,CHHH2K);	   //��2ͨ��ϵ������
		while(IICSaveS==0);
		OSTimeDly(1);
		SDF=~  CHHH2K;
		IICSaveS=SendWordEEPROM(502,SDF);
		while(IICSaveS==0);
		OSTimeDly(1);
	}
	else
	{
		State.Basic.ParamERR=1;			//��������
	}
	if((CHHH3K>1700)&&(CHHH3K<2200))
	{
		Param.Basic.CH3K=CHHH3K;
		IICSaveS=SendWordEEPROM(404,CHHH3K);	   //��3ͨ��ϵ������
		while(IICSaveS==0);
		OSTimeDly(1);
		SDF=~  CHHH3K;
		IICSaveS=SendWordEEPROM(504,SDF);
		while(IICSaveS==0);
		OSTimeDly(1);
	}
	else
	{
		State.Basic.ParamERR=1;			//��������
	}
	if((CHHH4K>1700)&&(CHHH4K<2200))
	{
		Param.Basic.CH4K=CHHH4K;
		IICSaveS=SendWordEEPROM(406,CHHH4K);	   //��3ͨ��ϵ������
		while(IICSaveS==0);
		OSTimeDly(1);
		SDF=~  CHHH4K;
		IICSaveS=SendWordEEPROM(506,SDF);
		while(IICSaveS==0);
		OSTimeDly(1);
	}
	else
	{
		State.Basic.ParamERR=1;			//��������
	}
}
