#ifndef __APP_485MODBUSSLAVE_H
#define __APP_485MODBUSSLAVE_H
#include "stm32f10x.h"

//֪ͨmodbus master��������֡
void C485_ModbusSlave_ProcessFrame(u8* chardata,int len);

#endif
