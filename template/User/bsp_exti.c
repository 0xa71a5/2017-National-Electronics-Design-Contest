/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_exti.h"
#include "bsp_led.h"


 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
    
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  /* ������ռ���ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* �����ж�Դ������2������ʹ������������� */  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC ,ENABLE);//���������ֱ���
  
  /* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* ���� NVIC */
  NVIC_Configuration();
  
	/* ѡ�񰴼�1������ */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  /* ��������Ϊ����ģʽ */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
  /* �������Ų�����Ҳ������ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  /* ʹ������Ľṹ���ʼ������ */
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ ��key1���� */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

  /* ѡ�� EXTI �ж�Դ */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  /* �ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* �½��ش��� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  /* ʹ���ж�/�¼��� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* ѡ�񰴼�2������ */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  
  /* ����������������ͬ */
  GPIO_Init(GPIOC, &GPIO_InitStructure);      

	/* ���� EXTI �ж�Դ ��key2 ���� */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource13);

  /* ѡ�� EXTI �ж�Դ */
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* �����ش��� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line0) != RESET) 
  {
    LED1_TOGGLE();
    EXTI_ClearITPendingBit(EXTI_Line0);     
  }  
}

void EXTI15_10_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line13) != RESET) 
  {
    LED2_TOGGLE();
    EXTI_ClearITPendingBit(EXTI_Line13);     
  }  
}
/*********************************************END OF FILE**********************/
