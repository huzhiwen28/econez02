#ifndef __APP_USBMODBUSSLAVE_H
#define __APP_USBMODBUSSLAVE_H
#include "stm32f10x.h"

//modbus slave��������
void USB_ModbusSlave_ProcessFrame(u8* chardata,int len);

#endif
