

/*++++++++++++++++++++++++++++++++++++++
Action
��ѭ��
+++++++++++++++++++++++++++++++++++++++*/

#ifndef _APP_ACTION_H
#define _APP_ACTION_H

#include "stm32f10x.h"
#include "app_basicdef.h"
#include "app_pid.h"

extern uint AAA;

extern uint KFLG;

//��ʼ����
void StartAction(void);
//�㶯����
void JogAction(void);

//�㶯ֹͣ
void JogStop(void);

//��������
void ChangAction(uint S);			//�����ź�

void IoOutSwitch(uint S);		//IO�����������

void OutSaveSet(uint S);		//�������

void IOJog(uint S);

//����
void IOSpeedUp(uint S);
//����
void IOSpeedDown(uint S);

//A������ٶ�
void RollASpeed(void);

//B������ٶ�
void RollBSpeed(void);

//��ǣ���ٶ�
void MainSpeed(void);

//ģ�������
void OutPut(void);

//�������
void DISOutPut(void);


//�����л�
void RunSwitch( uint RunS);

//����
void IOInput(void);

void  MainMotFR(uint RunS);

//��ʼ����
void StartIOInput(void);

void  DaiClear(void);	  //��������0
void DiaSave(void);

void DDInput(void);

				//����˲�

#endif




