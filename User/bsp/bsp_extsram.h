#ifndef __EXTSRAM_H
#define __EXTSRAM_H
#include "stm32f10x.h"


/*����sram��ַ*/
#define EXT_SRAM_ADDR    ((u32)0x60000000)


#define fpga_write(offset, data) *((volatile u8*)(EXT_SRAM_ADDR + (offset << 16))) = data

#define fpga_read(offset)       *((volatile u8*)(EXT_SRAM_ADDR + (offset << 16)))

/*�ڴ��д����
  Addr Ϊƫ�Ƶ�ַ��0-12287(512K�ֽ�)
 
  д�ڴ�����: *(u16 *) (EXT_SRAM_ADDR + Addr) = 1;
  ���ڴ�����: u16 val = *(__IO u16*) (EXT_SRAM_ADDR + Addr);
 
 
 �����ڴ��д�ŵ���Ϣ��ʲô����ʹ�����Լ�����ʹ�÷��䣬�����ʹ��ָ��
*/
/*����sram��ʼ��*/
void BSP_ExtSramInit(void);

//��������ٶ�
//number��� 0��Ӧ��ʼ��� 
//speed mm/s ����ٶ�
void MotorSetSpeed(int number,double speed);

//����ٶ���Ч���������Ա�֤��������ٶ�һ�µ�ˢ��
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ������ٶ�ֵˢ��
void MotorSpeedFlush(u8 bits);

//���PWM�����Ч
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ������������,0��ʾ��Ӧ�����ֹͣ�������
void MotorPwmout(u8 bits);

//��ﵱǰ��������ˢ�£�����stm32��������������
//bits�е�λ��Ӧ��Ӧ����bit0-7��ʾ���1�����8��λֵΪ1��ʾ��Ӧ�������������ˢ��
void MotorOutCntFlush(u8 bits);

//��ȡ��ﵱǰ���͵���������
//number��� 0��Ӧ��ʼ��� 
int MotorReadOutCnt(int number);

void Motorpingpong(void);
#endif

