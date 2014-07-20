
/*  
����
time:2008,09,26
��־����ֲ 20120215
*/

#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "bsp_TimesInt.h"
#include "app_kernelregs.h"
#include "bsp_eeprom.h"
#include "app_PidOut.h"

uint PPP,DDD;
uint Mod,Mod2;
int	Error,dErr;
long PidErrAbs;
long PidErrO;
int  ErrAbs;
int	 ErrO;

//struct PidRunState PidRun;

u32 MSpeedP;
u32  KPSL;

void InitPIDOut()
{
	if(State.Basic.OutCh==0)		
	{
 		 Pid.SumError=State.Out.Data;
	}
	else 
	{
 		 Pid.SumError=State.Out.Data2;
	}
	  Pid.Start=0;
	RUNState.No=0;	 
	RUNState.UseNo=0;
}


uint PIDOut(int NextPoint )
{
static	int dError[5];
u16 JdE;
static	u16 MSPTimes=100;    //	�жϾ���
static	u32 LMS[5];	//��5�����ٶ�
u16  MSPADD;   //�Ӽ��ٶȱ�־,�ж���������
static	u32  KPDS=100;    //��̬PIDϵ��
u32 LDataP;
u32 DiaOpenOut;       //���ݾ�����Ŀ������ֵ
u32  ABSKIOut;			//PID�����KI��� �� ��	�� ֵ
	LDataP=10000;  //10V
	LDataP=LDataP*Param.Basic.MostOut;
	Pid.IFull=LDataP/100;				//���ֱ��ͣ�������ֵ����Pid.IFullΪ������ֵ
	State.PID.MostOut=Pid.IFull;

	LDataP=10000;  //10V
	LDataP=LDataP*Param.Basic.LeastOut;
	Pid.IZero=LDataP/100;				//���ֱ��ͣ�������ֵ����Pid.IFullΪ������ֵ
//	State.PID.MostOut=Pid.IFull;
		
    if(Param.Basic.Out==1)			//Ť�����״̬
	{
		if((Param.Basic.MachMode==1)||(Param.Basic.MachMode==2)||(Param.Basic.MachMode==3))				//�ž�׶��  �ž�ʱ��Ť�����״̬�����������������ֵ�������С
		{
			if(Pid.Pol==1)								//PID�ļ���
			{
					Error = Pid.SetD-NextPoint;           // ƫ��������ֵ������ʵֵ���������������ֵС��ʵ��ֵ�������С
			}
			else
			{
					Error = NextPoint-Pid.SetD;           // ƫ��������ֵ������ʵֵ�������С���������ֵС��ʵ��ֵ��������
			}
		}
	}
	else if(Param.Basic.Out==2)				//�ٶ����״̬
	{
		if(Param.Basic.MachMode==1)				//�ž�׶��  �ž�ʱ���ٶ����״̬�����������������ֵ��������
		{
			if(Pid.Pol==2)								//PID�ļ��ԣ���������
			{
					Error = Pid.SetD-NextPoint;           // ƫ��������ֵ������ʵֵ�������С���������ֵС��ʵ��ֵ��������

			}
			else								//����Ĭ��ֵ
			{	
					Error = NextPoint-Pid.SetD ;           // ƫ��������ֵ������ʵֵ���������������ֵС��ʵ��ֵ�������С
												//ʵ��ֵ���ʱ��Ӧ�ӿ��ٶȣ���Сʱ��С�ٶ�
			}
		}
		else if((Param.Basic.MachMode==2)||(Param.Basic.MachMode==3))				//�վ�׶��  ��������������ֵ��Ӧ��С�ٶȣ�����С�ˣ�Ӧ����
		{
			if(Pid.Pol==1)								//PID�ļ���
			{
					Error = Pid.SetD-NextPoint;           // ƫ�����ֵ����ʵ��ֵʱ��������󣬼��٣�ʹ������
			}
			else
			{
					Error = NextPoint-Pid.SetD ;           // ƫ�
			}
		}
	}
	if(Pid.Start==0)	 //PID��ʼ��
	{
		Pid.LastError=Error;
		Pid.Start=1;
		LMS[1]=State.Work.MSpeed;
		LMS[2]=State.Work.MSpeed;
		LMS[3]=State.Work.MSpeed;
		LMS[4]=State.Work.MSpeed;
	}
//****************************���ٶ��������ж�**************************************************
	MSPADD=100;						   //����MSPADD��ֵ���жϼӼ��١�>103����<97��
	for(JdE=0;JdE<4;JdE++)
	{
		  dError[JdE]=dError[JdE+1];	//΢�����飬���ٶ��ж��޹�
		  if((LMS[JdE+1]-LMS[JdE])>5)		 //���ٶ��жϣ���һ���ٶȴ���ǰһ���ٶ�0.005V
		  {
				MSPADD++;					 //�Ӽ��ٶ�����
		  }
		  else if((LMS[JdE]-LMS[JdE+1])>5)
		  {
				MSPADD--;
		  }
	  LMS[JdE]=LMS[JdE+1];					//���ٶ� ����		  
	}
	LMS[4]=State.Work.MSpeed;		//��ǰ���ٶ�
	if((LMS[4]-LMS[3])>5)		 //���ٶ�
	{
		MSPADD++;					 //�Ӽ��ٶ�����
	}
	else if((LMS[3]-LMS[4])>5)
	{
		MSPADD--;
	}
//*************************΢���벻��ȫ΢��*************************************************
	dError[4] = Error - Pid.LastError;             // ��ǰ΢�֣���ǰ���ϴ����
	Pid.LastError=Error;						  //��¼��ǰ���Ա��´μ���΢��

	if(Error>=0)								 //���Ϊ��
	{
		PPP=Error;
	}
	else
	{
		PPP=0-Error;
	}
	dErr=dError[4]*5+dError[3]*4+dError[2]*3+dError[1]*2+dError[0];	//΢���˲�
	dErr=dErr/15;									//����ȫ΢��
	if(dErr>=0)									   //΢��Ϊ��
	{
		DDD=dErr;
	}
	else
	{
		DDD=0-dErr;
	}

	if(RUNState.UseNo<499)	 //0~498ʱ
	{
		 if(RUNState.UseNo==0)
	 	{
	 		PidErrAbs=0;
			PidErrO=0;
	 	}
		RUNState.ErrAbs[RUNState.No]= PPP;		 //����ֵ
	    RUNState.ErrO[RUNState.No]=Error; 		//�з���ֵ
		PidErrAbs=PidErrAbs+RUNState.ErrAbs[RUNState.No];
		PidErrO=PidErrO+RUNState.ErrO[RUNState.No];
		ErrAbs=	PidErrAbs/(RUNState.UseNo+1);		//ÿ��PID���ڵ�������ֵ��ƽ��
		ErrO=PidErrO/(RUNState.UseNo+1);		//ÿ��PID���ڵ�ƽ��������ֵ
		RUNState.No++;
		RUNState.UseNo++;
	}
	else		//0~499������
	{
		if(PidErrAbs>=RUNState.ErrAbs[RUNState.No])
		{
			PidErrAbs=PidErrAbs-RUNState.ErrAbs[RUNState.No];
		}
		else
		{
			PidErrAbs=0;
		}
		PidErrO=PidErrO-RUNState.ErrO[RUNState.No];
		RUNState.ErrAbs[RUNState.No]= PPP;		 //����ֵ
		RUNState.ErrO[RUNState.No]=Error; 		//�з���ֵ	
		PidErrAbs=PidErrAbs+RUNState.ErrAbs[RUNState.No];
		PidErrO=PidErrO+RUNState.ErrO[RUNState.No];
		ErrAbs=	PidErrAbs/500;		//ÿ��PID���ڵ�������ֵ��ƽ��
		ErrO=PidErrO/(RUNState.UseNo+1);		//ÿ��PID���ڵ�ƽ��������ֵ
		if(RUNState.No<RUNState.UseNo)
		{
			RUNState.No++;	
		}
		else
		{
		  	RUNState.No=0;
		}
	}
	PidOut_Cap();
//************************************���ָ���****************************************************8
	KIout=Pid.SumError;			//�������
	if(KIout>=0)
	{
		ABSKIOut=KIout;
	}
	else
	{
 		ABSKIOut=0-KIout;
	}
	if(Param.Basic.Out==2)				//�ٶ����״̬
	{									//ֻ�����ٶ�ģʽ�²��ж��ٶȵĸ���
		if(State.Work.MSpeed>200)	//���ٶȴ���0.2V
		{
			if(KIout>=(PidOut/100))			//	  30���ֵ��/////�Ժ�Ҫ�ĳɶ�̬�ı�����ϵ  ,��Ϊ�����ֵ��1/20
			{
				if(ErrAbs<((State.PID.Over*35)/10))	   //����ڳ���������
				{
					MSPTimes++;
					if(State.Basic.DiaOK==0)		//��δ���
					{
						MSPTimes++;
					}
					if(KIout>=(PidOut/12))		  //
					{
					  	MSPTimes++;
						if(State.Basic.DiaOK==0)		//��δ���
						{
							MSPTimes++;
						}
					}
					if(KIout>(PidOut/8))
					{
		  			  	MSPTimes++;
						if(State.Basic.DiaOK==0)		//��δ���
						{
							MSPTimes++;
						}
					}
					if(KIout>=(PidOut/4))
					{
					  	MSPTimes++;
						if(State.Basic.DiaOK==0)		//��δ���
						{
							MSPTimes++;
						}
					}
					if(KIout>=(PidOut/2))
					{
					  	MSPTimes++;
						if(State.Basic.DiaOK==0)		//��δ���
						{
							MSPTimes++;
						}
					}
				}
			}
			else if(KIout<0)
			{
				if(ErrAbs<((State.PID.Over*35)/10))
				{
					if(ABSKIOut>=((PidOut+ABSKIOut)/100))			 //	KIout<0,�������ֵ����PID�����1/20
					{
						MSPTimes--;
						if(State.Basic.DiaOK==0)		//��δ���
						{
							MSPTimes--;
						}
						if(ABSKIOut>=((PidOut+ABSKIOut)/12))
						{
						  	MSPTimes--;
							if(State.Basic.DiaOK==0)		//��δ���
							{
								MSPTimes--;
							}
						}
						if(ABSKIOut>=((PidOut+ABSKIOut)/8))
						{
						  	MSPTimes--;
							if(State.Basic.DiaOK==0)		//��δ���
							{
								MSPTimes--;
							}
						}
						if(ABSKIOut>=((PidOut+ABSKIOut)/4))
						{
						  	MSPTimes--;	
							if(State.Basic.DiaOK==0)		//��δ���
							{
								MSPTimes--;
							}
						}
						if(ABSKIOut>=((PidOut+ABSKIOut)/2))
						{
						  	MSPTimes--;
							if(State.Basic.DiaOK==0)		//��δ���
							{
								MSPTimes--;
							}
						}
					}
				}
			}
		}
		if(MSPTimes>108)	 //���ִ���0�Ƚ�
		{
	//		if((MSPADD<103)&&(MSPADD>97))	  //���ٶ�ʱ
			{
	//			if(MSpeedP<(Param.Basic.MaxMainSpeedPor*10))		  //�Ѵ�������ٶȱ���
				{
					MSpeedP++;
				}
			}
			MSPTimes=100;
		}
		else if(MSPTimes<92)
		{
	//		if((MSPADD<103)&&(MSPADD>97))	  //���ٶ�ʱ
			{
	//			if(MSpeedP>(Param.Basic.MaxMainSpeedPor/100))		  //��С30/1000���ٶȱ���
				{
					MSpeedP--;
				}
			}
			MSPTimes=100;
		}
		if(State.Work.MSpeed<=10000);          //���ٶ�10V����
		{
		    if((Param.Dai.Way==0)||(Param.Dai.Way==4))		//��������Լ�ģ������ֵ��	
			{
				if(Param.Basic.Out==2)				//�ٶ����״̬
				{
					DiaOpenOut=(MSpeedP*((LMS[4]+LMS[3]+LMS[2]+LMS[1]+LMS[0])/5))/1000;	   //���ٶ�ʱ�����ٶȵ��˲���ʹ����ȶ�
					if(Param.Basic.MaxMainSpeedPor>0)
					{
						Pid.IFull=((LMS[4]+LMS[3]+LMS[2]+LMS[1]+LMS[0])/5)*Param.Basic.MaxMainSpeedPor/100;
						Pid.IFull=Pid.IFull+Pid.IFull/20;			//105%	 ,�ʵ���һЩ���������
					}
					if(Param.Dai.Way==4)
					{
						if(ErrAbs<(State.PID.Over*3))	   //����ڳ���������
						{
							if(ABSKIOut<=(DiaOpenOut/5))	 //���������С
							{
						//		MSpeedP=(Param.Basic.MaxMainSpeedPor*Param.Dai.LeastDia*10)/State.Work.Dia;	   //��ʼֵΪ��С��
								State.Basic.DiaOK=1;
								if(State.Basic.OutCh==0)			//A�����ģʽ
								{
								 	State.Work.Dia=(Param.Basic.MaxMainSpeedPor*Param.Dai.LeastDia*10)/MSpeedP;
									if(Param.Dai.MostDia>Param.Dai.LeastDia)
									{
										State.Work.DiaPer=((State.Work.Dia-Param.Dai.LeastDia)*1000)/(Param.Dai.MostDia-Param.Dai.LeastDia);		//	 1/1000
									}
								}
								else		   //B�����
								{
								 	State.Work.Dia2=(Param.Basic.MaxMainSpeedPor*Param.Dai.LeastDia*10)/MSpeedP;
									if(Param.Dai.MostDia>Param.Dai.LeastDia)
									{
										State.Work.DiaPer2=((State.Work.Dia2-Param.Dai.LeastDia)*1000)/(Param.Dai.MostDia-Param.Dai.LeastDia);		//	 1/1000
									}
								}
							}
						}
					}
				}
			}
			else if((Param.Dai.Way==1)||(Param.Dai.Way==2)||(Param.Dai.Way==3)||(Param.Dai.Way==5)||(Param.Dai.Way==6))		//�����
			{
				DiaOpenOut=((LMS[4]+LMS[3]+LMS[2]+LMS[1]+LMS[0])/5)*Param.Basic.MaxMainSpeedPor/100;	 //����СҶ��Ӧ���ٶȵ����ֵ
				if(Param.Basic.MaxMainSpeedPor>0)
				{
					Pid.IFull=DiaOpenOut+DiaOpenOut/10;				//110%	 ,�ʵ���һЩ���������
				}
				if(State.Basic.OutCh==0)			//A�����ģʽ
				{
					if((State.Work.Dia>0)&&(State.Work.Dia>= Param.Dai.LeastDia))
					{
						DiaOpenOut=(DiaOpenOut*Param.Dai.LeastDia)/State.Work.Dia; //���ݾ���С������Ŀ������ֵ
					}
				}
				else  if(State.Basic.OutCh==1)			//B�����ģʽ
				{
 					if((State.Work.Dia2>0)&&(State.Work.Dia2>= Param.Dai.LeastDia))
					{
						DiaOpenOut=(DiaOpenOut*Param.Dai.LeastDia)/State.Work.Dia2; //���ݾ���С������Ŀ������ֵ
					}
				}
				DiaOpenOut=DiaOpenOut* MSpeedP/1000;
				if((MSpeedP>1250)||	(MSpeedP<750))	 //�����25%���⣬�����ô���
				{
				 	State.Work.DIAERR=1 ;
				}
				else
				{
				  	State.Work.DIAERR=0;
				}
			}
		}
		if(	Pid.IFull>State.PID.MostOut)
		{
			Pid.IFull=State.PID.MostOut;		  //���ֱ���=������ֵ
		}
		
//*********************************�ٶ�ǰ������***************************************************************
		if((MSPADD<103)&&(MSPADD>97))	  //���ٶ�ʱ
		{
		}
		else if(MSPADD>=103)
		{
			 DiaOpenOut=DiaOpenOut+((Param.Basic.MSPAF*(LMS[4]-LMS[0]))/100);		
		}	//	                �ٶ�ǰ���ٷֱ�   ���ٶ�ֵ					 ����ϵ��
		else if(MSPADD<=97)
		{
			DiaOpenOut=DiaOpenOut-((Param.Basic.MSPAF*(LMS[0]-LMS[4]))/100);		
		}	//				���ٶȸ���                �ٶ�ǰ���ٷֱ�   ���ٶ�ֵ					 ����ϵ��
	}
	else if(Param.Basic.Out==1)				//Ť�����״̬
	{
	 	DiaOpenOut=0;
		//*********************************�ٶ�ǰ������***************************************************************
		if((MSPADD<103)&&(MSPADD>97))	  //���ٶ�ʱ
		{

		}
		else if(MSPADD>=103)
		{
				
		}	//	                �ٶ�ǰ���ٷֱ�   ���ٶ�ֵ					 ����ϵ��
		else if(MSPADD<=97)
		{
			
		}
	}

	if(Param.Dai.Way==0)		//�������
	{
		KPDS=1+(((State.Work.MSpeed/10000)*Pid.KPV)/100);
	}
	else
	{
		KPDS=1+(((State.Work.MSpeed/10000)*Pid.KPV)/100);		   
	}

	KPDS=300+(((State.Work.MSpeed/10000)*Pid.KPV)/100);
	KPSL= KPDS;
 /*
	if(KPout>=0)
	{
		KPout=KPout*KPSL/300;
	}
	else
	{
	  	KPout=0-(((0-KPout)*KPSL)/300);
	}
	*/
	if(KIout>=0)
	{
   		if(KIout>Pid.IFull)
		{
			KIout=Pid.IFull;
			Pid.SumError= KIout;
		}
	}
	else
	{
   		if((0-KIout)>Pid.IFull)
		{
			KIout=0-Pid.IFull;
			Pid.SumError= KIout;
		}
	}
	 if(Param.Basic.Out==1)			//Ť�����״̬
	 {
		if(KIout<Pid.IZero)
		{
		  	KIout=Pid.IZero;
			Pid.SumError=KIout;
		}
	 }
	KIout=KIout;
    if(Param.Basic.Out==1)				//Ť�����״̬
	{
    	PidOut=KPout+KDout+KIout;
	}
	else if(Param.Basic.Out==2)				//�ٶ����״̬
	{
		if(Param.Basic.MaxMainSpeedPor>0)
		{
			PidOut=KPout+KDout+KIout+DiaOpenOut;
		}
		else
		{
     		PidOut=KPout+KDout+KIout;
		}
	}	

	if((PidOut<0x7fffffff)&(PidOut>0))
	{
		if(PidOut>(Pid.IFull+200))
		{
			PidOut=Pid.IFull+200;
		}
	}
	else
	{
		PidOut=0;
	}
	if(PidOut<State.PID.LeastOut)
	{
		PidOut=State.PID.LeastOut;
	}
	KernelRegs[95]=KPout;
 	KernelRegs[96]=KIout;
  	KernelRegs[97]=KDout;
//	KernelRegs[93]=ErrAbs;
//	KernelRegs[94] =MSPTimes;
	KernelRegs[98]=MSpeedP;
//	KernelRegs[99]=MSpeedP;
	KernelRegs[89]=	Error/10;
//	KernelRegs[90]=	DDD;
	return (PidOut);
}


void  PID_StateSave()
{
}

void  PID_StateRead()
{
u16 Dat1;
u8 PidStateBuf[10];
//	BSP_EEPROMReadByte( PidStateBuf,10,400, 0x00A0);
    if(PidStateBuf[1]==(~PidStateBuf[3]))
	{
		Dat1=PidStateBuf[1];
	}
	else
	{
	}
    if(PidStateBuf[0]==(~PidStateBuf[2]))
	{
		MSpeedP=Dat1+(PidStateBuf[0]<<8);
	}
	else
	{
	}

}

void PID_StateInit()
{
  	if((Param.Dai.Way==0))		//��������Լ�ģ������ֵ��
	{
	 	MSpeedP=Param.Basic.MaxMainSpeedPor*4;	   //��ʼֵΪ��С��ʱ���ٶȵ�40%
	}
	else if((Param.Dai.Way==4)||(Param.Dai.Way==3))		   //ģ������ֵ���������ֵ��ʱ
	{
		if(State.Basic.OutCh==0)			//A�����ģʽ
		{
			if((State.Work.Dia>0)&(State.Work.Dia>=Param.Dai.LeastDia))
			{
	 	 		MSpeedP=(Param.Basic.MaxMainSpeedPor*Param.Dai.LeastDia*10)/State.Work.Dia;	   //��ʼֵΪ��С��ʱ���ٶȵ�40%
			}
			else
			{
				if(Param.Basic.MachMode==1)			//�ž�
				{									//�ž�ʱ��һЩ
	 		 		MSpeedP=Param.Basic.MaxMainSpeedPor*6;	   //��ʼֵΪ��С��ʱ���ٶȵ�60%
				}
				else							   //�վ�ʱ��һЩ
				{
	 		 		MSpeedP=Param.Basic.MaxMainSpeedPor*4;	   //��ʼֵΪ��С��ʱ���ٶȵ�40%
				}
			}
		}
		else if(State.Basic.OutCh==1)			//B�����ģʽ
		{
			if((State.Work.Dia2>0)&(State.Work.Dia2>=Param.Dai.LeastDia))
			{
	 	 		MSpeedP=(Param.Basic.MaxMainSpeedPor*Param.Dai.LeastDia*10)/State.Work.Dia2;	   //��ʼֵΪ��С��ʱ���ٶȵ�40%
			}
			else
			{
				if(Param.Basic.MachMode==1)			//�ž�
				{									//�ž�ʱ��һЩ
	 		 		MSpeedP=Param.Basic.MaxMainSpeedPor*6;	   //��ʼֵΪ��С��ʱ���ٶȵ�60%
				}
				else							   //�վ�ʱ��һЩ
				{
	 		 		MSpeedP=Param.Basic.MaxMainSpeedPor*4;	   //��ʼֵΪ��С��ʱ���ٶȵ�40%
				}
			}
		}
	}
	else
	{
		MSpeedP=1000;
	}
	RUNState.No=0;	 			//����ͳ���������������
	RUNState.UseNo=0;
//	 PID_StateSave();
}

