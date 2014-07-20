

#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "bsp_TimesInt.h"
#include "app_kernelregs.h"
#include "bsp_eeprom.h"

void PidOut_Cap(void);
uint DaiOut(void);

void PPB(void);

void PPM(void);
void PPS(void);
void PZO(void);
void PNS(void);
void PNM(void);
void PNB(void);
void IPB(void);
void IPM(void);
void IPS(void);
void IZO(void);
void INS(void);
void INM(void);
void INB(void);
void DPB(void);
void DPM(void);
void DPS(void);
void DZO(void);
void DNS(void);
void DNM(void);
void DNB(void);


#define KPPB  	PPB();				// ������  ���  ��
#define KPPM  	PPM();
#define KPPS  	PPS();				// ������  ��С	 ��
#define KPZO  	PZO();
#define KPNS  	PNS();
#define KPNM  	PNM();						// ������  ��	��
#define KPNB  	PNB();

//pid.SumError=Pid.SumError+(Pid.KP*Pid.CycT*PPP)/(Pid.KI*100)
#define KIPB  	IPB();
#define KIPM  	IPM();							// ������  �� 	 ��
#define KIPS  	IPS();
#define KIZO  	IZO();				 				// ������  0
#define KINS  	INS();			// ������  ��С  ��
#define KINM  	INM();
#define KINB  	INB();


//Iout=KP*(TD/TS)*DE
#define KDPB  	DPB();
#define KDPM  	DPM();							// ΢����  ��	��
#define KDPS  	DPS();
#define KDZO  	DZO();										// ΢����  0
#define KDNS  	DNS();
#define KDNM  	DNM();						// ΢����  ��	��
#define KDNB  	DNB();

