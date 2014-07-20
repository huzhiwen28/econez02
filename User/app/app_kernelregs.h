#ifndef __KERNELREGS_H
#define __KERNELREGS_H
#include <ucos_ii.h>
#include "stm32f10x.h"

//��Ҫ������ڴ�����
#define NEEDSAVENUM 71

//ÿ�����ƵļĴ�������
#define REGSNUM 111

//�������Ĵ���
extern volatile short KernelRegs[];
extern OS_EVENT *RegsWriteMutex;//д��
//���ܴ�6���ֽڵ�seed����2���ֽڵ�crc��
extern u8 Encryption[8];

//д���ܴ���ʶ	 0��д 1д
extern u8 WriteEncry;


//�Ĵ���״̬�޸ı�ʶ 0û�ı� 1�ı�
extern char RegsFlag[];


//�Ĵ������ݳ�ʼ��
void App_RegsInit(void);

//�Ĵ������ݺ�̨ˢ������
void App_RgesRun(void *p_arg);

void  ParamInit(void);

#endif
