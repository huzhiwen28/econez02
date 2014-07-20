/*
*********************************************************************************************************
*	                                  
*	ģ������ : ϵͳ����ģ�顣
*	�ļ����� : system_stm32f10x.c 
*	��    �� : V3.4.0
*	˵    �� :	����ST�̼����ṩ��CMSIS Cortex-M3����ӿڲ�ϵͳ�ļ������������ӽ���Ӣ��ע�ͽ����˷��룬
*			Դ������δ�����κθ��ġ�
*				SystemInit()����������¹�����
*				(1) ����CPU��ʱ�� ��ȱʡ72MHz��
*				(2) ��ʼ��FSMC�ӿ������ⲿSRAM ��ȱʡ������FSMC��
*				(3) �����ж��������ַ ��ȱʡ��λ���ڲ�Flash��ַ0x0800 0000��
*
*				SystemInit()�����������ļ��б����ã���c���ʼ��ǰ��ִ�У������û����Խ�C���������������
*			�ⲿSRAM��
*				
*				����VECT_TAB_SRAM����Խ��ж�������λCPU�ڲ�RAM��ַ0x2000 0000��
*
*				������һ��ȫ�ֱ���uint32_t SystemCoreClock���û��������������ж�ȡ�ñ������CPU��ʱ�ӡ�
*				
*				�û�������Ҫ�޸ı��ļ���
*				(1) �޸ı��ļ��е���غ�������CPU��ʱ�ӡ�(ȱʡ����Ϊ72MHz)
*				(2) ȡ��#define DATA_IN_ExtSRAM��ע��ʹ��FSMC���ù��ܡ�(ȱʡ�ǽ�ֹ��)
*				(3) ������C��������Ԥ����ѡ��������VECT_TAB_SRAM�꽫�ж�������λ��CPU�ڲ�RAM��
*
*				system_stm32f10x.h�ļ������ \Libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2010-10-15 ST      ST�̼���V3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/  

#include "stm32f10x.h"

/*
	ͨ��ע����صĺ�ѡ��ϵͳʱ�ӡ�
	��Ҫ��ʾ��
	1��CPU��λ֮��HSI����SYSCLKԴ��
	2����ȷ��ѡ���ϵͳʱ�Ӳ�����MCU�����Ƶ�ʡ�
	3������������κ�ʱ�Ӻ꣬��ôHSI������ϵͳʱ�ӡ�
	4��ϵͳʱ�����ú����ṩ
		- ���ڵ�������ֵ�͡���������ֵ�ͺ͸�������ֵ�͵�MCU���ⲿ8MHz������Ϊϵͳʱ��������
		- ���ڵ������͡��������ͺ͸������͵�MCU���ⲿ8MHz������Ϊϵͳʱ��������
		- ���ڻ����͵�MCU���ⲿ25MHz������Ϊϵͳʱ��������
	
		���ʹ�ò�ͬ�ľ���������޸���Ӧ�ĳ���
*/
#if defined (STM32F10X_LD_VL) || (defined STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
 #define SYSCLK_FREQ_24MHz  24000000
#else
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
/* #define SYSCLK_FREQ_24MHz  24000000 */ 
/* #define SYSCLK_FREQ_36MHz  36000000 */
/* #define SYSCLK_FREQ_48MHz  48000000 */
/* #define SYSCLK_FREQ_56MHz  56000000 */
#define SYSCLK_FREQ_72MHz  72000000
#endif

/* ʹ���ⲿSRAM��ΪC����������ʱ����Ҫ�����DATA_IN_ExtSRAM */
#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
/* #define DATA_IN_ExtSRAM */
#endif

/* ѡ���ж��������ַ����ע�����д����ʾ�ж�������λ��CPU�ڲ�Flash������ᶨλ��CPU�ڲ�SRAM */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0	/* �ж�������ƫ�Ƶ�ַ��������0x100�������� */

/* ϵͳʱ�ӣ��ں�ʱ�ӣ����� */
#ifdef SYSCLK_FREQ_HSE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_HSE;
#elif defined SYSCLK_FREQ_24MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_24MHz;
#elif defined SYSCLK_FREQ_36MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_36MHz;
#elif defined SYSCLK_FREQ_48MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz;
#elif defined SYSCLK_FREQ_56MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_56MHz;
#elif defined SYSCLK_FREQ_72MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_72MHz;
#else
  uint32_t SystemCoreClock         = HSI_VALUE;
#endif

/* AHBԤ��Ƶ�� */
__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

static void SetSysClock(void);

#ifdef SYSCLK_FREQ_HSE
  static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_24MHz
  static void SetSysClockTo24(void);
#elif defined SYSCLK_FREQ_36MHz
  static void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
  static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_56MHz
  static void SetSysClockTo56(void);  
#elif defined SYSCLK_FREQ_72MHz
  static void SetSysClockTo72(void);
#endif

#ifdef DATA_IN_ExtSRAM
  static void SystemInit_ExtMemCtl(void); 
#endif

/*
*********************************************************************************************************
*	�� �� ��: SystemInit
*	����˵��: ��ʼ����Ƕ��Flash�ӿں�PLL������SystemCoreClock����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void SystemInit (void)
{
  /* ��λRCCʱ�����õ�ȱʡ״̬(���ڵ���)������HSIONλ */
  RCC->CR |= (uint32_t)0x00000001;

  /* ��λSW��HPRE��PPRE1��PPRE2, ADCPRE��MCOλ */
#ifndef STM32F10X_CL
  RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
  RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif /* STM32F10X_CL */   
  
  /* ��λHSEON��CSSON��PLLONλ */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* ��λHSEBYPλ */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* ��λPLLSRC��PLLXTPRE��PLLMUL��USBPRE/OTGFSPREλ */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_CL
  /* ��λPLL2ON��PLL3ONλ */
  RCC->CR &= (uint32_t)0xEBFFFFFF;

  /* ��ֹ���е��жϣ�����жϱ�־ */
  RCC->CIR = 0x00FF0000;

  /* ��λCFGR2�Ĵ��� */
  RCC->CFGR2 = 0x00000000;
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
  /* ��ֹ���е��жϲ�����жϱ�־ */
  RCC->CIR = 0x009F0000;

  /* ��λCFGR2�Ĵ��� */
  RCC->CFGR2 = 0x00000000;      
#else
  /* ��ֹ���е��жϲ�����жϱ�־ */
  RCC->CIR = 0x009F0000;
#endif /* STM32F10X_CL */
    
#if defined (STM32F10X_HD) || (defined STM32F10X_XL) || (defined STM32F10X_HD_VL)
  #ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl(); 
  #endif /* DATA_IN_ExtSRAM */
#endif 

  /* ����ϵͳʱ��Ƶ�ʣ�HCLK��PCLK2��PCLK1Ԥ��Ƶ�� */
  /* ����Flash�������ڣ�ʹ��Ԥȡ���� */
  SetSysClock();

#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;	/* ������λ���ڲ�SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* ������λ���ڲ�Flash */
#endif 
}

/*
*********************************************************************************************************
*	�� �� ��: SystemCoreClockUpdate
*	����˵��: ����ʱ�ӼĴ���ֵ����SystemCoreClock����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0;

#ifdef  STM32F10X_CL
  uint32_t prediv1source = 0, prediv1factor = 0, prediv2factor = 0, pll2mull = 0;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
  uint32_t prediv1factor = 0;
#endif /* STM32F10X_LD_VL or STM32F10X_MD_VL or STM32F10X_HD_VL */
    
  /* ��ȡSYSCLKʱ��Դ */
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  
  switch (tmp)
  {
    case 0x00:  /* HSI ����ϵͳʱ�� */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE ����ϵͳʱ�� */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL ����ϵͳʱ�� */

      /* ��ȡPLLʱ��Դ�ͱ�Ƶ���� */
      pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
      
#ifndef STM32F10X_CL      
      pllmull = ( pllmull >> 18) + 2;
      
      if (pllsource == 0x00)
      {
        /* HSI����ʱ�ӳ���2����PLLʱ����� */
        SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {
 #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
       prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
       /* HSE����ʱ������PREDIV1ʱ����� */
       SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull; 
 #else
        /* HSE����PLLʱ����� */
        if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
        {/* HSE����ʱ�ӳ���2 */
          SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
        }
        else
        {
          SystemCoreClock = HSE_VALUE * pllmull;
        }
 #endif
      }
#else
      pllmull = pllmull >> 18;
      
      if (pllmull != 0x0D)
      {
         pllmull += 2;
      }
      else
      { /* PLL��Ƶ���� = PLL PLL multiplication factor = PLL input clock * 6.5 */
        pllmull = 13 / 2; 
      }
            
      if (pllsource == 0x00)
      {
        /* HSI ����ʱ�ӳ���2����PLLʱ����� */
        SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {/* PREDIV1����PLLʱ����� */
        
        /* ��ȡPREDIV1ʱ��Դ�ͷ�Ƶ���� */
        prediv1source = RCC->CFGR2 & RCC_CFGR2_PREDIV1SRC;
        prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
        
        if (prediv1source == 0)
        { 
          /* HSE����ʱ������PREDIV1ʱ����� */
          SystemCoreClock = (HSE_VALUE / prediv1factor) * pllmull;          
        }
        else
        {/* PLL2ʱ������PREDIV1ʱ����� */
          
          /* ��ȡPREDIV2��Ƶ���Ӻ�PLL2��Ƶ���� */
          prediv2factor = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4) + 1;
          pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8 ) + 2; 
          SystemCoreClock = (((HSE_VALUE / prediv2factor) * pll2mull) / prediv1factor) * pllmull;                         
        }
      }
#endif /* STM32F10X_CL */ 
      break;

    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  
  /* ����HCLKʱ��Ƶ�� */
  /* ��ȡHCLKԤ��Ƶ�� */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLKʱ��Ƶ�� */
  SystemCoreClock >>= tmp;  
}

/*
*********************************************************************************************************
*	�� �� ��: SetSysClock
*	����˵��: ����ϵͳʱ��Ƶ�ʣ�HCLK��PCLK2��PCLK1Ԥ��Ƶ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_24MHz
  SetSysClockTo24();
#elif defined SYSCLK_FREQ_36MHz
  SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
  SetSysClockTo56();  
#elif defined SYSCLK_FREQ_72MHz
  SetSysClockTo72();
#endif
 
  /* ��������define��û��ѡ����ôHSI����ϵͳʱ��Դ����λ���ȱʡֵ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: SystemInit_ExtMemCtl
*	����˵��: �����ⲿ�洢������������startup_stm32f10x_xx.s�ļ��б����ã�������main֮ǰ��ִ�С��������
*			���ÿ������ϵ��ⲿSRAM�������ڳ�����������������Ѻ�ջ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
#ifdef DATA_IN_ExtSRAM
void SystemInit_ExtMemCtl(void) 
{
  /* FSMC Bank1 NOR/SRAM3 �����ⲿSRAM�������Ҫ����������BANK�����޸ļĴ�����ַ */
  
  /* ʹ��FSMCʱ�� */
  RCC->AHBENR = 0x00000114;
  
  /* ʹ��GPIOD, GPIOE, GPIOF��GPIOGʱ�� */  
  RCC->APB2ENR = 0x000001E0;
  
  /* ����SRAM�������ߡ�NOE��NWE����ַ�ߡ�Ƭѡ�ߡ�NBL0��NBL1 */
  GPIOD->CRL = 0x44BB44BB;  
  GPIOD->CRH = 0xBBBBBBBB;

  GPIOE->CRL = 0xB44BB4BB;  /* ����������������PE3��PE4����ַ���� */
  GPIOE->CRH = 0xBBBBBBBB;

  GPIOF->CRL = 0x44BBBBBB;  
  GPIOF->CRH = 0xBBBB4444;

  GPIOG->CRL = 0x44BBBBBB;  
  GPIOG->CRH = 0x44444B44;
  
  /* ����FSMC��ʹ��Bank1_SRAM */
  FSMC_Bank1->BTCR[4] = 0x00001011;
  FSMC_Bank1->BTCR[5] = 0x00000200;
}
#endif /* DATA_IN_ExtSRAM */

#ifdef SYSCLK_FREQ_HSE
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockToHSE
*	����˵��: ѡ��HSE��Ϊϵͳʱ��Դ��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* ����SYSCLK, HCLK, PCLK2 �� PCLK1 */    
  /* ʹ��HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* �ȴ�HSE׼�������������ʱ���˳� */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    /* ʹ��Ԥȡ���� */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0�ȴ� */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);

#ifndef STM32F10X_CL
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
#else
    if (HSE_VALUE <= 24000000)
	{
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;
	}
	else
	{
      FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;
	}
#endif /* STM32F10X_CL */
#endif
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
    /* ѡ��HSE��Ϊϵͳʱ��Դ */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;    

    /* �ȴ�HSE�ɹ�����ϵͳʱ��Դ */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }  
}
#elif defined SYSCLK_FREQ_24MHz
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockTo24
*	����˵��: ����ϵͳʱ��Ϊ24MHz��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
static void SetSysClockTo24(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* ����SYSCLK, HCLK, PCLK2��PCLK1 */    
  /* ʹ��HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* �ȴ�HSE׼�������������ʱ���˳� */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL 
    /* ʹ��Ԥȡ���� */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0�ȴ� */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    
#endif
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
#ifdef STM32F10X_CL
    /* ����PLL */
    /* PLL���ã�PLLCLK = PREDIV1 * 6 = 24 MHz */ 
    RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL6); 

    /* PLL2����: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    /* PREDIV1���ã�PREDIV1CLK = PLL2 / 10 = 4 MHz */       
    RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
    RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                             RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10);
  
    /* ʹ��PLL2 */
    RCC->CR |= RCC_CR_PLL2ON;
    /* �ȴ�PLL2���� */
    while((RCC->CR & RCC_CR_PLL2RDY) == 0)
    {
    }   
#elif defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
    /*  PLL����:  = (HSE / 2) * 6 = 24 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1_Div2 | RCC_CFGR_PLLMULL6);
#else    
    /*  PLL����:  = (HSE / 2) * 6 = 24 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLXTPRE_HSE_Div2 | RCC_CFGR_PLLMULL6);
#endif /* STM32F10X_CL */

    /* ʹ��PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* �ȴ�PLL���� */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* ѡ��PLL��Ϊϵͳʱ��Դ */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* �ȴ�PLL�ɹ�����ϵͳʱ��Դ */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* ���HSE����ʧ�ܣ�Ӧ�ó�����һ�������ʱ�����ã��û������ڴ���Ӵ��봦��������� */
  } 
}
#elif defined SYSCLK_FREQ_36MHz
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockTo36
*	����˵��: ����ϵͳʱ��Ϊ36MHz��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
static void SetSysClockTo36(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
#ifdef STM32F10X_CL
    /* Configure PLLs ------------------------------------------------------*/
    
    /* PLL configuration: PLLCLK = PREDIV1 * 9 = 36 MHz */ 
    RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL9); 

	/*!< PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 10 = 4 MHz */
        
    RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
    RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                             RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV10);
  
    /* Enable PLL2 */
    RCC->CR |= RCC_CR_PLL2ON;
    /* Wait till PLL2 is ready */
    while((RCC->CR & RCC_CR_PLL2RDY) == 0)
    {
    }
    
#else    
    /*  PLL configuration: PLLCLK = (HSE / 2) * 9 = 36 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLXTPRE_HSE_Div2 | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  } 
}
#elif defined SYSCLK_FREQ_48MHz
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockTo48
*	����˵��: ����ϵͳʱ��Ϊ48MHz��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/   
static void SetSysClockTo48(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
#ifdef STM32F10X_CL
    /* Configure PLLs ------------------------------------------------------*/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */
        
    RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
    RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                             RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);
  
    /* Enable PLL2 */
    RCC->CR |= RCC_CR_PLL2ON;
    /* Wait till PLL2 is ready */
    while((RCC->CR & RCC_CR_PLL2RDY) == 0)
    {
    }
    
   
    /* PLL configuration: PLLCLK = PREDIV1 * 6 = 48 MHz */ 
    RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL6); 
#else    
    /*  PLL configuration: PLLCLK = HSE * 6 = 48 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);
#endif /* STM32F10X_CL */

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  } 
}

#elif defined SYSCLK_FREQ_56MHz
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockTo56
*	����˵��: ����ϵͳʱ��Ϊ48MHz��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/   
static void SetSysClockTo56(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/   
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
    /* Configure PLLs ------------------------------------------------------*/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */
        
    RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
    RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                             RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);
  
    /* Enable PLL2 */
    RCC->CR |= RCC_CR_PLL2ON;
    /* Wait till PLL2 is ready */
    while((RCC->CR & RCC_CR_PLL2RDY) == 0)
    {
    }
    
   
    /* PLL configuration: PLLCLK = PREDIV1 * 7 = 56 MHz */ 
    RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL7); 
#else     
    /* PLL configuration: PLLCLK = HSE * 7 = 56 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL7);

#endif /* STM32F10X_CL */

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  } 
}

#elif defined SYSCLK_FREQ_72MHz
/*
*********************************************************************************************************
*	�� �� ��: SetSysClockTo72
*	����˵��: ����ϵͳʱ��Ϊ72MHz��������HCLK��PCLK2��PCLK1Ԥ��Ƶ��������������ڸ�λ��ִ��һ�Ρ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/   
static void SetSysClockTo72(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

#ifdef STM32F10X_CL
    /* Configure PLLs ------------------------------------------------------*/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    /* PREDIV1 configuration: PREDIV1CLK = PLL2 / 5 = 8 MHz */
        
    RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                              RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
    RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                             RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);
  
    /* Enable PLL2 */
    RCC->CR |= RCC_CR_PLL2ON;
    /* Wait till PLL2 is ready */
    while((RCC->CR & RCC_CR_PLL2RDY) == 0)
    {
    }
    
   
    /* PLL configuration: PLLCLK = PREDIV1 * 9 = 72 MHz */ 
    RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | 
                            RCC_CFGR_PLLMULL9); 
#else    
    /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif /* STM32F10X_CL */

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }
}
#endif
