
/*������̿���
����
2012 03 27
*/
#include <ucos_ii.h>
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "bsp.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "app_action.h"
#include "app_inout.h"
#include "app_kernelregs.h"
#include "app_OutControl.h"
#include "bsp_enc.h"
#include "app_Port.h"


#define LedOn       GPIO_ResetBits(GPIOE, GPIO_Pin_2);
#define LedOff       GPIO_SetBits(GPIOE, GPIO_Pin_2);

extern OS_EVENT *Tim5Msg; //��ʱ��5����֪ͨ

int aa,bb,cc;
uint a;
uint OldOutA,OldOutB;
 long LongData;
u16  JogTimes=0;		//�㶯ʱ�䣬�㶯ʱ����������ʱ��
u16  DiaTiems=0;		//�����ֵ�����ʱ����ʱ��
u16  MainSpeedTimes=0;		//�����ٶ�����������ʱ����ʱ��

u32 RunTimes;	//����ʱ�䣬Ƿ���������

 u16 Leda;
void OutControl()
{
		u8 err;
   		//�ȴ�֪ͨ
   		OSSemPend(Tim5Msg,0,&err);
//		if(State.Basic.PIDCycFLG==1)			//�ѵ�PID��������
		{		
			for(aa=1;aa<10;aa++)
			{
				bb=aa-1;
				OUTDATA[bb]=OUTDATA[aa];				//A����滻
				OUTDATB[bb]=OUTDATB[aa];				//B����滻
			}
//////////////////////////////������//////////////////////////////////////////////
			if(Param.Dai.Way==0)		//�������
			{
				State.Work.Dia=0;
				State.Work.DiaPer=0;
				State.Work.Dia2=0;
				State.Work.DiaPer2=0;
			}
			else if(Param.Dai.Way==1)		//������ģ�����˿��źŵİٷֱ�
			{
				if(State.Basic.OutCh==0)			//A�����ģʽ
				{
					if(Param.Basic.MachMode==1)				//�ž�
					{
						LongData=(Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer;	//	DAI=ԭʼ������������ԭʼ��������ǰ���ٷֱ�	
					}
					else  if(Param.Basic.MachMode==2)				//�ž�
					{
						LongData=(Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer;	//	DAI=ԭʼ������������ԭʼ��������ǰ���ٷֱ�	
					}
					State.Work.Dia=Param.Dai.LeastDia+LongData/1000;
				}
				else
				{
					if(Param.Basic.MachMode==1)				//�ž�
					{
						LongData=(Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2;	//	DAI=ԭʼ������������ԭʼ��������ǰ���ٷֱ�	
					}
					else  if(Param.Basic.MachMode==2)				//�ž�
					{
						LongData=(Param.Dai.MostDia-Param.Dai.LeastDia)*State.Work.DiaPer2;	//	DAI=ԭʼ������������ԭʼ��������ǰ���ٷֱ�	
					}
					State.Work.Dia2=Param.Dai.LeastDia+LongData/1000;
				}
			}
			else if(Param.Dai.Way==2)		//���Ϻ���ۼӷ�
			{	
				if(State.Basic.OutCh==0)			//A�����ģʽ
				{
					if(Param.Dai.RollAWay==1)	   // A������������IO��6
					{
												  	//����������IO�ڴ�����
					}
					else if(Param.Dai.RollAWay==2)	   // A������������2����������
					{
						if((* BSP_ENC_CNT)>=0)
						{
							State.Work.RollDT= 	State.Work.RollDT+(* BSP_ENC_CNT);
						}
						else
						{
							State.Work.RollDT= State.Work.RollDT+(0-(* BSP_ENC_CNT));
						}
					}
					LongData=(State.Work.RollDT)/Param.Dai.RollD;		//����ת��
					if(Param.Basic.MachMode==1)				//�ž�
					{
						State.Work.Dia=Param.Dai.MostDia-(LongData*Param.Dai.Ply*2)/100;	   //����λ(0.1mm��=���Ϻ�ȣ�um��/100 
						LongData=(State.Work.Dia-Param.Dai.LeastDia)*1000;
						State.Work.DiaPer=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);		//	 1/1000
					}
					else if(Param.Basic.MachMode==2)				//�վ�
					{
						State.Work.Dia=Param.Dai.LeastDia+(LongData*Param.Dai.Ply*2)/100;			//,ֱ������2�����ĵ�λ��0��1mm�������0��01mm���ٳ�10
						LongData=(State.Work.Dia-Param.Dai.LeastDia)*1000;
						State.Work.DiaPer=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);
					}
				}
				else if(State.Basic.OutCh==1)			//B�����ģʽ
				{
					if(Param.Dai.RollBWay==1)	   // B������������IO��6
					{
														//����������IO�ڴ�����
					}
					else if(Param.Dai.RollBWay==2)	   // B������������2����������
					{
						if((* BSP_ENC_CNT)>=0)
						{
							State.Work.RollDT2=State.Work.RollDT2+ (* BSP_ENC_CNT);
						}
						else
						{
							State.Work.RollDT2= State.Work.RollDT2+(0-(* BSP_ENC_CNT));
						}
					}						 	//	DAI=ԭʼ����������������/��ÿת���������������
						LongData=State.Work.RollDT2/Param.Dai.RollD;		//����ת������100��ȥС��
						if(Param.Basic.MachMode==1)				//�ž�
						{
							State.Work.Dia2=Param.Dai.MostDia-(LongData*Param.Dai.Ply*2)/100;	   //1000=100*10= ת��*100   ����λ=����λ/10
							LongData=(State.Work.Dia2-Param.Dai.LeastDia)*1000;
							State.Work.DiaPer2=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);		//	 1/1000
						}
						else if(Param.Basic.MachMode==2)				//�վ�									   
						{
							State.Work.Dia2=Param.Dai.LeastDia+(LongData*Param.Dai.Ply*2)/100;			//,ֱ������2�����ĵ�λ��0��1mm�������0��01mm���ٳ�10��������һ�еĳ���100�����ܳ�1000
							LongData=(State.Work.Dia2-Param.Dai.LeastDia)*1000;
							State.Work.DiaPer2=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);
						}
				}											
			}
			else if(Param.Dai.Way==3)		//��ֵ��1
			{													//	DAI=������ٶȡ�����뾶/������ٶ�   ����������ٶ�=������ٶȡ�����뾶
																//State.Work.MainDSpeed:����תһȦ��ʱ�䣬���ٶȵĵ���
																//State.Work.RollSpeed:����תһȦ��ʱ�䣬���ٶȵĵ���
																//  Dai/State.Work.RollSpeed=Param.Dai.MainD/State.Work.MainDSpeed
																//  Dai=��Param.Dai.MainD/State.Work.MainDSpeed����State.Work.RollSpeed
																//	   =(State.Work.RollSpeed/State.Work.MainDSpeed)*Param.Dai.MainD 
				if(DiaTiems<10000)	   //50�����һ��
				{
				  	DiaTiems++;
				}
				else
				{
				 	DiaTiems=0;		
					  //�ȼ���������ٶ�	 ,50��ת��Ȧ��
					if(Param.Basic.MainSpeedWay==3)	  			//   ���ٶ�������3�������������ź�
					{
						if(Param.Dai.RollMWay==1)	   // M������������IO��5
						{
							State.Work.MainDSpeed=((State.Work.MainDT-State.Work.MainDS)*10);	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
							State.Work.MainDS= State.Work.MainDT;
						}
						else if(Param.Dai.RollMWay==2)	   // M������������  2����������
						{
					   		//State.Work.MainDSpeed=  ÿ��ת�������� *10

							State.Work.MainDSpeed=(State.Work.MainDSpeed*50)/ Param.Dai.MainD;
								//���ٶȣ�ÿ1�����һ�Σ����50�����һ�Σ�����Ҫ*50
						}
					}
					else
					{
						if(Param.Dai.RollMWay==1)	   // M������������IO��5
						{
							State.Work.MainDSpeed=((State.Work.MainDT-State.Work.MainDS)*10)/Param.Dai.MainD;	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
							State.Work.MainDS= State.Work.MainDT;
						}
						else if(Param.Dai.RollMWay==2)	   // M������������  2����������
						{
	
							if((* BSP_ENC_CNT)>=0)
							{
								State.Work.MainDT= 	(* BSP_ENC_CNT)/Param.Dai.MainD;
							}
							else
							{
								State.Work.MainDT=(0-(* BSP_ENC_CNT))/Param.Dai.MainD;
							}
							 //������Ȧ��
							State.Work.MainDSpeed=(State.Work.MainDT-State.Work.MainDS)*10;	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
							State.Work.MainDS= State.Work.MainDT;
						}
					}

					if(State.Basic.OutCh==0)			//A�����ģʽ
					{	
						if(Param.Dai.RollAWay==1)	   // A������������IO��6
						{						 		//����A����ٶ�	
							State.Work.RollDSpeed= ((State.Work.RollDT-State.Work.RollDS)*10)/Param.Dai.RollD ;
							State.Work.RollDS= State.Work.RollDT;
						}
						else if(Param.Dai.RollAWay==2)	   // A������������2����������
						{
							if((* BSP_ENC_CNT)>=0)
							{
								State.Work.RollDT= (* BSP_ENC_CNT)/Param.Dai.RollD;
							}
							else
							{
								State.Work.RollDT=(0-(* BSP_ENC_CNT))/Param.Dai.RollD;
							}
  						 	//A����Ȧ��
							State.Work.RollDSpeed=(State.Work.RollDT-State.Work.RollDS)*10;	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
							State.Work.RollDS= State.Work.RollDT;	   //DT  DS
						}
						LongData=State.Work.MainDSpeed*Param.Dai.Ratio;		//	 Param.Dai.Ratio ����ֱ��
						//���ٶ�=������ٶ�*  ����ֱ��
						State.Work.Dia = LongData/State.Work.RollDSpeed;
						//�� ��=���ٶ�/������ٶ�
						LongData=State.Work.Dia*1000;

						State.Work.DiaPer=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);

					}
					else if(State.Basic.OutCh==1)			//B�����ģʽ
					{							 		//����B����ٶ�	
						if(Param.Dai.RollBWay==1)	   // A������������IO��6
						{
							State.Work.RollD2Speed= ((State.Work.RollDT2-State.Work.RollDS2)*10)/Param.Dai.RollD ;
							State.Work.RollDS2= State.Work.RollDT2;
						}
						else  if(Param.Dai.RollBWay==2)		  // B������������2����������
						{
 							if((* BSP_ENC_CNT)>=0)
							{
								State.Work.RollDT2= (* BSP_ENC_CNT)/Param.Dai.RollD;
							}
							else
							{
								State.Work.RollDT2= (0-(* BSP_ENC_CNT))/Param.Dai.RollD;
							}
  						 	//B����Ȧ��
							State.Work.RollD2Speed=(State.Work.RollDT2-State.Work.RollDS2)*10;	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
							State.Work.RollDS2= State.Work.RollDT2;
						}
						LongData=State.Work.MainDSpeed*Param.Dai.Ratio;		//	 Param.Dai.Ratio ����ֱ��
						//���ٶ�=������ٶ�*  ����ֱ��
						State.Work.Dia2 = LongData/State.Work.RollD2Speed;
						//�� ��=���ٶ�/������ٶ�
						LongData=State.Work.Dia2*1000;

						State.Work.DiaPer2=LongData/(Param.Dai.MostDia-Param.Dai.LeastDia);
					}
				}
         
			}
			else if(Param.Dai.Way==4)		//��ֵ��2   ģ������ֵ��  //��PID�ĳ������Ѿ�����
			{													//	DAI=������ٶȡ�����뾶/������ٶ�   ����������ٶ�=������ٶȡ�����뾶
																//State.Work.MSpeed:������ٰٷֱ�
																//State.Work.RSpeed:������ٰٷֱ�
																//  Dai=��State.Work.MSpeed/State.Work.RSpeed����State.Work.RollSpeed
																//	   =(State.Work.MSpeed/State.Work.MainDSpeed)*State.Work.RSpeed
			}
			else if(Param.Dai.Way==5)		//5�����Գ�����������  �ݲ�֧��
			{
			}
			else if(Param.Dai.Way==6)	   //6��6: ����ͨ��  KernelRegs[64]��  ͨ��������������ʵ��
			{
			}
//////////////////////////////׶�ȼ���///////////////////////////////////////////////
			if(Param.Basic.MachMode==1)				//�ž�׶��    ��׶�� ����������
			{
				Pid.SetD=Param.Auto.SetAim;
	//			Param.Basic.ZDLine=0;
			}
			else if(Param.Basic.MachMode==2)			//�վ�׶��  ����Խ��ԽС			
			{
				if(Param.Basic.ZDLine==0)			//����������
				{
					Pid.SetD=Param.Auto.SetAim;					
				}
				if(Param.Basic.ZDLine==1)			//ֱ��׶��
				{
					if(State.Basic.OutCh==0)			//A�����ģʽ	
					{
					 	LongData=State.Work.DiaPer;
					}
					else if(State.Basic.OutCh==1)			//B�����ģʽ	
					{
						LongData=State.Work.DiaPer2;
					}

					if(LongData<=100)		//0~10%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ10+((100-Param.ZDZ.ZDZ10)*(100-LongData))/100))/100;
					}
					else if(LongData<=200)		//10~20%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ20+((Param.ZDZ.ZDZ10-Param.ZDZ.ZDZ20)*(200-LongData))/100))/100;
					}
					else if(LongData<=300)		//20~30%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ30+((Param.ZDZ.ZDZ20-Param.ZDZ.ZDZ30)*(300-LongData))/100))/100;
					}
					else if(LongData<=400)		//30~40%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ40+((Param.ZDZ.ZDZ30-Param.ZDZ.ZDZ40)*(400-LongData))/100))/100;
					}
					else if(LongData<=500)		//40~50%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ50+((Param.ZDZ.ZDZ40-Param.ZDZ.ZDZ50)*(400-LongData))/100))/100;
					}
					else if(LongData<=600)		//50~60%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ60+((Param.ZDZ.ZDZ50-Param.ZDZ.ZDZ60)*(400-LongData))/100))/100;
					}
					else if(LongData<=700)		//60~70%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ70+((Param.ZDZ.ZDZ60-Param.ZDZ.ZDZ70)*(400-LongData))/100))/100;
					}
					else if(LongData<=800)		//70~80%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ80+((Param.ZDZ.ZDZ70-Param.ZDZ.ZDZ80)*(400-LongData))/100))/100;
					} 
					else if(LongData<=900)		//80~90%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ90+((Param.ZDZ.ZDZ80-Param.ZDZ.ZDZ90)*(400-LongData))/100))/100;
					}
					else if(LongData<=1000)		//90~100%����
					{
						Pid.SetD=(Param.Auto.SetAim*(Param.ZDZ.ZDZ100+((Param.ZDZ.ZDZ90-Param.ZDZ.ZDZ100)*(400-LongData))/100))/100;
					}
					else
					{
						Pid.SetD=( Param.Auto.SetAim*Param.ZDZ.ZDZ100)/100;
					}
				}
			}
			else if(Param.Basic.MachMode==3)			//ǣ��   ����������
			{
				Pid.SetD=Param.Auto.SetAim;
			}
////////////////////////////////////////////////////////////////////////////////////

			if(State.Action.StopFlg==2)
			{
  				State.Action.StopFlg=0;
//				PID_StateSave();	  //ͣ���󣬱���PID����
			}
			if(State.Out.Mode==0)						////����Ԥ���ģʽ
			{
				OUTDATA[9]=0;
				if(State.Basic.OutCh==0)  //A��
				{
					if(JogTimes>0)
					{
						LongData=10000;
						LongData=Param.Auto.JogOut*LongData;
						LongData=LongData/100;
					 	OUTDATA[9]=JogTimes*LongData/1000;
						JogTimes-- ;
						if(JogTimes>0)
						{
							JogTimes-- ;
						}
						if(JogTimes>0)
						{
							JogTimes-- ;
						}
					}
				}			
			}
			else if(State.Out.Mode==1)						////�Զ��������ģʽ
			{
				if(State.Basic.PIDCycFLG==1)			//�ѵ�PID��������
				{
					if(State.Basic.Run==2)			//����PID
					{
						OUTDATA[9]=PIDOut(State.Work.Tens);
					}
					else if(State.Basic.Run==3)			//ͣ��PID	  Ť��
					{
						LongData=State.Out.Data;
						LongData=LongData*Param.Auto.StopOut;
						LongData=LongData/1000;
						if(State.Basic.PidInit==0)
						{
							InitPIDOut();
							State.Basic.PidInit=1;
						}
						LongData=PIDOut(State.Work.Tens);
						OUTDATA[9]=LongData;
					}
				}
			}
			else if(State.Out.Mode==2)				//�ֶ����ģʽ
			{
			 	if((KernelRegs[77]>1000)&&(KernelRegs[77]<=2000))
				{
					OUTDATA[9]=	KernelRegs[77]*10;
				}
			}
			else if(State.Out.Mode==3)				//����Ԥ���ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.OpenOut*LongData;		//�������
				LongData=LongData/100;
				LongData=Param.Auto.StartOut*LongData;		//��������������������������
				LongData=LongData/100;
				OUTDATA[9]=LongData;
			}
/*			else if(State.Out.Mode==4)				//ͣ��Ԥ���ģʽ
			{
			}
*/
			else if(State.Out.Mode==5)				//�㶯���ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.JogOut*LongData;
				LongData=LongData/100;
				if(JogTimes<=1)		  //5ms*1=5ms, ��500ms���ڣ�����
				{
				 	LongData=LongData/10;
					JogTimes++;
				}
				else if(JogTimes<1000)		  //5ms*100=500ms, ��500ms���ڣ�����
				{
					if(JogTimes<(Param.Auto.JogOut*9))
					{
				 		LongData=LongData/10+JogTimes*10;
					}
					JogTimes++;
				}
				else
				{
					LongData=LongData;
				}

				OUTDATA[9]=LongData;
			}
			else if(State.Out.Mode==6)				//�������ģʽ
			{								   //13632765713  13714858621
				LongData=10000;
				LongData=Param.Auto.ChangeOut*LongData;
				LongData=LongData/100;
				OUTDATA[9]=LongData;
			}
			else if(State.Out.Mode==7)				//�����ƶ����ģʽ
			{
				OUTDATA[9]=OUTDATA[8];
			}
			else if(State.Out.Mode==8)				//�������ģʽ
			{
				if(JogTimes>0)
				{
				 	JogTimes=0;
				}
				LongData=10000;
				LongData=Param.Auto.OpenOut*LongData;
				LongData=LongData/100;
				OUTDATA[9]=LongData;
			}	
			else if(State.Out.Mode==9)				//�������ģʽ
			{
				if(JogTimes>0)
				{
				 	JogTimes=0;
				}
			}
			else if(State.Out.Mode==10)				////�������ģʽ
			{
				OUTDATA[9]=OUTDATA[8];
			}	

//**************************************************************************
			if(State.Out.Mode2==0)				////�����ģʽ
			{
				OUTDATB[9]=0;
				if(State.Basic.OutCh==1)  //B��
				{
					if(JogTimes>0)
					{
						LongData=10000;
						LongData=Param.Auto.JogOut*LongData;
						LongData=LongData/100;
					 	OUTDATA[9]=JogTimes*LongData/1000;
						JogTimes-- ;
						if(JogTimes>0)
						{
							JogTimes-- ;
						}
						if(JogTimes>0)
						{
							JogTimes-- ;
						}
					}
				}
			}
			else if(State.Out.Mode2==1)				////PID���ģʽ
			{
				if(State.Basic.PIDCycFLG==1)			//�ѵ�PID��������
				{
					if(State.Basic.Run==2)			//����PID
					{
						OUTDATB[9]=PIDOut(State.Work.Tens);
					}
					else if(State.Basic.Run==3)			//ͣ��PID
					{
						LongData=State.Out.Data2;
						LongData=LongData*Param.Auto.StopOut;
						LongData=LongData/1000;
						if(State.Basic.PidInit==0)
						{
							InitPIDOut();
							State.Basic.PidInit=1;
						}
						LongData=PIDOut(State.Work.Tens);
						OUTDATB[9]=LongData;
					}
				}
			}
			else if(State.Out.Mode2==2)				//�ֶ����ģʽ
			{
			 	if((KernelRegs[78]>1000)&&(KernelRegs[78]<=2000))
				{
					OUTDATB[9]=	KernelRegs[78]*10;
				}
			}
			else if(State.Out.Mode2==3)				//����Ԥ���ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.OpenOut*LongData;		//�������
				LongData=LongData/100;
				LongData=Param.Auto.StartOut*LongData;		//��������������������������
				LongData=LongData/100;
				OUTDATB[9]=LongData;
			}
/*
			else if(State.Out.Mode2==4)				//ͣ��Ԥ���ģʽ
			{
			}
*/
			else if(State.Out.Mode2==5)				//�㶯���ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.JogOut*LongData;
				LongData=LongData/100;
				if(JogTimes<=1)		  //5ms*1=5ms, ��500ms���ڣ�����
				{
				 	LongData=LongData/10;
					JogTimes++;
				}
				else if(JogTimes<1000)		  //5ms*100=500ms, ��500ms���ڣ�����
				{
					if(JogTimes<(Param.Auto.JogOut*9))
					{
				 		LongData=LongData/10+JogTimes*10;
					}
					JogTimes++;
				}
				else
				{
					LongData=LongData;
				}
				OUTDATB[9]=LongData;
			}
			else if(State.Out.Mode2==6)				//�������ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.ChangeOut*LongData;
				LongData=LongData/100;
		
				OUTDATB[9]=LongData;
			}
			else if(State.Out.Mode2==7)				//�����ƶ����ģʽ
			{
				OUTDATB[9]=OUTDATB[8];
			}
			else if(State.Out.Mode2==8)				//�������ģʽ
			{
				LongData=10000;
				LongData=Param.Auto.OpenOut*LongData;
				LongData=LongData/100;
				OUTDATB[9]=LongData;
			}
	
			else if(State.Out.Mode2==9)				//�������ģʽ
			{
			}
			else if(State.Out.Mode2==10)				//�������ģʽ
			{
				OUTDATB[9]=OUTDATB[8];
			}

			State.Out.Data=OutFilter(1);			//����˲�

			State.Out.Per=State.Out.Data/100;			//����ٷֱ�
	
			State.Out.Data2=OutFilter(2);			//����˲�

			State.Out.Per2=LongData/100;			//����ٷֱ�
//			if((OldOutA!=State.Out.Data)||(State.Out.Data2!=OldOutB))//���ֵ�иı�
			{
				if(State.Basic.Out==1)		  //�������
				{
					OldOutA=State.Out.Data;
					OldOutB=State.Out.Data2;
				}
				else
				{
					OldOutA=0;
					OldOutB=0;
				}
	//			OutPut();					//�������
			}
			State.Basic.PIDCycFLG=0;
		}
		if(State.Basic.OverFLG==0)
		{
			if((State.Work.Tens>0)&(Param.Basic.OverTest>0))
			{
				if(State.Work.Tens>Param.Basic.OverTest)			//���������	
				{
					if(State.Action.OverFLG==0)
					{
						State.Action.OverFLG=1;
						State.Action.Over=0;
					}
				}
			}
		}
		else if(State.Basic.OverFLG==1)						//	//������ʱ
		{
			if(State.Work.Tens>0)
			{
				if(State.Work.Tens<Param.Basic.OverTest)			//�����������	
				{
					if(State.Action.OverFLG==0)
					{
						State.Action.OverFLG=2;
						State.Action.Over=0;
					}
				}
			}
			else
			{
				if(Param.Basic.OverTest>0)
				{
					if(State.Action.OverFLG==0)
					{
						State.Action.OverFLG=2;
						State.Action.Over=0;
					}
				}
				else if(State.Work.Tens>Param.Basic.OverTest)
				{
					if(State.Action.OverFLG==0)
					{
						State.Action.OverFLG=2;
						State.Action.Over=0;
					}
				}
			}
		}
//***************************Ƿ�������****************************************************************
		if(State.Basic.Run!=0)		  //����״̬
		{
	   		RunTimes++;
		}
		else
		{
		 	RunTimes=0;
		}
		if((RunTimes/200)>KernelRegs[5])		  //30����ټ��
		{
			if(State.Basic.ZeroFLG==0)
			{
				if((State.Work.Tens>0)&&(Param.Basic.ZeroTest>0))
				{
				 	if(State.Work.Tens<Param.Basic.ZeroTest)		//Ƿ�������
					{
						if(State.Action.ZeroFLG==0)
						{
							State.Action.ZeroFLG=1;
							State.Action.Zero=0;
						}
					}
				}
				else if((State.Work.Tens<0)&(Param.Basic.ZeroTest>=0))
				{
					if(State.Action.ZeroFLG==0)
					{
						State.Action.ZeroFLG=1;
						State.Action.Zero=0;
					}
				}
			}
			else if(State.Basic.ZeroFLG==1)
			{
				if((State.Work.Tens>0)&(Param.Basic.ZeroTest>=0))
				{
				 	if(State.Work.Tens>Param.Basic.ZeroTest)		//Ƿ�������
					{
						if(State.Action.ZeroFLG==0)
						{
							State.Action.ZeroFLG=2;
							State.Action.Zero=0;
						}
					}
				}
			}
		}
//**************************************************************************
		if((State.Basic.OverFLG==1)||(State.Basic.ZeroFLG==1))
		{
			if(State.Basic.Alarm==0)
			{
				State.Basic.Alarm=1;
//				LedOn(7);
			}
		}
		else
		{
			if(State.Basic.Alarm==1)
			{
				State.Basic.Alarm=0;
//				LedOff(7);
			}
		}
			if(State.Basic.OverFLG==1)				//������
			{
//					DOUT1=0;						//�˿�SO1���
				GPIO_SetBits(GPIOA, GPIO_Pin_0);	//�̵���1���
			}
			else
			{
				 GPIO_ResetBits(GPIOA, GPIO_Pin_0);		 //�̵���1�����,�Ͽ�
			}
			if(State.Basic.ZeroFLG==1)         //Ƿ����
			{
 				GPIO_SetBits(GPIOA, GPIO_Pin_1);	//�̵���2���
			}
			else
			{
 				 GPIO_ResetBits(GPIOA, GPIO_Pin_1);		 //�̵���2�����,�Ͽ�
			}
			if(Leda<=100)
			{
				
				Leda++;
			}

			else
			{
			 	Leda=0;
				if(cc==0)
				{
//				  	GPIO_SetBits(GPIOA, GPIO_Pin_0);
 					XTLedOn;
				 	cc++;
				}
				else
				{
//				 	GPIO_ResetBits(GPIOA, GPIO_Pin_0);	
					XTLedOff;
				  	cc=0;
				}
			}
			if(MainSpeedTimes<=19)		   //100mS��
			{
			  	MainSpeedTimes++;
			}
			else
			{
				MainSpeedTimes=0;
			   	if(Param.Basic.MainSpeedWay==3)	  			//   ���ٶ�������3�������������ź�
				{	
	//				if(Param.Dai.Way!=3)		//��ֵ��1	  �����Ѵ���
					{
	  					if(Param.Dai.RollMWay==1)	   // M������������IO��5
						{
								//������ IO��5 �� Ч
						}
						else if(Param.Dai.RollMWay==2)	   // M������������  2����������
						{
	
							if((* BSP_ENC_CNT)>=0)
							{
								State.Work.MainDT= 	(* BSP_ENC_CNT);
							}
							else
							{
								State.Work.MainDT=(0-(* BSP_ENC_CNT));
							}
							 //������Ȧ��
							 if( State.Work.MainDT>=State.Work.MainDS)
							 {
								State.Work.MainDSpeed=(State.Work.MainDT-State.Work.MainDS)*100;	 //   *10��Ϊ��ȥС������ͬ Param.Dai.MainD����������
 								State.Work.MainDS= State.Work.MainDT;									//�ٳ���10����Ϊ100ms���� KernelRegs[15]��λ��1/10
							}
							else
							{
								State.Work.MainDS= State.Work.MainDT;
								State.Work.MainDSpeed=0;
							}
							
						}	//State.Work.MainDSpeed=  ÿ��ת�������� ��*10����10��
						LongData=(State.Work.MainDSpeed*1000);		 //	 State.Work.MSpeed  ���ٶ�  0��10000 ��������*100������ֻ���ٳ�1000
						State.Work.MSpeed=LongData/KernelRegs[15];
					}
				}
			}
	}
