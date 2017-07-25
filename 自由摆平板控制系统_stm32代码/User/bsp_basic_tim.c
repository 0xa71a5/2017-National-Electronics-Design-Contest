/**
  ******************************************************************************
  * @file    bsp_TIM6.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   基本定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
/**
  *基本定时器 TIM6 TIM7
  *通用定时器 TIM2 TIM3 TIM4 TIM5 TIM9 TIM10 TIM11 TIM12 TIM13 TIM14
  *高级定时器 TIM1 TIM8
*/

#include "bsp_basic_tim.h"
u8 ledState=0;
 /**
  * @brief  基本定时器 TIMx,x[6,7]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIM6_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM7_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;   
    // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Init time7
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;   
    NVIC_Init(&NVIC_InitStructure);
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
static void TIM6_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 2000-1;       
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=84MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;	
	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	TIM_Cmd(TIM6, ENABLE);	
  TIM6->DIER &= (uint16_t)~TIM_IT_Update;//禁止中断6服务
}

static void TIM7_Mode_Config(void)
{
  //
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/    
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 1000-1;   

  //10=1ms     one time
  //5000=500ms one time     
  //10000=1000ms one time

  //定时器时钟源TIMxCLK = 2 * PCLK1  
  //        PCLK1 = HCLK / 4 
  //        => TIMxCLK=HCLK/2=SystemCoreClock/2=84MHz
  // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1; //8400 1Mhz;840 10Mhz;84 100Mhz
                                               //8400 Period/10 ms
                                               //840  Period/100 ms
                                               //84   Period us 
  // 初始化定时器TIMx, x[2,3,4,5]
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  
  
  // 清除定时器更新中断标志位
  TIM_ClearFlag(TIM7, TIM_FLAG_Update);
  
  // 开启定时器更新中断
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
  
  // 使能定时器
  TIM_Cmd(TIM7, ENABLE);  
  //TIM7->DIER &= (uint16_t)~TIM_IT_Update;//禁止中断7服务
}



/**
  * @brief  初始化基本定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
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



/*********************************************END OF FILE**********************/
