#ifndef __EEPROM_H
#define __EEPROM_H 

/* Includes ------------------------------------------------------------------*/	   
#include <ucos_ii.h>
#include "stm32f10x.h"
#include "bsp_fifo.h"
/*EEPROMͨ��I2C����
  ʹ��IOģ�ⷽ����дeeprom
  ��ֲ�Ա���Ƽ�
*/

/*EEPROM�豸*/
struct _EEPROM{
	  /*�豸�Ƿ��*/
	  u8 active;
      OS_EVENT *IICMutex;//д��
};

/*EEPROM��ʼ��*/
void BSP_EEPROMInit(void);

/*EEPROM��*/
void BSP_EEPROMOpen(void);

/*EEPROM�ر�*/
void BSP_EEPROMClose(void);

/*******************************************************************************
* Function Name  : BSP_EEPROMWriteByte
* Description    : дһ�ֽ�����
* Input          : - SendByte: ��д������
*           	   - WriteAddress: ��д���ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
*******************************************************************************/           
u8 BSP_EEPROMWriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress);	 //


/*******************************************************************************
* Function Name  : BSP_EEPROMReadByte
* Description    : ��ȡһ������
* Input          : - pBuffer: ��Ŷ�������
*           	   - length: ����������
*                  - ReadAddress: ��������ַ
*                  - DeviceAddress: ��������
* Output         : None
* Return         : ����Ϊ:=1�ɹ�����,=0ʧ��
*******************************************************************************/          
u8 BSP_EEPROMReadByte(u8* pBuffer,   u16 length,   u16 ReadAddress,  u8 DeviceAddress);

/*����ʱ��ȡeeprom�ӿ���*/
u8 BSP_EEPROMReadByte2(u8* pBuffer,   u16 length,   u16 ReadAddress,  u8 DeviceAddress);

u8 SendWordEEPROM(u16 Adss,u16 SData);
u16 ReadWordEEPROM(u16 Adss);

#endif

