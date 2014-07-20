/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��
*	�ļ����� : bsp_button.c
*	˵    �� : ʵ�ְ����ļ�⣬��������˲����ƣ����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*				(5) ��ϼ�
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include <stdio.h>

#include "bsp_button.h"

static BUTTON_T s_BtnUser;		/* USER �� */
static BUTTON_T s_BtnTamper;	/* TAMPER �� */
static BUTTON_T s_BtnWakeUp;	/* WAKEUP �� */

static BUTTON_T s_BtnUserTamper;/* ��ϼ���USER��TAMPER�� */

static KEY_FIFO_T s_Key;		/* ����FIFO����,�ṹ�� */

static void bsp_InitButtonVar(void);
static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

/*
	�������߷��䣺
	USER1��     : PC6  (�͵�ƽ��ʾ����)
	TAMPEER��  : PC13 (�͵�ƽ��ʾ����)
	WKUP��     : PA0  (!!!�ߵ�ƽ��ʾ����)

	���庯���жϰ����Ƿ��£�����ֵ1 ��ʾ���£�0��ʾδ����
*/
static uint8_t IsKeyDownUser(void) 		{if (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownTamper(void) 	{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET) return 0; return 1;}
//static uint8_t IsKeyDownWakeUp(void) 	{if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) return 1; return 0;}
static uint8_t IsKeyDownUserTamper(void) {if (IsKeyDownUser() && IsKeyDownTamper()) return 0; return 1;}	/* ��ϼ� */

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButton
*	����˵��: ��ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitButton(void)
{
	bsp_InitButtonVar();		/* ��ʼ���������� */
	bsp_InitButtonHard();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    �Σ�_KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];

		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonHard
*	����˵��: ��ʼ������Ӳ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*
	�������߷��䣺
	USER1��     : PC6  (�͵�ƽ��ʾ����)
	TAMPEER��  : PC13 (�͵�ƽ��ʾ����)
	WKUP��     : PA0  (!!!�ߵ�ƽ��ʾ����)
	*/
			
	/* ��1������GPIOA GPIOC GPIOD GPIOF GPIOG��ʱ��
	   ע�⣺����ط�����һ����ȫ��
	*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC
//			| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG, ENABLE);
	
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
/*	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/	/* PA0 */
/*	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStructure);*/	/* PC13 */
/*	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);*/	/* PC6 */
}
	
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitButtonVar
*	����˵��: ��ʼ����������
*	��    �Σ�strName : ���������ַ���
*			  strDate : ���̷�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitButtonVar(void)
{
	/* �԰���FIFO��дָ������ */
	s_Key.Read = 0;
	s_Key.Write = 0;

	/* ��ʼ��USER����������֧�ְ��¡����𡢳��� */
	s_BtnUser.IsKeyDownFunc = IsKeyDownUser;		/* �жϰ������µĺ��� */
	s_BtnUser.FilterTime = BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnUser.LongTime = BUTTON_LONG_TIME;			/* ����ʱ�� */
	s_BtnUser.Count = s_BtnUser.FilterTime / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnUser.State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnUser.KeyCodeDown = KEY_DOWN_USER;			/* �������µļ�ֵ���� */
	s_BtnUser.KeyCodeUp = KEY_UP_USER;				/* ��������ļ�ֵ���� */
	s_BtnUser.KeyCodeLong = KEY_LONG_USER;			/* �������������µļ�ֵ���� */
	s_BtnUser.RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnUser.RepeatCount = 0;						/* ���������� */		

	/* ��ʼ��TAMPER����������֧�ְ��� */
	s_BtnTamper.IsKeyDownFunc = IsKeyDownTamper;	/* �жϰ������µĺ��� */
	s_BtnTamper.FilterTime = BUTTON_FILTER_TIME;	/* �����˲�ʱ�� */
	s_BtnTamper.LongTime = 0;						/* ����ʱ��, 0��ʾ����� */
	s_BtnTamper.Count = s_BtnTamper.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnTamper.State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnTamper.KeyCodeDown = KEY_DOWN_TAMPER;		/* �������µļ�ֵ���� */
	s_BtnTamper.KeyCodeUp = 0;						/* ��������ļ�ֵ���� */
	s_BtnTamper.KeyCodeLong = 0;					/* �������������µļ�ֵ���� */
	s_BtnTamper.RepeatSpeed = 0;					/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnTamper.RepeatCount = 0;					/* ���������� */	

	/* ��ʼ��WAKEUP����������֧�ְ��� */
//	s_BtnWakeUp.IsKeyDownFunc = IsKeyDownWakeUp;	/* �жϰ������µĺ��� */
	s_BtnWakeUp.FilterTime = BUTTON_FILTER_TIME;	/* �����˲�ʱ�� */
	s_BtnWakeUp.LongTime = 0;						/* ����ʱ�� */
	s_BtnWakeUp.Count = s_BtnWakeUp.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnWakeUp.State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnWakeUp.KeyCodeUp = 0;						/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnWakeUp.KeyCodeDown = KEY_DOWN_WAKEUP;		/* �������µļ�ֵ���� */
	s_BtnWakeUp.KeyCodeLong = 0;					/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnWakeUp.RepeatSpeed = 10;					/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnWakeUp.RepeatCount = 0;					/* ���������� */		

	/* ��ʼ����ϰ���������֧�ְ��� */
	s_BtnUserTamper.IsKeyDownFunc = IsKeyDownUserTamper;	/* �жϰ������µĺ��� */
	s_BtnUserTamper.FilterTime = BUTTON_FILTER_TIME;		/* �����˲�ʱ�� */
	s_BtnUserTamper.LongTime = 0;							/* ����ʱ�� */
	s_BtnUserTamper.Count = s_BtnUserTamper.FilterTime / 2;	/* ����������Ϊ�˲�ʱ���һ�� */
	s_BtnUserTamper.State = 0;								/* ����ȱʡ״̬��0Ϊδ���� */
	s_BtnUserTamper.KeyCodeDown = KEY_DOWN_USER_TAMPER;		/* �������µļ�ֵ���� */
	s_BtnUserTamper.KeyCodeUp = 0;							/* ��������ļ�ֵ���룬0��ʾ����� */
	s_BtnUserTamper.KeyCodeLong = 0;						/* �������������µļ�ֵ���룬0��ʾ����� */
	s_BtnUserTamper.RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
	s_BtnUserTamper.RepeatCount = 0;						/* ���������� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectButton
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    �Σ������ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectButton(BUTTON_T *_pBtn)
{
	/* ���û�г�ʼ�������������򱨴�
	if (_pBtn->IsKeyDownFunc == 0)
	{
		printf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
	}
	*/

	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeDown);
				}
			}

			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey(_pBtn->KeyCodeLong);						
					}
				}
				else
				{
					if (_pBtn->RepeatSpeed > 0)
					{
						if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
						{
							_pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey(_pBtn->KeyCodeDown);														
						}
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* ��ֵ���밴��FIFO */
					bsp_PutKey(_pBtn->KeyCodeUp);			
				}
			}
		}

		_pBtn->LongCount = 0;
		_pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyPro
*	����˵��: ������а�����������״̬�����뱻�����Եĵ��á�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyPro(void)
{
	bsp_DetectButton(&s_BtnUser);		/* USER �� */
	bsp_DetectButton(&s_BtnTamper);		/* TAMPER �� */
	bsp_DetectButton(&s_BtnWakeUp);		/* WAKEUP �� */
	bsp_DetectButton(&s_BtnUserTamper);	/* ��ϼ� */
}
