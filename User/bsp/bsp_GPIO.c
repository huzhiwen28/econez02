
/*GPIO��ʼ��
����
2012 03 27
*/

#include "stm32f10x.h"
#include <stdio.h>
#include <bsp_GPIO.h>


 /*************************************************
����: void GPIO_Configuration(void��
����: GPIOE����		ָʾ�����
A0:				  �̵���1
A1:	              DOUT1
����: ��
����: ��
 */

/*******************************************************************************
	��������GPIO_Configuration
	��  ��:
	��  ��:
	����˵�������������
	
 	��  ����߷���
	IN1	  PG8
	IN2	  PG9
	IN3	  PG11
	IN4	  PG12
	IN5	  PG13
	IN6	  PG7

*/
/**************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;        //����GPIO��ʼ���ṹ��
 
   /* Enable GPIOE clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  //����GPIOE��AFIOʱ��

  /* Configure PE.2,PE.3,PE.4,PE.5, as Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

     /* Configure PC.10,PC.11as Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PA.0,PA.1as Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

      /* Configure PG.14,PG.15as Output push-pull */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOG, &GPIO_InitStructure);


  GPIO_ResetBits(GPIOE, GPIO_Pin_2);		 //�����
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);		 //�����
  GPIO_ResetBits(GPIOE, GPIO_Pin_4);		 //�����
  GPIO_ResetBits(GPIOE, GPIO_Pin_5);		 //�����

 GPIO_SetBits(GPIOA, GPIO_Pin_0);		 //�����,�Ͽ�
 GPIO_SetBits(GPIOA, GPIO_Pin_1);		 //����ߣ��Ͽ�


  /*
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);  //����GPIOE��AFIOʱ��
*/
  /* Configure PE.2,PE.3,PE.4,PE.5, as Output push-pull */ 
/*  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
  GPIO_Init(GPIOE, &GPIO_InitStructure); 



 GPIO_SetBits(GPIOF, GPIO_Pin_10);		 //�����
 GPIO_SetBits(GPIOF, GPIO_Pin_11);		 //�����


 */


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	//IO ����ڳ��Ի�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	/* PB5 */
				   
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	/* PF6 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	/* PF7 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	/* PF8 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	/* PF9 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	/* PC12 */

	//ID���뿪��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	/* PC12 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	/* PC12 */
}






