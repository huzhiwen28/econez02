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
