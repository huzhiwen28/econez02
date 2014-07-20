#include <ucos_ii.h>
#include "app_kernelregs.h"
#include "bsp_eeprom.h"


volatile short KernelRegs[REGSNUM];
OS_EVENT *RegsWriteMutex;//写锁

char RegsFlag[REGSNUM];

//加密串6个字节的seed，和2个字节的crc码
u8 Encryption[8];

//写加密串标识	 0不写 1写
u8 WriteEncry;


//上电读EEPROM数据
void App_RegsInit(void)				
{
	u8 err =0;
    int i = 0;
	u8 buff[80*2]={0};
	u8 buff2[80*2]={0};
	u8 RData;
	u16 AdcAdjust[5];		//ADC通道修正值
	WriteEncry = 0;
	//?eeprom????
	BSP_EEPROMReadByte(buff, 80*2,0, 0xA0);

	//延时
	{
 	  u16 i=36000;     
 	  while(i) 
  	 { 
  	   i--; 
  	 } 
	}

	BSP_EEPROMReadByte(buff2, 80*2,200, 0xA0);
	//延时
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
	    	KernelRegs[i] = KernelRegs[i] << 8;	   //高8位
		}
		else
		{
		 	State.Work.EEPROMERR=1 ;
		}

		RData=~buff2[2*i  +1];
		if(buff[2*i + 1]==RData)				//低8位
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

	//读加密串，8个字节，450起始地址
	BSP_EEPROMReadByte(Encryption,8,450, 0xA0);

	RegsWriteMutex = OSMutexCreate(2,&err);//OSSemCreate(1);

	BSP_EEPROMReadByte(buff, 10,400, 0xA0);	 //读ADC端口修正值
	{
	 	   i=1600;      	//延时
	 	  while(i) 
	  	  { 
	  	   	i--; 
	  	  } 
	}
	BSP_EEPROMReadByte(buff2, 10,500, 0xA0);	   //读ADC端口修正值
	{
	 	   i=1600;      	//延时
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
	    	AdcAdjust[i] =AdcAdjust[i] << 8;	   //高8位
		}
		else
		{
			 State.Work.EEPROMERR=1 ;
		}

		RData=~buff2[2*i  +1];
		if(buff[2*i + 1]==RData)				//低8位
		{
	    	AdcAdjust[i] =AdcAdjust[i] +  buff[2*i + 1];
		}
		else
		{
		  State.Work.EEPROMERR=1 ;
		}
	}
	Param.Basic.CH1K=  	AdcAdjust[0];//通道1修正值
	Param.Basic.CH2K=  	AdcAdjust[1]; 	//通道2修正值
	Param.Basic.CH3K=  	AdcAdjust[2]; 	//通道3修正值
	Param.Basic.CH4K=  	AdcAdjust[3];	//通道4修正值
	Param.Basic.CH2Z=  	AdcAdjust[4];	//通道2零点
}

//往EEPROM里保存数据
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

RD:		SSS=SendWordEEPROM(2*i,SaveD);	   //写第一次
//
//		while(!BSP_EEPROMWriteByte(2*i, 2*i, 0xA0));
//		while(!BSP_EEPROMWriteByte(2*i+1, 2*i+1, 0xA0));
		while(SSS==0);

	    OSTimeDly(3);
		SSS=ReadWordEEPROM(2*i);	            //读第一次
		if(SSS!=SaveD)
		{
			 EETimes++; 
			 if(EETimes>5)
			 {
			 	State.Work.EEPROMERR=1 ;
				goto  RD; 			//EERPOM错误
			 }
			 else
			 {
				goto  RD; 
			 }
		}
 		State.Work.EEPROMERR=0 ;
		EETimes=0;
RD2:	OSTimeDly(3);
		WData=~SaveD;			//按位取反
		Read=(2*i)+200;
		SSS=SendWordEEPROM(Read,WData);	    //写第二次
		while(SSS==0);
		OSTimeDly(3);
		WData=ReadWordEEPROM(Read);	            //读第二次
		Read=~WData;
		if(SaveD!=Read)
		{	     
			 EETimes++; 
			 if(EETimes>5)
			 {
			 	State.Work.EEPROMERR=1 ;
				goto  RD2; 				//EERPOM错误
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

		//写数据
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

	KernelRegs[0]=0;	   // 预设输出值	 %
	KernelRegs[1]=100;	   //启动增益	  %
	KernelRegs[2]=500;		   //启动延时时间 单位MS
	KernelRegs[3]=100; 			// 停车增益	   %
	KernelRegs[4]=500;			//		停车延时时间 单位MS
//	KernelRegs[5]			   //备用
	KernelRegs[6]=20;				//轴切输出值%
	KernelRegs[7]=1000;				//轴切时间	  MS
	KernelRegs[8]=2000;				//轴切辅制动时间MS
 	KernelRegs[9]=20;	   //

   	KernelRegs[10]=0;	   // 欠张力检测值
  	KernelRegs[11]=0;	   //欠张力检测值时间  ms
  	KernelRegs[12]=800;	   // 过张力检测值
  	KernelRegs[13]=10;	   // 过张力检测值时间	 ms

	KernelRegs[14]=50;		//卷轴最小时的输出值与主速度的比值  %

  	KernelRegs[15]=1000;	   // 最大速度（10V）对应主轴每分钟脉冲数

	KernelRegs[16]=2;		//	1：经典PID 控制  2：模糊PID 控制
   	KernelRegs[17]=2;		//1：放卷；2：收卷；3：牵引。
   	KernelRegs[18]=50;		//自动运行模式输出值上限  %		50==5V
//    KernelRegs[19]= ;
	KernelRegs[20]=0;		 //0：无锥度 1：有锥度
	KernelRegs[21]=50;		 //输出滤波值  %
	KernelRegs[22]=0;		 //卷径检测,0：不检测 1：检测
   	KernelRegs[23]=1;		 //1速度  2扭矩
   	KernelRegs[24]=1;		 //卷轴数 1单轴,2A/Blm 

   	KernelRegs[25]=100;		 //PID增益微调   %  （50~200）%
   	KernelRegs[26]=1;		 //PIDe极性	  1/2
   	KernelRegs[27]=10;		  //PID周期
   	KernelRegs[28]=100;		  //PID  KP
   	KernelRegs[29]=100;		  //PID  K1
   	KernelRegs[30]=100;		  //PID  KD
   	KernelRegs[31]=100;		  //PID  KB
   	KernelRegs[32]=100;		  //PID  KM
   	KernelRegs[33]=1;		  //PID 死区范围
   	KernelRegs[34]=2;		  //PID 超调区范围
   	KernelRegs[35]=5;		  //PID	   输入跳变率：

	KernelRegs[36]=0;		 //卷径测量方法	0、1、2、3、4、5、6	1	0：不测量1：来自于模拟量（5,6,7）2：厚度累加法(2,3,4,5,6,7)3：脉冲比值法(3,4,5,6,8)4：模拟量比值法(5,6,8)5：来自超声波传感器6: 来自通信

	KernelRegs[37]=	100;		//卷料厚度	1～10000 um	100um	用于厚度累加法中计算卷径	 1um 单位
	KernelRegs[38]=1;		//卷轴脉冲数	0~10000	1	用于方法:4/5/6
	KernelRegs[39]=1;		//主轴脉冲数	0~10000	1	用于方法:5/6
	KernelRegs[40]=1000;		   //最小半径值	100~10000	1000(10cm)	10mm~1000mm最终用户用	 0.1mm单位
	KernelRegs[41]=5000;	//最大半径值	100~50000	5000(50cm)	10mm~5000mm最终用户用	 0.1mm单位
	KernelRegs[42]=	0;	 //42（加）	主轴脉冲信号来自于	0：无1：IO口5  2：编码器口

	KernelRegs[43]=100;			//43	主轴直径	100～20000	100(10mm)		0.1mm单位
	KernelRegs[44]=0;		//	模拟量端口1输出（需上电重启后有效）	0-6	1	0：0-5V1：0 - 10V2：-10 - +10V3：-5-+5V4：4-20 ma5：0-20 ma6：-20-+20ma
	KernelRegs[45]=	0;	 	//	模拟量端口2输出
	KernelRegs[46]=0;	 	//	模拟量端口3输出
	KernelRegs[47]=0;	 	//	模拟量端口4输出
	KernelRegs[48]=0;	   //卷轴A脉冲信号来自于	0：无  1：IO口6   2：编码器口	0	用于方法2、3
	KernelRegs[49]=0;	   //卷轴B脉冲信号来自于	0：无  1：IO口5   2：编码器口	0	用于方法2、3
	KernelRegs[50]=1;		//速度信号来自	1-2	1	1：来自模拟输入口2  2：来自于通信
	KernelRegs[51]=2;		//设定信号来自	1-2	1	1：来自模拟输入口4  2：来自于通信

	KernelRegs[52]=100;		//10%卷径时锥度值	0-100%
	KernelRegs[53]=100;		//20%卷径时锥度值	0-100%
	KernelRegs[54]=100;		//30%卷径时锥度值	0-100%
	KernelRegs[55]=100;		//40%卷径时锥度值	0-100%
	KernelRegs[56]=100;		//50%卷径时锥度值	0-100%
	KernelRegs[57]=100;		//60%卷径时锥度值	0-100%
	KernelRegs[58]=100;		//70%卷径时锥度值	0-100%
	KernelRegs[59]=100;		//80%卷径时锥度值	0-100%
	KernelRegs[60]=100;		//90%卷径时锥度值	0-100%
	KernelRegs[61]=100;		//100%卷径时锥度值	0-100%

	KernelRegs[62]=0;		//速度前馈	-1000~1000	0	补尝主轴电机与主轴速度模拟量之间的滞后性
	KernelRegs[63]=0;		//张力零位值	0～10000	0	只读
	KernelRegs[64]=1000;		//主速度零位值	0～10000	5000	只读
	KernelRegs[65]=0;		//卷径百分比设定	0-99%	0%	读写
	KernelRegs[66]=0;		//内部状态量	0-	0	只读
//	KernelRegs[67]=1000;	//	卷径修正比	0～5000	1000	只读
	KernelRegs[68]=0;	//	停机时卷径清零	0：不清零 1：清零	0	当RUN信号由1变0时，自动把卷径值清除	,22：不清零，停机自动记忆，上电自动恢复
   	KernelRegs[69]=0;		  //PID

   	KernelRegs[70]=0x0070;	  //485set,9600bps,8,1,N

	for(i = 0; i < NEEDSAVENUM;i++)
	{
	    RegsFlag[i] = 1;
	}
  
}
