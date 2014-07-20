
#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "bsp_TimesInt.h"
#include "app_kernelregs.h"
#include "bsp_eeprom.h"
#include "app_PidOut.h"

int  IYuShu;		//PID ��I������,�з���


void PPB()
{
	KPout=(((PPP *  Pid.KP*Pid.KB )/10000)*Param.Basic.KPid)/1000;     // ������  ���  ��
}

void PPM()
{
	KPout=(((PPP *  Pid.KP)/100)*Param.Basic.KPid)/1000;							// ������  ��	��
}
void PPS()
{
	KPout=(((PPP *  Pid.KP*Pid.KS)/10000)*Param.Basic.KPid)/1000;
}
void PZO()
{
	KPout=0;											// ������  0
}
void PNS()
{
	KPout=0-((((PPP *  Pid.KP*Pid.KS)/10000)*Param.Basic.KPid)/1000);				// ������  ��С	��
}
void PNM()
{
	KPout=0-((((PPP *  Pid.KP)/100)*Param.Basic.KPid)/1000);
}
void PNB()
{
	KPout=0-((((PPP *  Pid.KP*Pid.KB )/10000)*Param.Basic.KPid)/1000);				// ������  ���	��
}

void IPB()
{
	int  Data;
//	int   ABSI;		//I�����ľ���ֵ
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*Pid.KB)<=0xa30000)
	 {
	 	 Data=(Pid.KB*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;
	//	 Data=Data+IYuShu;
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(Pid.KB*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 L64Data=L64Data+IYuShu;
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	Pid.SumError=Pid.SumError+Data;				// ������  ���  ��
}
void IPM()
{
	int  Data;
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*100)<=0xa30000)
	 {
	 	 Data=(100*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(100*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }	
 	Pid.SumError=Pid.SumError+Data;				// ������  ��  ��
}
void IPS()
{
	int  Data;
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*Pid.KS)<=0xa30000)
	 {
	 	 Data=(Pid.KS*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(Pid.KS*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	Pid.SumError=Pid.SumError+Data; 				// ������  ��С  ��
}
void IZO()
{
//	Pid.SumError=Pid.SumError;	
}
void INS()
{
	int  Data;
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*Pid.KS)<=0xa30000)
	 {
	 	 Data=(Pid.KS*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(Pid.KS*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	Pid.SumError=Pid.SumError-Data; 				// ������  ��С  ��	
}
void INM()
{
	int  Data;
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*100)<=0xa30000)
	 {
	 	 Data=(100*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(100*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 Pid.SumError=Pid.SumError-Data;						// ������  ��    ��
}
void INB()
{
	int  Data;
//	int   ABSI;		//I�����ľ���ֵ
	long  L64Data;
	 Data=PPP/10;				//��0��10000���0��1000
	 if((Pid.CycT*Data*Pid.KB)<=0xa30000)
	 {
	 	 Data=(Pid.KB*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;
	//	 Data=Data+IYuShu;
		 IYuShu=Data%(Pid.KI*10000);			//�µ������������´���
		 Data=Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	 else
	 {
  	 	 L64Data=(Pid.KB*(Pid.CycT/5)*Data*Param.Basic.KPid)+IYuShu;		//��ǰֵ���ϴ�����ֵ
		 L64Data=L64Data+IYuShu;
		 IYuShu=L64Data%(Pid.KI*10000);			//�µ������������´���
		 Data=L64Data/(Pid.KI*10000);	  			//10000=100(Pid.KB�İٷֱȣ�*100��Param.Basic.KPid�ٷֱȣ�
	 }
	Pid.SumError=Pid.SumError-Data-1;				// ������  ���  ��
}

void DPB()
{
	KDout=(((Pid.KP*Pid.KD*Pid.KB* DDD)/(Pid.CycT*10000))*Param.Basic.KPid)/100;					//΢����	��� 	��
}
void DPM()
{
	KDout=(((Pid.KP*Pid.KD* DDD)/(Pid.CycT*100))*Param.Basic.KPid)/100;
}
void DPS()
{
	KDout=(((Pid.KP*Pid.KD*Pid.KS* DDD)/(Pid.CycT*10000))*Param.Basic.KPid)/100;						// ΢����  ��С	 ��
}
void DZO()
{
	KDout=0;
}
void DNS()
{
	KDout=0-(((Pid.KP*Pid.KD*Pid.KS* DDD)/(Pid.CycT*10000))*Param.Basic.KPid)/100;					// ΢����  ��С	��
}
void DNM()
{
	KDout=0-(((Pid.KP*Pid.KD* DDD)/(Pid.CycT*100))*Param.Basic.KPid)/100;	
}
void DNB()
{
	KDout=0-(((Pid.KP*Pid.KD*Pid.KB* DDD)/(Pid.CycT*10000))*Param.Basic.KPid)/100;						// ΢����  ���	��
}

void PidOut_Cap()
{
u32 LData32;
	if(Param.Basic.Mode==2)		//ģ��PID
	{
		LData32=Pid.SetD*Pid.Over;
		State.PID.Over=LData32/100;			//������

		LData32=Pid.SetD*Pid.Die;
		State.PID.Die=LData32/100;			//����

		LData32=10000*Pid.Change;
		State.PID.ChangeH=LData32/50;
		State.PID.ChangeL=LData32/200;
		if(Error>0)
		{
			if(Error>State.PID.Over)						//E   PB//ʵ��ֵƫС,������
			{
				if(dErr>0)									//Խ��Խ��
				{
					if(dErr>State.PID.ChangeH)					//PB  PB
					{							
						KPPB					// ������  ���
						KIPB					//���������
						KDPB					//΢�������
						Mod=1000;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PB PM
					{
						KPPB					// ������  ���
						KIPB					//���������
						KDPB					//΢�������
						Mod=1000;
						Mod2=800;
					}
					else 				//PB PS
					{
						KPPB					// ������  ���
						KIPM					//������̫�����׳���
						KDPM					//΢�������
						Mod=1000;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PB PZ
				{
					KPPB					// ������  ���
					KIPM					//���������
					KDPS					//΢����   0
					Mod=1000;
					Mod2=500;

				}
				else			//DDD=0-dError,�����
				{
				    if(DDD<State.PID.ChangeL)				//PB NS��dErrorС��
					{
						KPPM					// 
						KIPS					//
						KDZO					//
						Mod=1000;
						Mod2=400;
					}
					else if(DDD<State.PID.ChangeH)				//PB NM���и�
					{
						KPPS					// ������  ���
						KIPS					//���������
						KDZO					//΢������� 					//С 0 0
						Mod=1000;
						Mod2=200;
					}
					else
					{
						KPPS
						KIZO
						KDZO
						Mod=1000;
						Mod2=0x0;
					}
				}
			}	
			else if(Error>State.PID.Die)						//������
			{
				if(dErr>0)
				{
					if(dErr>State.PID.ChangeH)					//PB  PB
					{							
						KPPB					// ������  ���
						KIPB					//���������
						KDPB					//΢�������		//�� �� ��
						Mod=800;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PB PM
					{
						KPPB					// ������  ��
						KIPM					//���������
						KDPM					//΢�������		/
						Mod=800;
						Mod2=800;
					}
					else 				//PB PS
					{
						KPPM					// ������  ��
						KIPM					//����������
						KDPM					//΢��������		/				
						Mod=800;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PB PZ
				{
					KPPM					// ������  ��
					KIPS					//������   ��
					KDPS					//΢����   ��		/		//�� �� �� 
						Mod=800;
						Mod2=500;
				}
				else
				{
					if(DDD<State.PID.ChangeL)				//PB NS
					{
						KPPS					// ������  С
						KIPS					//������   С
						KDPS					//΢����   С		/				//С  С  -С 
						Mod=800;
						Mod2=400;
					}
					else if(DDD<State.PID.ChangeH)				//PB NM
					{
						KPPS
						KIZO
						KDZO				//0 0 0
						Mod=800;
						Mod2=200;
					}
					else
					{
						KPZO
						KIZO
						KDNS				//0 0 0
						Mod=800;
						Mod2=0x0;
					}
				}
			}
	
			else if(Error>0)						//����
			{
				if(dErr>0)
				{
					if(dErr>State.PID.ChangeH)					//PS  PB
					{							
						KPPM					// ������  ��
						KIPM					//������   ��
						KDPM					//΢����   ��		/		//�� �� �� 
						Mod=600;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PS PM
					{
						KPPM					// ������  ��
						KIPM					//������   ��
						KDPS					//΢����   ��		/		//�� �� �� 
						Mod=600;
						Mod2=800;
					}
					else 
					{
						KPPS					// ������   С
						KIPS					//������    С
						KDPS					//΢����    С		/
						Mod=600;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PS PZ
				{
					KPPS					// ������   С
					KIPS					//������    С
					KDZO					//΢����    0
						Mod=600;
						Mod2=500;
				}
				else
				{
					if(DDD<State.PID.ChangeL)				//PS NS
					{
						KPZO
						KIZO
						KDZO				//0 0 0
						Mod=600;
						Mod2=400;
					}
					else if(DDD<State.PID.ChangeH)				//PS NM
					{
						KPZO					// ������   -С
						KINS					//������    -С
						KDNS					//΢����    -С
						Mod=600;
						Mod2=200;
					}
					else
					{
						KPNS					// ������   -С
						KINS					//������    -С
						KDNM					//΢����    -С
						Mod=600;
						Mod2=0;
					}
				}
			}
		}
		else if(Error==0)
		{
			if(dErr>0)
			{
				if(dErr>State.PID.ChangeH)					//PS  PB
				{							
					KPPM					// ������  ��
					KIPM					//������   ��
					KDPM					//΢����   ��		/		//�� �� �� 
						Mod=500;
						Mod2=1000;
				}
				else if(dErr>State.PID.ChangeL)				//PS PM
				{
					KPPS					// ������  ��
					KIPS					//������   ��
					KDPM					//΢����   ��		/		//�� �� ��			//�� �� ��
						Mod=500;
						Mod2=800;
				}
				else 				//PS PS
				{
					KPPS					// ������   С
					KIPS					//������    С
					KDPS					//΢����    С
						Mod=500;
						Mod2=600;
				}
			}
			else if(dErr==0)				//PS PZ
			{
				KPZO
				KIZO
				KDZO				//0 0 0
						Mod=500;
						Mod2=500;
			}
			else
			{
				if(DDD<State.PID.ChangeL)				//PS NS
				{
					KPNS					// ������   -С
					KINS					//������    -С
					KDNS					//΢����    -С
						Mod=500;
						Mod2=400;
				}
				else if(DDD<State.PID.ChangeH)				//PS NM
				{
					KPNS					// ������   -С
					KINS					//������    -С
					KDNM					//΢����    -С
						Mod=500;
						Mod2=200;
				}
				else
				{
					KPNM					// ������   -��
					KINM					//������    -��
					KDNM					//΢����    -��
						Mod=500;
						Mod2=0x0;
				}
			}
		}
		else 			//С��0
		{
			if(PPP<State.PID.Die)			  //����
			{
				if(dErr>0)
				{
					if(dErr>State.PID.ChangeH)					//PS  PB    �����
					{							
						KPPS					// ������    ��
						KIPS					//+   �ɸĵ�M				//???????????????????????????????  ��׼��KIPS
						KDPM					//��
						Mod=400;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PS PM		 ���С��
					{
						KPZO					// ������ ��
						KIPS		
						KDPS
						Mod=400;
						Mod2=800;
					}
					else 				//PS PS		���0
					{
						KPZO					// ������ ��
						KIZO		
						KDZO
						Mod=400;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PS PZ  ���С��
				{
					KPNS					// ������ -С
					KINS					//��С
					KDZO					//0
						Mod=400;
						Mod2=500;
				}
				else
				{
					if(DDD<State.PID.ChangeL)				//PS NS  ����У�
					{
						KPNS					// ������ ��		-
						KINS					//-
						KDNS					//-
						Mod=400;
						Mod2=400;
					}
					else if(DDD<State.PID.ChangeH)				//PS NM
					{
						KPNM					// ������ ����
						KINM					//-	
						KDNM					//-
						Mod=400;
						Mod2=200;
					}
					else
					{
						KPNM					// ������ ��  -
						KINM					//-
						KDNM					//-
						Mod=400;
						Mod2=0x0;
					}
				}
			}
			else if(PPP<State.PID.Over)
			{
				if(dErr>0)
				{
					if(dErr>State.PID.ChangeH)					//PS  PB
					{							
						KPZO					// 
						KIZO		
						KDPS
						Mod=200;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PS PM
					{
						KPNS					// 
						KIZO		
						KDZO
						Mod=200;
						Mod2=800;
					}
					else 				//PS PS		�����С���������
					{
						KPNS					// ������ ��	-
						KINS					//    -
						KDNS					//-
						Mod=200;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PS PZ
				{
					KPNM					// ������ ��	-
					KINS					//-	
					KDNS					//0
						Mod=200;
						Mod2=500;
				}
				else
				{
					if(DDD<State.PID.ChangeL)				//PS NS
					{
						KPNM					// ������ ��  -
						KINM					//-
						KDNM					//
						Mod=200;
						Mod2=400;
		
					}
					else if(DDD<State.PID.ChangeH)				//PS NM
					{
						KPNB					// ������ ��  -
						KINM					//-
						KDNM					//
						Mod=200;
						Mod2=200;
					}
					else
					{
						KPNB					// ������ ��  -
						KINB					//-
						KDNB					//-
						Mod=200;
						Mod2=0x0;
					}
				
				}
			}
			else
			{
				if(dErr>0)
				{
					if(dErr>State.PID.ChangeH)					//PS  PB
					{							
						KPNS					// 
						KIZO		
						Mod=0;
						Mod2=1000;
					}
					else if(dErr>State.PID.ChangeL)				//PS PM
					{
						KPNS					// -
						KINS		
						KDZO
						Mod=0x0;
						Mod2=800;
					}
					else 				//PS PS		�����С���������
					{
						KPNM					// ������ ��	-
						KINS					//    -
						KDZO					//-
						Mod=0x0;
						Mod2=600;
					}
				}
				else if(dErr==0)				//PS PZ
				{
					KPNM					// ������ ��	-
					KINM					//-	
					KDNS					//0
						Mod=0x0;
						Mod2=500;
				}
				else 
				{
					if(DDD<State.PID.ChangeL)				//PS NS
					{
						KPNM					// ������ ��  -
						KINM					//-
						KDNM					//-
						Mod=0x0;
						Mod2=400;
		
					}
					else if(DDD<State.PID.ChangeH)				//PS NM
					{
						KPNB					// ������ ��  -
						KINB					//-
						KDNB					//
						Mod=0x0;
						Mod2=200;
					}
					else
					{
						KPNB					// ������ ��  -
						KINB					//-
						KDNB					//-
						Mod=0x0;
						Mod2=0x0;
					}
				}
			}
		}
	}
	else if(Param.Basic.Mode==1)
	{
		if(Error>=0)
		{
			if(dErr>0)
			{
				KPPM					// ������ ��  -
				KIPM					//-
				KDPM					//-
					Mod=0x99;
			}
			else
			{
				KPPM					// ������ ��  -
				KIPM					//-
					Mod=0x99;
			}
		}
		else
		{
			if(dErr>0)
			{
				KPNM					// ������ ��  -
				KINM					//-
					Mod=0x99;
			}
			else
			{
				KPNM					// ������ ��  -
				KINM					//-
				KDNM					//-
					Mod=0x99;
			}
		}
	}
	else if(Param.Basic.Mode==4)			//�ֶ�  /�㶨
	{
//		KPout=10000;
//  		KPout=Param.Basic.SetOut*KPout/1000;		//���ֵ=����ֵ��ǧ�ֱȣ�*10V��10000��/1000	
		KPout=Param.Basic.SetOut*10;
		Pid.SumError=0;
		KDout=0;
	}
}


///////////////////////////////////////�����Ʒ�ʽ���//////////////////////////////////////////////
uint DaiOut()
{
uint DAIOut;
ulong LLLL;
//	if(Param.Basic.MostOut==1)		//Ť�����
	{
		DAIOut=Param.Basic.LeastOut+(Param.Basic.MostOut-Param.Basic.LeastOut)*State.Work.DiaPer/1000;
//		if(Param.Basic.MachMode==2)				//�վ�
		{
			if(Param.Basic.ZDLine==0)			//����������
			{
				DAIOut=DAIOut;					
			}
			if(Param.Basic.ZDLine==1)			//ֱ��׶��
			{
				LLLL=State.Work.DiaPer*Param.Basic.ZDZ;	//FX=F0(1-State.Work.Dia*Param.Basic.ZDZ)
				LLLL=1000000-LLLL;					//State.Work.Dia=DX/DMAX  100.0%
				LLLL=LLLL*DAIOut;
				DAIOut=LLLL/1000000;
			}
			else if(Param.Basic.ZDLine==2)		//����
			{
				LLLL=State.Work.DiaPer/2;					//XXX=State.Work.Dia*������
				//huzhiwen comment LLLL=3162-ZDQX[LLLL];
				LLLL=3162000-LLLL*Param.Basic.ZDZ;	//FX=F0*XXXX*Param.Basic.ZDZ)
				LLLL=LLLL*DAIOut;
				DAIOut=LLLL/3162000;
			}
		}
	}
//	else if(Param.Basic.MostOut==2)		//	//�ٶ�ģʽ����Խ�����ԽС���ٶȿ����޷�ʵ��׶�ȿ���
	{
	//	if(Param.Basic.MachMode==1)				//�ž�
		{
			DAIOut=Param.Basic.LeastOut+(Param.Basic.MostOut-Param.Basic.LeastOut)*(1000-State.Work.DiaPer)/1000;
		}
	//	else if(Param.Basic.Mache==2)				//�վ�
		{

		}
	}
	DAIOut=DAIOut*4096/1000;
	return (DAIOut);
}


