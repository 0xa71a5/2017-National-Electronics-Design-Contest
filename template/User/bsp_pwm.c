/**
  ******************************************************************************
  * @file    bsp_TIM3.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ͨ�ö�ʱ��PWM�������
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


#include "bsp_pwm.h"
/*  PWM���ŷ���
    TIM3_CH1 --->  PB4 PC6 
    TIM3_CH2 --->  PB5 PC7
    TIM3_CH3 --->  PB0 PC8
    TIM3_CH4 --->  PB1 PC9
    
    TIM4_CH1 --->  PB6 PD12
    TIM4_CH2 --->  PB7 PD13
    TIM4_CH3 --->  PB8 PD14
    TIM4_CH4 --->  PB9 PD15

    TIM9_CH1 --->  PE5
    TIM9_CH2 --->  PE6

    TIM12_CH1 -->  PB14
    TIM12_CH2 -->  PB15    
*/

static void TIM3_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE); 
  /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);  //ch1
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);  //ch2
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);  //ch3
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);  //ch4
  
  
	/* ��ʱ��ͨ���������� */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
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
static void TIM3_PWMOUTPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 999;       
	
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 83;	
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*PWMģʽ����*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�   TIM3->CCR1 = TIM_OCInitStruct->TIM_Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //ʹ��ͨ��1  
	/*ʹ��ͨ��1����*/
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);


  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);


	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM3, ENABLE);	
}

/**
  * @brief  ��ʼ������ͨ�ö�ʱ��
  * @param  ��
  * @retval ��
  */
void TIM3_PWM_Configuration(void)
{
	TIM3_GPIO_Config();
  TIM3_PWMOUTPUT_Config();
}

/****************************************** T I M 4 ******************************/
/*
    TIM4_CH1 --->  PB6 PD12
    TIM4_CH2 --->  PB7 PD13
    TIM4_CH3 --->  PB8 PD14
    TIM4_CH4 --->  PB9 PD15
*/
static void TIM4_GPIO_Config(void) 
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;

  /*������ص�GPIO����ʱ��*/
  RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE); 
  /* ��ʱ��ͨ�����Ÿ��� */
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);  //ch1
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);  //ch2
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4);  //ch3
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4);  //ch4
  
  /* ��ʱ��ͨ���������� */                                
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode       TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM4_PWMOUTPUT_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  // ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/    
  //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 999;       
  
  // ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 83; 
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  
  // ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  /*PWMģʽ����*/
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 100;//300/1000ռ�ձ�   TIM3->CCR1 = TIM_OCInitStruct->TIM_Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);   //ʹ��ͨ��1  
  /*ʹ��ͨ��1����*/
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);


  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 0;//300/1000ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);   //ʹ��ͨ��1
  /*ʹ��ͨ��1����*/
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  // ʹ�ܶ�ʱ��
  TIM_Cmd(TIM4, ENABLE);  
}

/**
  * @brief  ��ʼ������ͨ�ö�ʱ��
  * @param  ��
  * @retval ��
  */
void TIM4_PWM_Configuration(void)
{
  TIM4_GPIO_Config();
  TIM4_PWMOUTPUT_Config();
}

/*********************************************END OF FILE**********************/
