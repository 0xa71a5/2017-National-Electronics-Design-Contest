/**
  ******************************************************************************
  * @file    bsp_TIM6.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ������ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
/**
  *������ʱ�� TIM6 TIM7
  *ͨ�ö�ʱ�� TIM2 TIM3 TIM4 TIM5 TIM9 TIM10 TIM11 TIM12 TIM13 TIM14
  *�߼���ʱ�� TIM1 TIM8
*/

#include "bsp_basic_tim.h"
u8 ledState=0;
 /**
  * @brief  ������ʱ�� TIMx,x[6,7]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIM6_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM7_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;   
    // ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Init time7
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;   
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM6_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 5000-1;       
	
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=84MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM6, ENABLE);	
}

static void TIM7_Mode_Config(void)
{
  //
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/    
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 5000-1;       
  
  //��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //        PCLK1 = HCLK / 4 
  //        => TIMxCLK=HCLK/2=SystemCoreClock/2=84MHz
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; 
  
  // ��ʼ����ʱ��TIMx, x[2,3,4,5]
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  
  
  // �����ʱ�������жϱ�־λ
  TIM_ClearFlag(TIM7, TIM_FLAG_Update);
  
  // ������ʱ�������ж�
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
  
  // ʹ�ܶ�ʱ��
  TIM_Cmd(TIM7, ENABLE);  
}



/**
  * @brief  ��ʼ��������ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIM6_Configuration(void)
{
	TIM6_NVIC_Configuration();	
  TIM6_Mode_Config();
}

void TIM7_Configuration(void)
{
  TIM7_NVIC_Configuration();  
  TIM7_Mode_Config();
}

void TIM6_DAC_IRQHandler(void)
{
  if ( TIM_GetITStatus( TIM6, TIM_IT_Update) != RESET ) 
  { 
    ledState=~ledState;
    PFout(6)=ledState;
    TIM_ClearITPendingBit(TIM6 , TIM_IT_Update);      
  }     
}

void TIM7_IRQHandler(void)
{
  if ( TIM_GetITStatus( TIM7, TIM_IT_Update) != RESET ) 
  { 
    ledState=~ledState;
    PFout(6)=ledState;
    TIM_ClearITPendingBit(TIM7 , TIM_IT_Update);      
  }     
}
/*********************************************END OF FILE**********************/
