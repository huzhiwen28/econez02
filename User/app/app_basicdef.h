										   
/*  
����
time:2008,09,26
*/
#ifndef _APP_BASICDEF_H
#define _APP_BASICDEF_H

#include "stm32f10x.h"


typedef unsigned int uint;    
typedef unsigned long ulong;    
typedef unsigned char uchar;



extern uint ADCBuf[];
extern int PidOut;
extern int KPout;
extern int KIout;
extern int KDout;
extern u32  KPSL;
extern struct PID Pid; // PID Control Structure
extern struct ParameterSate Param;
extern	struct state State;
extern struct PortState Port;

extern unsigned int rout; // PID Response (Output)
extern unsigned int rin; // PID Feedback (Input)


extern	uint  OUTDATA[10];
extern 	uint  OUTDATB[10];
extern	long int SETD;

//uint CH2AD[20];

//#define _nop_() {__asm__ volatile("nop");}
#define _nop_() { ;} 
 

void Delay_1ms(void);
void Delay(unsigned int n);
void Write_Cmd(unsigned char cmd);
void Write_Data(unsigned char dat);
uchar Read_Data(void);
void Ini_Lcd(void);
void dis_pic(uchar *pic);
void tuchu(uchar x,uchar y)	;
void Clear_GDRAM(void);
void Draw_TX(unsigned char Yaddr,unsigned char Xaddr,const unsigned char * dp);
void Disp_HZ(uchar addr,const uchar * pt,uchar num);
void Draw_PM(const uchar *ptr);
void Read_Data1(uchar*lcdtemp1,uchar *lcdtemp2);
void ATUO_Menu1(void);
void ZL_Make(void);
void DateDisplay(uchar , uchar, uchar, uchar, uchar );


struct PID {
uint SetD;      // �趨Ŀ��Desired value
ulong CycT;
ulong  KP;    // ��������Proportional Const
ulong   KI;      // ���ֳ���Integral Const
ulong   KD;    // ΢�ֳ���Derivative Const
ulong   KB;    // ����������
ulong   KS;	//��������
int LastError;     // Error[-1]

int PrevError;    // Error[-2]
int  SumError;    // Sums of Errors
int IFull;
int IZero;
uint Pol;

uint Die;			//
uint Over;			//��������Χ1~100% 80%
uint Change;
u16 Start;
u16 KPV;
u16 KPD;
};


struct AutoParam
{
	uint SetAim;
	uint OpenOut;
	uint StartOut;
	uint StartDelayTime;
	uint StopOut;
	uint StopDelayTime;
	uint JogOut;
	uint ChangeOut;
	uint ChangeTime;
	uint ChangeStopTime;
};

struct MaunParam
{
	uint SetAim;
	uint Out;
	uint SetupZero;

};

struct TensilityInputParam
{
	uint Code;
	uint Float;
	uint Unit;
	uint Full;
	uint SetZero;
	uint Test;
	uint Zero;
	uint Line;	
};


struct DiaParam			//�����Ʋ���
{
	uint Test;         //0"����⣬1���
	uint Way;			//����������
	uint Ply;			//���Ϻ��
	int  RollD;			//����������
	int  MainD;			//����������
	uint LeastDia;			//��Сֱ��ֵ
	uint MostDia;			//���ֱ��ֵ
//	uint StartDia;			//����ֱ������ֵ
//	uint InitDia;		//��ʼֱ��
	uint Ratio;				//����ֱ��/
	uint RollAWay;			//����A�����ź�������  0���� 1��IO��6   2����������
	uint RollBWay;			//����B�����ź�������  0���� 1��IO��5   2����������
	uint RollMWay;			//���������ź�������  0���� 1��IO��5   2����������

};

struct BasicParam
{
//	uint Code;				//�ڲ�����ֵ
	uint MMFR;				//0����ת��1��ת
	uint Mode;				//����ģʽ���Զ����ֶ�����
	uint MachMode;			//1���ž�2���վ�3��ǣ����
	uint ShowFilter;		//ʵ��������ʾ�˲�
	uint OutFilter;			//ʵ����������˲�
	uint Out;				//1:Ť�������2���ٶ����
//	uint UpSpeed;			 //������yi
//	uint DownSpeed;
	uint ZeroTest;			//���������ֵ
	uint OverTest;			//���������ֵ
	uint ZeroTestTiems;		//���������ʱ��
	uint OverTestTiems;		//���������ʱ��
	uint ZDZ;				//׶��ֵ��
	//uint ZDMode;			//���ƹ���ѡ��
	uint ZDLine;			//׶������ 1��ֱ��׶�ȣ�2������׶��
	uint LeastOut;			//��С���
	uint MostOut;			//������
	uint SaveOut;			//�������
	uint MaxMainSpeedPor;	//������Сʱ��Ӧ���ٶȵ�������ʣ���������ٶȵ��������ٶȱ��ʣ�
	uint Chang ;			//0:���ᣬ1��˫��
	uint KPid;
	uint 	MSPAF;	 //�ٶ�ǰ������
	uint MainSpeedWay;  //���ٶ�������
	uint SetWay;          	//��������������
	int MainSpeedZero;         //���ٶ���λ
	uint AutoClearDia;		//����ͣ��ʱ�Զ�����0
	u16  SetOut;
	u16 CH1K;
	u16 CH2K;
	u16 CH3K;
	u16 CH4K;
	u16 CH2Z;
};

struct AOutPortParam			//�����Ʋ���
{
u8  CH1OutMode;
u8  CH2OutMode;
u8  CH3OutMode;
u8  CH4OutMode;

};

struct ZDZParam			//�����Ʋ���
{
u16  ZDZ10;
u16  ZDZ20;
u16  ZDZ30;
u16  ZDZ40;
u16  ZDZ50;
u16  ZDZ60;
u16  ZDZ70;
u16  ZDZ80;
u16  ZDZ90;
u16  ZDZ100;

};

struct ParameterSate
{
	struct BasicParam Basic;
	struct DiaParam Dai;
	struct AOutPortParam AOutPort;
	struct TensilityInputParam Tens;
	struct MaunParam Maun;
	struct AutoParam Auto;
	struct ZDZParam	ZDZ;
};
	struct BasicState					//�������״̬
	{
		uint T5;
		uint PIDCycFLG;
		uint DisplayFLG;
		uint DisMode;
		uint Alarm;
		uint OverFLG;
		uint ZeroFLG;
		uint Mima;          		     //�������������
		uint Mode;						//1:�Զ�����ģʽ��2�ֶ���3��
		uint KeyFLG:1;					//������־	
		uint ADFLG:1;					
		uint Param:3;

		uint Run:3;						//0:ֹͣ��1������
		uint OutCh:3;					//0:Aͨ�������1��B
		uint KeyOutA:3;					//����������� 0����ֹ��1������
		uint IoOutA:3;					//IO����������� 0����ֹ��1������//��IO��������3���ƣ���ѡ��ʱ��Ĭ���������
		uint OutSave:3;					//������䣬0������	��1��
		uint SpeedUp:3;					//�����źţ�0������1����
		uint SpeedDown:3;				//��
		uint Jog:3;						//�㶯�ź� 0:������1���㶯
		uint PidStart:2;						//�㶯�ź� 0:������1���㶯
		uint CodeError:1;
		uint SetLock:1;
		uint Out:2;						// 1��Ť���źţ�2 �ٶ��ź�
		uint SaveF:1;
		uint OK:1;
		uint ParamERR:1;		//�������ô���
		uint USBState:1;		//Usb״̬
		uint DiaOK:1;			//�����ҵ�
		uint ParamERRID;		//�������ô���ID
	    uint ID;
		uint KeyLTimes;
		uint SaveTimes;		
		uint PidInit;
	};	

	struct ADCState
	{
		uint Buf;				//ת��ֵ�����
		uint CHAL;
		uint Ch1Buf[10];			//ͨ��1
		uint Ch2Buf[10];			//ͨ��2
		uint Ch3Buf[10];			//ͨ��3
		uint Ch4Buf[10];			//ͨ��3
		uint Times;				//ת������
		uint AdcCh;				//ͨ��
	};

	struct WorkState
	{
		int InData;
		int Tens;
		uint SetData;
		uint MSpeed;				//�����ٶ��ź�
		uint RSpeed;				//������ٶ��ź�
		uint Dia;					//A��ʵ��ֵ
		u32 DiaPer;					//A���ٷֱ�
		uint Dia2;					//B��ʵ��ֵ
		u32 DiaPer2;				//B���ٷֱ�
		uint MaunOut;
		uint SpeedContrast;			//�ٶȲ��ź�
		uint ZDZ;
		u32 RollDT;
		u32 RollDT2;
		int64_t MainDT;
		u32 RollDS;
		u32 RollDS2;	//B��������
		int64_t MainDS;		  //����Ȧ��
		uint RollDSpeed;	 //A����ٶ�
		uint RollD2Speed;	 //B����ٶ�
		uint MainDSpeed;	//������ٶ�
		u16  EEPROMERR;  //EEPROM����
		u16  DIAERR;  //������
	};

	struct ActionSate
	{
		uint StartFlg;
		uint Start;
		uint StopFlg;
		uint Stop;	
		uint JogFlg;
		uint Jog;
		uint AChangeFlg;
		uint AChange;
		uint AChangeStopFlg;
		uint AChangeStop;
		uint BChangeFlg;
		uint BChange;
		uint BChangeStopFlg;
		uint BChangeStop;
		uint OverFLG;
		uint Over;
		uint ZeroFLG;
		uint Zero;

		uint KeyFLG;
		uint Key;

	};

	struct OutState
	{
		uint Mode;		//0���������1:PID�Զ������2�ֶ������3������Ԥ�����4��ͣ�����,5�㶯���,6���������7����Ԥ���
		uint Mode2;		//0���������1:PID�Զ������2�ֶ������3������Ԥ�����4��ͣ�����,5�㶯���,6���������7����Ԥ���
		uint Port;
		uint Data;		//ͨ��A���
		uint Data2;		//ͨ��B���
		uint Per;
		uint Per2;
	};

	struct PIDState
	{
	uint LeastOut;			//��С���
	uint MostOut;			//������		
	uint Over;				//������
	uint Die;				//����
	uint ChangeH;				//�仯�ȸ���
	uint ChangeL;				//�仯�ȵ���	
	};

	struct StataToPLCBit
	{
	  u16 Run: 1;	  //����
	  u16 ZeroFLG: 1;	   //Ƿ����
	  u16 OverFLG: 1;	   //������
	  u16 JOG: 1;		 //�㶯��ͣ
	  u16 XZFX:1;    //�����ת����0��10V��0��0��-10V:1
	  u16 ERR:1;
	  u16 OK:1;			//����
	  u16 Save:1;
	  u16 Break:1;       //�ϵ�
	  u16 A_B:1;   //0:A�ᣬ1B
	  u16 EepromErr:1;		//EEPROM����
	  u16 DiaErr:1;		//���ƫ���
	  u16 ParamC:1;		  //�����иı䣬��PLC��
	  u16 ParamErr:1;	//����У�Դ���
	  u16 USBState:1;		//Usb״̬
	  u16 DiaOK:1;			//�����ҵ�
	};

	union  StataToPLCUnion
	{
	 u16 All;
	 struct StataToPLCBit  Bit;
	};

 	struct PLCCommandBit
	{
	  u16 Run: 1;	  //����
	  u16 Sampling: 1;	   //����
	  u16 Refurbish: 1;	   //pc ������ˢ����
	  u16 Off_Line:1;        //PC����������
	  u16 JOG: 1;		 //�㶯��ͣ
	  u16 XZFX:1;    //�����ת����0��10V��0��0��-10V:1
	  u16 ERR:1;
	  u16 Reset:1;			//��λ
	  u16 Save:1;		   //����
	  u16 ClearSave:1;     //�������
	  u16 UseSave:1;       //Ӧ�ü���
	  u16 ClearDia:1;       //������
	  u16 TensZero:1;   //����У��
	  u16 MainSpeedZero:1;  //���ٶ�У��
	  u16 ABChange:1;  		//A/B���л�
	  u16 DiaSet:1;  		//A/B���л�
	};
	union  PLCCommandUnion
	{
	 u16 All;
	 struct PLCCommandBit  Bit;
	};

	struct PLCCommandBit2
	{
	  u16 InitParam: 1;	  //�ָ���������
	  u16 ADCZero: 1;	   //����
	  u16 ADCTest: 1;	   //pc ������ˢ����

	};
	union  PLCCommandUnion2
	{
	 u16 All;
	 struct PLCCommandBit2  Bit;
	};
 // 	union  PLCCommandUnion	PLC_Data;

	struct state
	{
		struct BasicState Basic;
		struct ADCState ADC;
		struct WorkState Work;
		struct ActionSate Action;
		struct OutState Out;
		struct PIDState	PID;
		union  StataToPLCUnion	ToPLC;
		union  PLCCommandUnion	PLC_Command;
		union  PLCCommandUnion2	PLC_Command2;
	//   	union  PLCCommandUnion	PLC_Data;
	};

struct DataInputPort
{
	uint S1:1;
	uint S2:1;
	uint S3:1;
	uint S4:1;
	uint S5:1;
	uint S6:1;
	uint S7:1;
	uint S8:1;
	uint S9:1;
};

union DInput
{
struct DataInputPort Bits;
uint All;
};

struct DataOutputPort
{
uint SO1:1;
uint SO2:1;
};

union DOutput
{
struct DataOutputPort Bits;
uint All;
};

struct AInputPort
{
uint Ch1;
int Ch2;
uint Ch3;
uint Ch4;
};

struct AOutputPort
{
u32 Ch1;
u32 Ch2;
u32 Ch3;
u32 Ch4;
};

struct PortState
{
union DInput  DIn;
union DOutput  DOut;
struct AOutputPort  AOut;
struct AInputPort  AIn;
};


struct PidRunState
{
u16  ErrAbs[500];
int  ErrO[500];
u16  DErr[500];
u16  No;
u16  UseNo;
};



#endif


