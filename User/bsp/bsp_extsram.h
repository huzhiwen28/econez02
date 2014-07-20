#ifndef __EXTSRAM_H
#define __EXTSRAM_H
#include "stm32f10x.h"


/*����sram��ַ*/
#define EXT_SRAM_ADDR    ((u32)0x68000000)
/*�ڴ��д����
  Addr Ϊƫ�Ƶ�ַ��0-12287(512K�ֽ�)
 
  д�ڴ�����: *(u16 *) (EXT_SRAM_ADDR + Addr) = 1;
  ���ڴ�����: u16 val = *(__IO u16*) (EXT_SRAM_ADDR + Addr);
 
 
 �����ڴ��д�ŵ���Ϣ��ʲô����ʹ�����Լ�����ʹ�÷��䣬�����ʹ��ָ��
*/
#define fpga_write(offset, data) *((volatile u16*)(EXT_SRAM_ADDR + offset)) = data

#define fpga_read(offset)       *((volatile u16*)(EXT_SRAM_ADDR + offset))

//��������ٶ�
//number��� 0��Ӧ��ʼ��� 
//speed mm/s ����ٶ�
void MotorSetSpeed(int number,float speed);

/*����sram��ʼ��*/
void BSP_ExtSramInit(void);


#endif

