

#ifndef _APP_PORT11_H
#define _APP_PORT11_H

#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"
#include "bsp_TimesInt.h"
#include "app_kernelregs.h"
#include "bsp_eeprom.h"

void UsbState(void );
void APortInput(void);				//ģ���������

void APortOutput(void);				//ģ���������

void App_Paramload(void);
void  ADCTsetS();
void  ADCZeroS();

uint  InputFilter(uint *InAddress);				//����˲�

uint  OutFilter(u16);

#endif



