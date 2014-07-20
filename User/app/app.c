

#include <includes.h>	/* ���ļ����������б����.h�ļ� */
#include <stdio.h>
#include <stdio.h>
#include "bsp_232c.h"
#include "bsp_485.h"
#include "bsp_usb.h"
#include "bsp_eeprom.h"
#include "bsp_dac.h"
#include "bsp_GPIO.h"
#include "bsp_sonic.h"
#include "bsp_enc.h"
#include "bsp_adc.h"
#include "bsp_TimesInt.h"
#include "bsp_extsram.h"
#include "app_kernelregs.h"
#include "app_usbcom.h"
#include "app_backend.h"
#include "app_usbgather.h"
#include "app_usbmodbusmaster.h"
#include "app_usbmodbusslave.h"
#include "app_485com.h"
#include "app_485modbusslave.h"
#include "app_OutControl.h"
#include "app_inout.h"
#include "app_action.h"
#include "app_pid.h"
#include "app_Port.h"
#include "app_encryption.h"
#include <math.h>

/*
*********************************************************************************************************
*                                       ģ����ȫ�ֱ���
*********************************************************************************************************
*/

/* ����ÿ������Ķ�ջ�ռ䣬app_cfg.h�ļ��к궨��ջ��С */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTask232CStk[APP_TASK_232C_STK_SIZE];
static OS_STK AppTaskUSBCOMStk[APP_TASK_USBCOM_STK_SIZE];
static OS_STK AppTaskUSBGatherStk[APP_TASK_USBGATHER_STK_SIZE];
static OS_STK AppTaskHeartBeatStk[APP_TASK_HEARTBEAT_STK_SIZE];
static OS_STK AppTaskServotimerStk[APP_TASK_SERVOTIMER_STK_SIZE];
static OS_STK AppTaskServoStk[APP_TASK_SERVO_STK_SIZE];

/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTask232C(void *p_arg);

void printint(int num);
u32 myplusecnt = 0;

OS_EVENT *Servotimermsg; //�ŷ�����֪ͨ

double beginspeed = 0.0;

struct tptp{
	u8 ptptype;//0 �� 1 5��	2 4�� 3 7�� 4 6��
	double t0,t1,t2,t3,t4,t5,t6,tv;
	double ahis,vhis,acur,vcur;
	double J,V,S;
	int Ta,Ts;
};

struct tjog{
	u8 jogtype;//0 �� 1 2��	2 3��
	double t0,t1,t2,t3,t4,t5,t6,tv;//tv�Ǳ����õı���
	double ahis,vhis,acur,vcur;
	double J,V,S;
	int Ta,Ts;
	u8 jogstate;//jog״̬�� 0�� 1���ٽ׶� 2���ٽ׶� 3���ٽ׶�
};

struct tmotor{
	u8 status;//0���� 1ptp 2jog 3stop 4gear 5gearptp 6gearjog 7gearstop
	struct tptp ptpdata;
	struct tjog jogdata;
	//OS_EVENT *ServoMsg; //�ŷ����ڴ���֪ͨ
}motor;


//���е�ֹͣ������jogstop��������ʱ����Լ��ٶ�����ֹͣ�㷨��ֻ����ٶ�����
//ֹͣ�㷨ֻ����ʱ����ֹͣ��������עָֹͣ����λ��
u8 jogstop(int Ta,int Ts,double J)
{
	double V = 0;
	if(motor.status == 1)
	{
		V =  motor.ptpdata.vcur;
	}
	else if(motor.status == 2)
	{
		V =  motor.jogdata.vcur;
	}
	else
	{
		return 1;
	}
	 
	 {
		 //�������߹滮����,V0,S0�ȶ���ָֻ��Ts��4���������
		 double A = J * Ts;
		 double V0 = A * Ts;
		 double S0 = 2*V0*Ts;
	
		 //�ٶȲ��������ٽ׶�ֻ��Ts
		 if(V < V0)
		 {
		 	double Tsnew = sqrt(V/J);
			double S0new = 2*V*Tsnew;
	
			{	
				//��2�ηֱ����������ÿ���ŷ�ʱ����v
				motor.jogdata.vhis = V;
				motor.jogdata.ahis = 0;
	
				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Tsnew;
				motor.jogdata.t1 = Tsnew;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 1;
				motor.status = 2;//jog״̬
			}
		 }
		 //�ٶ��㹻�Ĵ�,���ٺͼ��ٽ׶���Ta�׶�
		 else
		 {
		 	double S0new = V*Ta;
			{
				motor.jogdata.vhis = V;
				motor.jogdata.ahis = 0;

				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Ts;
				motor.jogdata.t1 = (V-V0)/A;
				motor.jogdata.t2 = Ts;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 2;
				motor.status = 2;//jog״̬
			}
		 }
		motor.jogdata.jogstate = 3;//ֹͣJOG�˶�
	 }
	 return 0;
}



//�ж�ָ��֮���ƶ��̶�λ�õ�ֹͣjogstopins��������ʱ����Լ��ٶ�����ֹͣ�㷨��ֻ����ٶ�����
u8 jogstopins(int Ta,int Ts,double J,double S)
{
	double V = 0;
	//ȡ�õ�ǰ�ٶ�
	if(motor.status == 1)
	{
		V =  motor.ptpdata.vcur;
	}
	else if(motor.status == 2)
	{
		V =  motor.jogdata.vcur;
	}
	else
	{
		return 1;
	}
	 
	 //���߹滮
	 {

		 //�������߹滮����,V0,S0�ȶ���ָֻ��Ts��4���������
		 double A = J * Ts;
		 double V0 = A * Ts;
		 double S0 = 2*V0*Ts;
	
		 //�ٶȲ��������ٽ׶�ֻ��Ts����Ts���޸�
		 if(V < V0)
		 {
		 	double Tsnew = sqrt(V/J);
			double S0new = V*Tsnew;
	
			//5��S����,�����ٽ׶�
			if(S0new <= S)
			{	
				//��5�ηֱ����������ÿ���ŷ�ʱ����v
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;
	
				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Tsnew;
				motor.jogdata.t1 = Tsnew;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 1;
				motor.status = 2;//jog״̬
			}
			else if(S0new > S)//4������,���벻�����ﲻ������ٶȣ�TsnewҪ�ı�
			{
	 			//��4�ηֱ����������ÿ���ŷ�ʱ����v
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;
	
				Tsnew = pow((S/(J)),1.0/3);
	
				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Tsnew;
				motor.jogdata.t1 = Tsnew;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 1;
				motor.status = 2;//jog״̬
			}
		 }
		 else if((V>= V0) && (S< S0))//4�����ߣ��ٶȹ����벻����TsҪ�޸������ٽ׶�
		 {
	 	 	double Tsnew;
	
			//��4�ηֱ����������ÿ���ŷ�ʱ����v
			motor.jogdata.vhis = 0;
			motor.jogdata.ahis = 0;
	
			Tsnew = pow((S/(J)),1.0/3);
	
			//ÿ��ʱ�䳤�ȷֱ�Ϊ
			motor.jogdata.t0 = Tsnew;
			motor.jogdata.t1 = Tsnew;

			motor.jogdata.tv = 0;
			motor.jogdata.jogtype = 1;
			motor.status = 2;//jog״̬
		 }
		 //�ٶȺ�·�̶��㹻�Ĵ�,���ٺͼ��ٽ׶���Ta�׶�,7���ٶ�����
		 else if((V >= V0) && (S >= S0))
		 {
		 	double S0new = V*Ta;
		 	if(S0new <= S)//���빻�����ԴﵽV,��7��
			{
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;

				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Ts;
				motor.jogdata.t1 = (V-V0)/A;
				motor.jogdata.t2 = Ts;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 2;
				motor.status = 2;//jog״̬
			}
			//�ﲻ���ٶ�V��ֻ��6��,Ta����Ҫ�ı䣬�����ٽ׶�
			else
			{
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;

				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Ts;
				motor.jogdata.t1 = sqrt(S/(J*Ts) + Ts*Ts/4 )- 1.5*Ts;
				motor.jogdata.t2 = Ts;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 2;
				motor.status = 2;//jog״̬
			}
		 }
		motor.jogdata.jogstate = 3;//ֹͣJOG�˶�
	 }
	 return 0;
}

//�㶯
u8 jog(int T,double V)
{
	if(motor.status != 0)
		return 1;
	 
	 //jog״̬��
	 {

		 //�������߹滮����,V0,S0�ȶ���ָֻ��Ts��4���������
		 double A = J * Ts;
		 double V0 = A * Ts;
		 double S0 = 2*V0*Ts;
	
		 //�ٶȲ��������ٽ׶�ֻ��Ts
		 if(V < V0)
		 {
		 	double Tsnew = sqrt(V/J);
			double S0new = 2*V*Tsnew;
	
			{	
				//��5�ηֱ����������ÿ���ŷ�ʱ����v
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;
	
				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Tsnew;
				motor.jogdata.t1 = Tsnew;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 1;
				motor.status = 2;//jog״̬
			}
		 }
		 //�ٶ��㹻�Ĵ�,���ٺͼ��ٽ׶���Ta�׶�
		 else
		 {
		 	double S0new = V*Ta;
			{
				motor.jogdata.vhis = 0;
				motor.jogdata.ahis = 0;

				//ÿ��ʱ�䳤�ȷֱ�Ϊ
				motor.jogdata.t0 = Ts;
				motor.jogdata.t1 = (V-V0)/A;
				motor.jogdata.t2 = Ts;

				motor.jogdata.tv = 0;
				motor.jogdata.jogtype = 2;
				motor.status = 2;//jog״̬
			}
		 }
		motor.jogdata.jogstate = 1;//����JOG
	 }
	 return 0;
}

//�㵽���ƶ�
u8 ptp(int T,double V,double S)
{	 
	if(motor.status != 0)
		return 1;

	motor.ptpdata.J = 0;
	motor.ptpdata.V = V;
	motor.ptpdata.S = S;
	 //ptp״̬��
	 {

		 //�������߹滮����,V0,S0�ȶ���ָֻ��Ts��4���������
		int Ts1 = floor(T/2);
		double S0 = 2*V*Ts1;
		double V0 = V;
		int Ty = 0;
		int Tss = 0;

		//4������,�޷��ﵽ�����V		
		if(S0 >=S)
		{
			motor.ptpdata.vhis = 0;
			motor.ptpdata.ahis = 0;

			V0 = S/(2*Ts1);
			motor.ptpdata.J = V0/(Ts1*Ts1);
			motor.ptpdata.t0 = Ts1;
			motor.ptpdata.t1 = Ts1;
			motor.ptpdata.t2 = Ts1;
			motor.ptpdata.t3 = Ts1;
			
			//��ʼ
			motor.ptpdata.tv = 0;
			motor.ptpdata.ptptype = 2;
			motor.status = 1;//ptp״̬
		 }
		 else //���Դﵽ�����V,��5�ηֱ�������߹滮
		 {
			motor.ptpdata.vhis = 0;
			motor.ptpdata.ahis = 0;

			//���ٶ�ʱ�䣬ȡ��
			Ty = floor((S-S0)/V);
			
			//Ϊ�˱���ʱ��������ȡ��������V,�����ָ���V������
			V0 = S/(2*Ts1 + Ty);

			motor.ptpdata.V = V0;
			motor.ptpdata.J = V0/(Ts1*Ts1);;
			motor.ptpdata.t0 = Ts1;
			motor.ptpdata.t1 = Ts1;
			motor.ptpdata.t2 = Ty;
			motor.ptpdata.t3 = Ts1;
			motor.ptpdata.t4 = Ts1;

			//��ʼ
			motor.ptpdata.tv = 0;
			motor.ptpdata.ptptype = 1;
			motor.status = 1;//ptp״̬
		 }
	 }
	 
	 KernelRegs[90] = motor.ptpdata.t0*100;
	 KernelRegs[91] = motor.ptpdata.t1*100;
	 KernelRegs[92] = motor.ptpdata.t2*100;
	 KernelRegs[93] = motor.ptpdata.t3*100;
	 KernelRegs[94] = motor.ptpdata.t4*100;
	 KernelRegs[95] = motor.ptpdata.t5*100;
	 KernelRegs[96] = motor.ptpdata.t6*100;

	 return 0;
}


//����������
static void ServoTask(void *p_arg)
{
	u8 err;
	int cnt = 0;
	motor.status = 0;

	while (1) 
	{
		 //ptp״̬��
		 if(motor.status == 1)
		 {
	 		//��5�ηֱ����������ÿ���ŷ�ʱ����v
			if(motor.ptpdata.ptptype == 1)
			{
				//�������ߵ�ʱ�����
				{
				   //�Ӽ��ٽ׶�
				   if(motor.ptpdata.tv < motor.ptpdata.t0)
				   {
				   	//���ٶȸı�ֵ
					double achg = motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
	
				   }
				   //�����ٽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1)) && (motor.ptpdata.tv>= motor.ptpdata.t0))
				   {
				   	//���ٶȸı�ֵ
					double achg = 0-motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   }
				   //���ٽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2)) && (motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1)))
				   {
	
					MotorSetSpeed(0,motor.ptpdata.V);
	
					motor.ptpdata.vcur = motor.ptpdata.V;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
					motor.ptpdata.ahis = 0;
	
				   }
				   //�����ٶȽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3)) && (motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2)))
				   {
				   	//���ٶȸı�ֵ
					double achg = 0-motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   }
				   //�Ӽ��ٶȽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3+motor.ptpdata.t4)) && (motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3)))
				   {
				   	//���ٶȸı�ֵ
					double achg = motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   }
				   //ִ����ϣ���λ����ɺ������ٶ�Ϊ0���˴���ʱ����λ��������ֱ�������ٶ�
				   else	if(motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3+motor.ptpdata.t4))
				   {
					MotorSetSpeed(0,0);
	
				   	motor.ptpdata.vcur = 0;
				   	//�����ٶȵ�fpga
				   	motor.status = 0;
				   }
				   motor.ptpdata.tv++;
				}
	
			}
			else if(motor.ptpdata.ptptype == 2)
			{
				{
				   //�Ӽ��ٽ׶�
				   if(motor.ptpdata.tv < motor.ptpdata.t0)
				   {
				   	//���ٶȸı�ֵ
					double achg = motor.ptpdata.J*1;
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
	 				MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
	
				   }
				   //�����ٽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1)) && (motor.ptpdata.tv>= motor.ptpdata.t0))
				   {
				   	//���ٶȸı�ֵ
					double achg = 0-motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   }
				   //�����ٶȽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2)) && (motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1)))
				   {
				   	//���ٶȸı�ֵ
					double achg =0 -motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   	
				   }
				   //�Ӽ��ٶȽ׶�
				   else if((motor.ptpdata.tv<(motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3)) && (motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2)))
				   {
				   	//���ٶȸı�ֵ
					double achg = motor.ptpdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.ptpdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.ptpdata.acur = motor.ptpdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.ptpdata.vcur = motor.ptpdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.ptpdata.vcur);
	
					//������ʷֵ
					motor.ptpdata.ahis = motor.ptpdata.acur;
					motor.ptpdata.vhis = motor.ptpdata.vcur;
				   }
				   //ִ����ϣ���λ����ɺ������ٶ�Ϊ0���˴���ʱ����λ��������ֱ�������ٶ�
				   else	 if(motor.ptpdata.tv >= (motor.ptpdata.t0+motor.ptpdata.t1+motor.ptpdata.t2+motor.ptpdata.t3))
				   {
					MotorSetSpeed(0,0);
	
				   	motor.ptpdata.vcur = 0;
				   	//�����ٶȵ�fpga
				   	motor.status = 0;
				   }
				   motor.ptpdata.tv++;
				}
	
			}
		 }
		 //jog״̬��
		 else if(motor.status == 2)
		 {
			//JOG�����׶�
			if(motor.jogdata.jogstate == 1)
			{
		 		//��2�ηֱ����������ÿ���ŷ�ʱ����v
				if( motor.jogdata.jogtype == 1)
				{
					//�������ߵ�ʱ�����
					{
					   //�Ӽ��ٽ׶�
					   if(motor.jogdata.tv < motor.jogdata.t0)
					   {
					   	//���ٶȸı�ֵ
						double achg = motor.jogdata.J*1;
		
						//�ٶȸı�ֵ
						double vchg = motor.jogdata.ahis*1 + achg*1/2;
		
						//��ǰʱ�����ٶ�
					   	motor.jogdata.acur = motor.jogdata.ahis+achg;
		
		
						//��ǰʱ����ٶ�
						motor.jogdata.vcur = motor.jogdata.vhis+vchg;
		
						//�����ٶȵ�fpga
						MotorSetSpeed(0,motor.jogdata.vcur);
		
						//������ʷֵ
						motor.jogdata.ahis = motor.jogdata.acur;
						motor.jogdata.vhis = motor.jogdata.vcur;
						motor.ptpdata.tv++;
					   }
					   //�����ٽ׶�
					   else if((motor.jogdata.tv<(motor.jogdata.t0+motor.jogdata.t1)) && (motor.jogdata.tv>= motor.jogdata.t0))
					   {
					   	//���ٶȸı�ֵ
						double achg = 0-motor.jogdata.J*1;
		
						//�ٶȸı�ֵ
						double vchg = motor.jogdata.ahis*1 + achg*1/2;
		
						//��ǰʱ�����ٶ�
					   	motor.jogdata.acur = motor.jogdata.ahis+achg;
		
						//��ǰʱ����ٶ�
						motor.jogdata.vcur = motor.jogdata.vhis+vchg;
		
						//�����ٶȵ�fpga
						MotorSetSpeed(0,motor.jogdata.vcur);
		
						//������ʷֵ
						motor.jogdata.ahis = motor.jogdata.acur;
						motor.jogdata.vhis = motor.jogdata.vcur;
						motor.ptpdata.tv++;
					   }
					   else if(motor.jogdata.tv>=(motor.jogdata.t0+motor.jogdata.t1))
					   {
						//��ǰʱ����ٶ�
						motor.jogdata.vcur = motor.jogdata.V;
						motor.jogdata.jogstate = 2;
						//�����ٶȵ�fpga
						MotorSetSpeed(0,motor.jogdata.vcur);
						motor.jogdata.vhis = motor.jogdata.vcur;
						motor.ptpdata.tv++;
					   }
					}
				}
			}
			//���ٽ׶�
			else if(motor.jogdata.jogstate == 2)
			{
				//��ǰʱ����ٶ�
				motor.jogdata.vcur = motor.jogdata.V;
				//�����ٶȵ�fpga
				MotorSetSpeed(0,motor.jogdata.vcur);
				motor.jogdata.vhis = motor.jogdata.vcur;
			}
			//���ٽ׶�
			else if(motor.jogdata.jogstate == 3)
			{
	
		 		//��2�ηֱ����������ÿ���ŷ�ʱ����v
				if( motor.jogdata.jogtype == 1)
				{
				   //�����ٶȽ׶�
				   if((motor.jogdata.tv<(motor.jogdata.t0)) && (motor.jogdata.tv >= (0)))
				   {
				   	//���ٶȸı�ֵ
					double achg = 0-motor.jogdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.jogdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.jogdata.acur = motor.jogdata.ahis+achg;
	
					//��ǰʱ����ٶ�
					motor.jogdata.vcur = motor.jogdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.jogdata.vcur);
	
					//������ʷֵ
					motor.jogdata.ahis = motor.jogdata.acur;
					motor.jogdata.vhis = motor.jogdata.vcur;
				   }
				   //�Ӽ��ٶȽ׶�
				   else if((motor.jogdata.tv<(motor.jogdata.t0+motor.jogdata.t1)) && (motor.ptpdata.tv >= (motor.ptpdata.t0)))
				   {
				   	//���ٶȸı�ֵ
					double achg = motor.jogdata.J*1;
	
					//�ٶȸı�ֵ
					double vchg = motor.jogdata.ahis*1 + achg*1/2;
	
					//��ǰʱ�����ٶ�
				   	motor.jogdata.acur = motor.jogdata.ahis+achg;
	
	
					//��ǰʱ����ٶ�
					motor.jogdata.vcur = motor.jogdata.vhis+vchg;
	
					//�����ٶȵ�fpga
					MotorSetSpeed(0,motor.jogdata.vcur);
	
					//������ʷֵ
					motor.jogdata.ahis = motor.jogdata.acur;
					motor.jogdata.vhis = motor.jogdata.vcur;
				   }
				   else if(motor.jogdata.tv>=(motor.jogdata.t0+motor.jogdata.t1)) 
				   {
					MotorSetSpeed(0,0);
	
				   	motor.ptpdata.vcur = 0;
				   	//�����ٶȵ�fpga
				   	motor.status = 0;
				   }
				}
			}
		}
		//���ʹ��
	 	MotorPwmout(0xff);
		//�ٶȸı�
		MotorSpeedFlush(0xff);

		//�ȴ��ŷ���ʱ��֪ͨ,����ͬ������
   		OSSemPend(Servotimermsg,0,&err);
		//servo�����л�
		Motorpingpong();

		//֪ͨˢ�����е����λ��
		MotorOutCntFlush(0xff);

		//����ĵ�ǰλ��
		cnt = MotorReadOutCnt(0);
		KernelRegs[81] = cnt & 0x0000ffff;
		KernelRegs[82] = (cnt >> 16) & 0x0000ffff;

		cnt = MotorReadOutCnt(1);
		KernelRegs[83] = cnt & 0x0000ffff;
		KernelRegs[84] = (cnt >> 16) & 0x0000ffff;
		KernelRegs[86] = motor.ptpdata.vcur*1000;
 		//OSTimeDlyHMSM(0, 0, 0, 1);

	}
}

//�ŷ�������ʱ������
static void ServotimerTask(void *p_arg)
{
	//Servotimermsg = OSSemCreate(0);
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);
		OSSemPost(Servotimermsg);		//����֪ͨ
	}
}

/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
	CPU_INT08U  err;
	int i;
	//��fpga�������
	for(i = 0; i < 1000000; i ++);
	
	/* ��ʼ��"uC/OS-II"�ں� */

	OSInit();
 	SystemInit();
 	Servotimermsg = OSSemCreate(0);

	//BSP_232CInit();
	//BSP_232COpen();
	BSP_ExtSramInit();
 	BSP_USBInit();
	BSP_USBOpen();


	App_Backend_Init();
	USB_ModbusMaster_Init();
	
	/* ��ֹ���е��ж� */
	BSP_IntDisAll();


	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	OSTaskCreateExt(AppTaskStart,	/* ����������ָ�� */
                    (void *)0,		/* ���ݸ�����Ĳ��� */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* ָ������ջջ����ָ�� */
                    APP_TASK_START_PRIO,	/* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                    APP_TASK_START_PRIO,	/* ����ID��һ����������ȼ���ͬ */
                    (OS_STK *)&AppTaskStartStk[0],/* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                    APP_TASK_START_STK_SIZE, /* ����ջ��С */
                    (void *)0,	/* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                       ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
					/*  �������£�
						OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
						OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
						OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
					*/                  

	/* ָ����������ƣ����ڵ��ԡ���������ǿ�ѡ�� */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);

	/* ����������ϵͳ������Ȩ����uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
//	uint8_t i, j;

	(void)p_arg;   /* �����ڱ���������澯����������������κ�Ŀ����� */

	/* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
	BSP_Init();

	/* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ����
	ע�⣺�˺������ӳ�100ms�ٷ��� */
#if (OS_TASK_STAT_EN > 0)
	OSStatInit();
#endif

	/* ����Ӧ�ó�������� */
	AppTaskCreate();

	/* �������壬������һ����ѭ�� */
	while (1)     
	{
		OSTimeDlyHMSM(20, 0, 0, 0);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	CPU_INT08U      err;


	
	OSTaskCreateExt(AppTask232C,
                    (void *)0,
                    (OS_STK *)&AppTask232CStk[APP_TASK_232C_STK_SIZE - 1],
                    APP_TASK_232C_PRIO,
                    APP_TASK_232C_PRIO,
                    (OS_STK *)&AppTask232CStk[0],
                    APP_TASK_232C_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_232C_PRIO, APP_TASK_232C_NAME, &err);

	OSTaskCreateExt(ServotimerTask,
                    (void *)0,
                    (OS_STK *)&AppTaskServotimerStk[APP_TASK_SERVOTIMER_STK_SIZE - 1],
                    APP_TASK_SERVOTIMER_PRIO,
                    APP_TASK_SERVOTIMER_PRIO,
                    (OS_STK *)&AppTaskServotimerStk[0],
                    APP_TASK_SERVOTIMER_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_SERVOTIMER_PRIO, APP_TASK_SERVOTIMER_NAME, &err);

	OSTaskCreateExt(ServoTask,
                    (void *)0,
                    (OS_STK *)&AppTaskServoStk[APP_TASK_SERVO_STK_SIZE - 1],
                    APP_TASK_SERVO_PRIO,
                    APP_TASK_SERVO_PRIO,
                    (OS_STK *)&AppTaskServoStk[0],
                    APP_TASK_SERVO_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_SERVO_PRIO, APP_TASK_SERVO_NAME, &err);

	OSTaskCreateExt(App_HeartbeatTask_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskHeartBeatStk[APP_TASK_HEARTBEAT_STK_SIZE - 1],
                    APP_TASK_HEARTBEAT_PRIO,
                    APP_TASK_HEARTBEAT_PRIO,
                    (OS_STK *)&AppTaskHeartBeatStk[0],
                    APP_TASK_HEARTBEAT_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskNameSet(APP_TASK_HEARTBEAT_PRIO, APP_TASK_HEARTBEAT_NAME, &err);

	OSTaskCreateExt(USBCOM_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskUSBCOMStk[APP_TASK_USBCOM_STK_SIZE - 1],
                    APP_TASK_USBCOM_PRIO,
                    APP_TASK_USBCOM_PRIO,
                    (OS_STK *)&AppTaskUSBCOMStk[0],
                    APP_TASK_USBCOM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USBCOM_PRIO, APP_TASK_USBCOM_NAME, &err);

	OSTaskCreateExt(USB_Gather_Run,
                    (void *)0,
                    (OS_STK *)&AppTaskUSBGatherStk[APP_TASK_USBGATHER_STK_SIZE - 1],
                    APP_TASK_USBGATHER_PRIO,
                    APP_TASK_USBGATHER_PRIO,
                    (OS_STK *)&AppTaskUSBGatherStk[0],
                    APP_TASK_USBGATHER_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USBGATHER_PRIO, APP_TASK_USBGATHER_NAME, &err);

}



/*
*********************************************************************************************************
*	�� �� ��: AppTask232C
*	����˵��: ��������������������USER����WAKEUP����״̬��������Ϣ���͵�
*				�û���������AppTaskUserIF��
*				������������򵥵��˲����ܣ������ο���
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTask232C(void *p_arg)
{
	u16 oldreg79 = 0;	//��ʷ�Ĵ���79������λ
	u16 oldreg00 = 0;	//��ʷ�Ĵ���00���ٶ�ֵmm/s
	u16 oldreg01 = 0;	//��ʷ�Ĵ���01���ٶ�ֵmm/s
	u16 oldreg02 = 0;	//��ʷ�Ĵ���02���ٶ�ֵmm/s
	u16 oldreg03 = 0;	//��ʷ�Ĵ���03���ٶ�ֵmm/s
	u16 oldreg04 = 0;	//��ʷ�Ĵ���04���ٶ�ֵmm/s
	u16 oldreg05 = 0;	//��ʷ�Ĵ���05���ٶ�ֵmm/s
	u16 oldreg06 = 0;	//��ʷ�Ĵ���06���ٶ�ֵmm/s
	u16 oldreg07 = 0;	//��ʷ�Ĵ���07���ٶ�ֵmm/s
	u8 speedchg = 0;
	int ii=0;
	short passclk = 0;
	u8 speedstate = 0;
	int cnt = 0;

	(void)p_arg;	//����������澯
 
 	//�ӳ�300ms�ȴ�fpga׼������������epcs��Ҫʱ�䣿
 	//OSTimeDlyHMSM(0, 0, 0, 300);

	for(ii = 0;ii <100;ii++)
	{
		KernelRegs[ii] = 0;
	}

	while (1) 
	{
	    u8 buff[30];
		u16 l = 0;
		int iii = 0;
		speedchg = 0;
		
	/*	if(speedstate == 0)
		{

			if(KernelRegs[00] > 500)
			{
				speedstate = 1;
			}
			else
			{
				KernelRegs[00]++;
			}
		}
		else
		{
			if(KernelRegs[00] < -500)
			{
				speedstate = 0;
			}
			else
			{
				KernelRegs[00]--;
			}
		}
	*/	
		
		

		if((((oldreg79 >> 4) & 0x0001) == 0x0000) &&(((KernelRegs[79]>>4) & 0x0001) == 0x0001))
		{
			ptp(KernelRegs[0],(double)(KernelRegs[1])/10,(double)(KernelRegs[2]));
		}
		oldreg79 = KernelRegs[79];

		OSTimeDlyHMSM(0, 0, 0, 20);	 /* Ҳ���Ե��� OSTimeDly() ����ʵ���ӳ� */				  
	}
}


/*
*********************************************************************************************************
*                                   ����Ӧ�ó����Ӻ���(HOOK)
*
* ˵�� : ��Щ������ os_cpu_c.c �й���(HOOK)���á�
*
* ���� : ��
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)

/* ��һ�����񴴽����ʱ��uCOS�ں˵��øú���������ʱ�ж��ǽ�ֹ״̬ */
void  App_TaskCreateHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* ��һ������ɾ����uCOS�ں˵��øú���������ʱ�ж��ǽ�ֹ״̬ */
void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/* ���������idle����(CPU����ʱ)���á��û�������Ӵ�����CPU����˯��ģʽ������ʱ�ж���ʹ��״̬*/
#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/* �ú�����uC/OS-II��ͳ��������ã�ÿ�����һ�Ρ��û�������Ӷ����ͳ�ƴ��� */
void  App_TaskStatHook (void)
{
}

/* �ú����������л�ʱ�����á�����ʱ�ж��ǽ�ֹ״̬ */
#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
}
#endif

/* ��������� OS_TCBInit()���ã��ڳ�ʼ������������ƿ�ʱ���á��жϿ����ǽ�ֹ��Ҳ������ʹ�ܵ� */
#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/* ���������ÿ�εδ�ʱ�����á��жϿ����ǽ�ֹ��Ҳ������ʹ�ܵ� */
#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
}
#endif
#endif
