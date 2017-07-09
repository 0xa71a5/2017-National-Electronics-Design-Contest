/**
  ******************************************************************************
  * @file    bsp_TIM3.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   通用定时器PWM输出范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_tim3_pwm.h"
/*
    TIM3_CH1 --->  PB4 PC6 
    TIM3_CH2 --->  PB5 PC7
    TIM3_CH3 --->  PB0 PC8
    TIM3_CH4 --->  PB1 PC9
*/

static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE); 
  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);  //ch1
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);  //ch2
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);  //ch3
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);  //ch4
  
  
	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_PWMOUTPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 999;       
	
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 83;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 100;//300/1000占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);


  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 300;//300/1000占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);   //使能通道1
  /*使能通道1重载*/
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 500;//300/1000占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);   //使能通道1
  /*使能通道1重载*/
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_Pulse = 700;//300/1000占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);   //使能通道1
  /*使能通道1重载*/
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);


	// 使能定时器
	TIM_Cmd(TIM3, ENABLE);	
}

/**
  * @brief  初始化控制通用定时器
  * @param  无
  * @retval 无
  */
void TIM3_PWM_Configuration(void)
{
	TIMx_GPIO_Config();
  
  TIM_PWMOUTPUT_Config();
}

/*********************************************END OF FILE**********************/
