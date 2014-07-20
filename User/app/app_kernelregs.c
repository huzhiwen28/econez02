#include <ucos_ii.h>
#include "app_kernelregs.h"
#include "bsp_eeprom.h"


volatile short KernelRegs[REGSNUM];
OS_EVENT *RegsWriteMutex;//д��

char RegsFlag[REGSNUM];

//���ܴ�6���ֽڵ�seed����2���ֽڵ�crc��
u8 Encryption[8];

//д���ܴ���ʶ	 0��д 1д
u8 WriteEncry;


//�ϵ��EEPROM����
void App_RegsInit(void)				
{
	u8 err =0;
    int i = 0;
	u8 buff[80*2]={0};
	u8 buff2[80*2]={0};
	u8 RData;
	u16 AdcAdjust[5];		//ADCͨ������ֵ
	WriteEncry = 0;
	//?eeprom????
	BSP_EEPROMReadByte(buff, 80*2,0, 0xA0);

	//��ʱ
	{
 	  u16 i=36000;     
 	  while(i) 
  	 { 
  	   i--; 
  	 } 
	}

	BSP_EEPROMReadByte(buff2, 80*2,200, 0xA0);
	//��ʱ
	{
 	   i=36000;     
 	  while(i) 
  	 { 
  	   i--; 
  	 } 
	}
    for(i = 0; i < NEEDSAVENUM;i++)
	{	

		RData=~buff2[2*i];
		if(buff[2*i]==RData)
		{
		    KernelRegs[i] = buff[2*i];
	    	KernelRegs[i] = KernelRegs[i] << 8;	   //��8λ
		}
		else
		{
		 	State.Work.EEPROMERR=1 ;
		}

		RData=~buff2[2*i  +1];
		if(buff[2*i + 1]==RData)				//��8λ
		{
	    	KernelRegs[i] = KernelRegs[i] +  buff[2*i + 1];
		}
		else
		{
		  State.Work.EEPROMERR=1 ;
		}
	}

    for(i = 0; i < REGSNUM;i++)
	{
	    RegsFlag[i] = 0;
	}

	//Gather para
	for(i = 0; i < 10;i++)
	{
		KernelRegs[100+i] = 0;
	}
	KernelRegs[110] = 10;

	//�����ܴ���8���ֽڣ�450��ʼ��ַ
	BSP_EEPROMReadByte(Encryption,8,450, 0xA0);

	RegsWriteMutex = OSMutexCreate(2,&err);//OSSemCreate(1);

	BSP_EEPROMReadByte(buff, 10,400, 0xA0);	 //��ADC�˿�����ֵ
	{
	 	   i=1600;      	//��ʱ
	 	  while(i) 
	  	  { 
	  	   	i--; 
	  	  } 
	}
	BSP_EEPROMReadByte(buff2, 10,500, 0xA0);	   //��ADC�˿�����ֵ
	{
	 	   i=1600;      	//��ʱ
	 	  while(i) 
	  	  { 
	  	   	i--; 
	  	  } 
	}

    for(i = 0; i < 6;i++)
	{	

		RData=~buff2[2*i];
		if(buff[2*i]==RData)
		{
		    AdcAdjust[i] = buff[2*i];
	    	AdcAdjust[i] =AdcAdjust[i] << 8;	   //��8λ
		}
		else
		{
			 State.Work.EEPROMERR=1 ;
		}

		RData=~buff2[2*i  +1];
		if(buff[2*i + 1]==RData)				//��8λ
		{
	    	AdcAdjust[i] =AdcAdjust[i] +  buff[2*i + 1];
		}
		else
		{
		  State.Work.EEPROMERR=1 ;
		}
	}
	Param.Basic.CH1K=  	AdcAdjust[0];//ͨ��1����ֵ
	Param.Basic.CH2K=  	AdcAdjust[1]; 	//ͨ��2����ֵ
	Param.Basic.CH3K=  	AdcAdjust[2]; 	//ͨ��3����ֵ
	Param.Basic.CH4K=  	AdcAdjust[3];	//ͨ��4����ֵ
	Param.Basic.CH2Z=  	AdcAdjust[4];	//ͨ��2���
}

//��EEPROM�ﱣ������
void App_RgesRun(void *p_arg)		  
{
    int i = 0;
	u16 SSS;
	u16 Read;
	u16 EETimes;
	u16 WData;
	u16 SaveD;

//	ParamInit();
	//	AInPortAdjudt();
	while(1)
	{
    for(i = 0; i < NEEDSAVENUM;i++)
	{
	  if(RegsFlag[i] == 1)
	  {
	    //?eeprom
		Read=2*i;
		OSTimeDly(3);
		SaveD=KernelRegs[i];

RD:		SSS=SendWordEEPROM(2*i,SaveD);	   //д��һ��
//
//		while(!BSP_EEPROMWriteByte(2*i, 2*i, 0xA0));
//		while(!BSP_EEPROMWriteByte(2*i+1, 2*i+1, 0xA0));
		while(SSS==0);

	    OSTimeDly(3);
		SSS=ReadWordEEPROM(2*i);	            //����һ��
		if(SSS!=SaveD)
		{
			 EETimes++; 
			 if(EETimes>5)
			 {
			 	State.Work.EEPROMERR=1 ;
				goto  RD; 			//EERPOM����
			 }
			 else
			 {
				goto  RD; 
			 }
		}
 		State.Work.EEPROMERR=0 ;
		EETimes=0;
RD2:	OSTimeDly(3);
		WData=~SaveD;			//��λȡ��
		Read=(2*i)+200;
		SSS=SendWordEEPROM(Read,WData);	    //д�ڶ���
		while(SSS==0);
		OSTimeDly(3);
		WData=ReadWordEEPROM(Read);	            //���ڶ���
		Read=~WData;
		if(SaveD!=Read)
		{	     
			 EETimes++; 
			 if(EETimes>5)
			 {
			 	State.Work.EEPROMERR=1 ;
				goto  RD2; 				//EERPOM����
			 }
			 else
			 {
				goto  RD2; 
			 }
		}
		State.Work.EEPROMERR=0 ;
		EETimes=0;
	    RegsFlag[i] = 0;
 
	  }
	}
	if(WriteEncry == 1)
	{
		WriteEncry = 0;

		//д����
		while(!BSP_EEPROMWriteByte(Encryption[0], 450+0, 0xA0));
				OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[1], 450+1, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[2], 450+2, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[3], 450+3, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[4], 450+4, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[5], 450+5, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[6], 450+6, 0xA0));
		OSTimeDly(3);
		while(!BSP_EEPROMWriteByte(Encryption[7], 450+7, 0xA0));
		OSTimeDly(3);
	}
ENDEEP:	OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

void  ParamInit()
{
 u8 i;

	KernelRegs[0]=0;	   // Ԥ�����ֵ	 %
	KernelRegs[1]=100;	   //��������	  %
	KernelRegs[2]=500;		   //������ʱʱ�� ��λMS
	KernelRegs[3]=100; 			// ͣ������	   %
	KernelRegs[4]=500;			//		ͣ����ʱʱ�� ��λMS
//	KernelRegs[5]			   //����
	KernelRegs[6]=20;				//�������ֵ%
	KernelRegs[7]=1000;				//����ʱ��	  MS
	KernelRegs[8]=2000;				//���и��ƶ�ʱ��MS
 	KernelRegs[9]=20;	   //

   	KernelRegs[10]=0;	   // Ƿ�������ֵ
  	KernelRegs[11]=0;	   //Ƿ�������ֵʱ��  ms
  	KernelRegs[12]=800;	   // ���������ֵ
  	KernelRegs[13]=10;	   // ���������ֵʱ��	 ms

	KernelRegs[14]=50;		//������Сʱ�����ֵ�����ٶȵı�ֵ  %

  	KernelRegs[15]=1000;	   // ����ٶȣ�10V����Ӧ����ÿ����������

	KernelRegs[16]=2;		//	1������PID ����  2��ģ��PID ����
   	KernelRegs[17]=2;		//1���ž�2���վ�3��ǣ����
   	KernelRegs[18]=50;		//�Զ�����ģʽ���ֵ����  %		50==5V
//    KernelRegs[19]= ;
	KernelRegs[20]=0;		 //0����׶�� 1����׶��
	KernelRegs[21]=50;		 //����˲�ֵ  %
	KernelRegs[22]=0;		 //�����,0������� 1�����
   	KernelRegs[23]=1;		 //1�ٶ�  2Ť��
   	KernelRegs[24]=1;		 //������ 1����,2A/Blm 

   	KernelRegs[25]=100;		 //PID����΢��   %  ��50~200��%
   	KernelRegs[26]=1;		 //PIDe����	  1/2
   	KernelRegs[27]=10;		  //PID����
   	KernelRegs[28]=100;		  //PID  KP
   	KernelRegs[29]=100;		  //PID  K1
   	KernelRegs[30]=100;		  //PID  KD
   	KernelRegs[31]=100;		  //PID  KB
   	KernelRegs[32]=100;		  //PID  KM
   	KernelRegs[33]=1;		  //PID ������Χ
   	KernelRegs[34]=2;		  //PID ��������Χ
   	KernelRegs[35]=5;		  //PID	   ���������ʣ�

	KernelRegs[36]=0;		 //����������	0��1��2��3��4��5��6	1	0��������1��������ģ������5,6,7��2������ۼӷ�(2,3,4,5,6,7)3�������ֵ��(3,4,5,6,8)4��ģ������ֵ��(5,6,8)5�����Գ�����������6: ����ͨ��

	KernelRegs[37]=	100;		//���Ϻ��	1��10000 um	100um	���ں���ۼӷ��м����	 1um ��λ
	KernelRegs[38]=1;		//����������	0~10000	1	���ڷ���:4/5/6
	KernelRegs[39]=1;		//����������	0~10000	1	���ڷ���:5/6
	KernelRegs[40]=1000;		   //��С�뾶ֵ	100~10000	1000(10cm)	10mm~1000mm�����û���	 0.1mm��λ
	KernelRegs[41]=5000;	//���뾶ֵ	100~50000	5000(50cm)	10mm~5000mm�����û���	 0.1mm��λ
	KernelRegs[42]=	0;	 //42���ӣ�	���������ź�������	0����1��IO��5  2����������

	KernelRegs[43]=100;			//43	����ֱ��	100��20000	100(10mm)		0.1mm��λ
	KernelRegs[44]=0;		//	ģ�����˿�1��������ϵ���������Ч��	0-6	1	0��0-5V1��0 - 10V2��-10 - +10V3��-5-+5V4��4-20 ma5��0-20 ma6��-20-+20ma
	KernelRegs[45]=	0;	 	//	ģ�����˿�2���
	KernelRegs[46]=0;	 	//	ģ�����˿�3���
	KernelRegs[47]=0;	 	//	ģ�����˿�4���
	KernelRegs[48]=0;	   //����A�����ź�������	0����  1��IO��6   2����������	0	���ڷ���2��3
	KernelRegs[49]=0;	   //����B�����ź�������	0����  1��IO��5   2����������	0	���ڷ���2��3
	KernelRegs[50]=1;		//�ٶ��ź�����	1-2	1	1������ģ�������2  2��������ͨ��
	KernelRegs[51]=2;		//�趨�ź�����	1-2	1	1������ģ�������4  2��������ͨ��

	KernelRegs[52]=100;		//10%��ʱ׶��ֵ	0-100%
	KernelRegs[53]=100;		//20%��ʱ׶��ֵ	0-100%
	KernelRegs[54]=100;		//30%��ʱ׶��ֵ	0-100%
	KernelRegs[55]=100;		//40%��ʱ׶��ֵ	0-100%
	KernelRegs[56]=100;		//50%��ʱ׶��ֵ	0-100%
	KernelRegs[57]=100;		//60%��ʱ׶��ֵ	0-100%
	KernelRegs[58]=100;		//70%��ʱ׶��ֵ	0-100%
	KernelRegs[59]=100;		//80%��ʱ׶��ֵ	0-100%
	KernelRegs[60]=100;		//90%��ʱ׶��ֵ	0-100%
	KernelRegs[61]=100;		//100%��ʱ׶��ֵ	0-100%

	KernelRegs[62]=0;		//�ٶ�ǰ��	-1000~1000	0	�����������������ٶ�ģ����֮����ͺ���
	KernelRegs[63]=0;		//������λֵ	0��10000	0	ֻ��
	KernelRegs[64]=1000;		//���ٶ���λֵ	0��10000	5000	ֻ��
	KernelRegs[65]=0;		//���ٷֱ��趨	0-99%	0%	��д
	KernelRegs[66]=0;		//�ڲ�״̬��	0-	0	ֻ��
//	KernelRegs[67]=1000;	//	��������	0��5000	1000	ֻ��
	KernelRegs[68]=0;	//	ͣ��ʱ������	0�������� 1������	0	��RUN�ź���1��0ʱ���Զ��Ѿ�ֵ���	,22�������㣬ͣ���Զ����䣬�ϵ��Զ��ָ�
   	KernelRegs[69]=0;		  //PID

   	KernelRegs[70]=0x0070;	  //485set,9600bps,8,1,N

	for(i = 0; i < NEEDSAVENUM;i++)
	{
	    RegsFlag[i] = 1;
	}
  
}
