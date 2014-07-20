#ifndef __EXTSRAM_H
#define __EXTSRAM_H
#include "stm32f10x.h"


/*外扩sram地址*/
#define EXT_SRAM_ADDR    ((u32)0x68000000)
/*内存读写方法
  Addr 为偏移地址，0-12287(512K字节)
 
  写内存例子: *(u16 *) (EXT_SRAM_ADDR + Addr) = 1;
  读内存例子: u16 val = *(__IO u16*) (EXT_SRAM_ADDR + Addr);
 
 
 具体内存中存放的信息是什么，由使用者自己自由使用分配，请灵活使用指针
*/
#define fpga_write(offset, data) *((volatile u16*)(EXT_SRAM_ADDR + offset)) = data

#define fpga_read(offset)       *((volatile u16*)(EXT_SRAM_ADDR + offset))

//设置马达速度
//number轴号 0对应起始马达 
//speed mm/s 马达速度
void MotorSetSpeed(int number,float speed);

/*外扩sram初始化*/
void BSP_ExtSramInit(void);


#endif

