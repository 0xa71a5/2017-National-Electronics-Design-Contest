#include "stepper.h"
#include "bsp_led.h"
uint8_t Stepper1_State=0;
int32_t Stepper1_Steps=0;
uint8_t Stepper1_RunFlag=0;


uint8_t Stepper2_State=0;
int32_t Stepper2_Steps=0;
uint8_t Stepper2_RunFlag=0;

//Stepper1: 黄线 pulse PE2 ;  橙线 dir PD3; 红线 vcc ; 棕线 en  PD7
//Stepper2: 绿线 pulse PB15;  黄线 dir PA2; 橙线 vcc ; 红线 en  PA3  


void Stepper_Init()
{
    //初始化6个引脚，全都是GPIO推挽输出
    //对于步进电机1 ： En->PD7  Dir->PD3  Pulse->PE2
    //对于步进电机2 :  En->PA3  Dir->PA2  Pulse->PB0
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC| RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 
    
    /* Stepper1 gpio init */
    GPIO_InitStructure.GPIO_Pin = Stepper1_En_Pin;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(Stepper1_En_Port, &GPIO_InitStructure);   
    
    GPIO_InitStructure.GPIO_Pin = Stepper1_Dir_Pin;
    GPIO_Init(Stepper1_Dir_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Stepper1_Pulse_Pin;
    GPIO_Init(Stepper1_Pulse_Port, &GPIO_InitStructure);


    /* Stpper2 gpio init */
    
    GPIO_InitStructure.GPIO_Pin = Stepper2_En_Pin;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(Stepper2_En_Port, &GPIO_InitStructure);   
    GPIO_InitStructure.GPIO_Pin = Stepper2_Dir_Pin;
    GPIO_Init(Stepper2_Dir_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = Stepper2_Pulse_Pin;
    GPIO_Init(Stepper2_Pulse_Port, &GPIO_InitStructure);    
    
    
    Stepper1_Dir(0);
    Stepper1_En(0);//失能步进电机1
    Stepper1_Pulse(1);


    Stepper2_Dir(0);
    Stepper2_En(0);//失能步进电机2
    Stepper2_Pulse(1);

    TIM7_Configuration();//初始化定时器7 控制步进电机1
    TIM2_Configuration();//初始化定时器2 控制步进电机2
    TIM7->DIER &= (uint16_t)~TIM_IT_Update;//禁止中断7服务
    TIM2->DIER &= (uint16_t)~TIM_IT_Update;//禁止中断2服务
}

void Stepper1_SetSpeed(s32 steps,u32 speed)
{
    //Stepper1_Speed(speed);
		if(steps<0)
		{
			steps=-steps;
			Stepper1_Dir(0);
		}
		else
		{
			Stepper1_Dir(1);
		}
    Stepper1_En(1);
    TIM7->ARR=speed;//设置转速为speed 单位us
    Stepper1_Steps=steps*2;//设置剩余步数为steps 脉冲数量是steps的两倍
    TIM7->DIER |= TIM_IT_Update;//使能中断7服务
    
}

void Stepper2_SetSpeed(s32 steps,u32 speed)
{
		if(steps<0)
		{
			steps=-steps;
			Stepper2_Dir(0);
		}
		else
		{
			Stepper2_Dir(1);
		}
		Stepper2_En(1);
    TIM2->ARR=speed;//设置转速为speed 单位us
    Stepper2_Steps=steps*2;//设置剩余步数为steps
    TIM2->DIER |= TIM_IT_Update;//使能中断2服务
}

void Stepper_Wait()
{
    while(Stepper1_Steps>0||Stepper2_Steps>0)
    {
        //printf("M Steps1=%d ;Steps2=%d\n",Stepper1_Steps,Stepper2_Steps);
        delay_us(50);
    }
		#ifdef UNLOCK
		Stepper1_En(0);
		Stepper2_En(0);
		#endif
    //TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE);
    TIM7->DIER &= (uint16_t)~TIM_IT_Update;//禁止中断7服务
}


void TIM7_IRQHandler(void)//定时器7中断 控制Stepper1的脉冲
{
 //判断是否是TIM7中断
 if (((TIM7->SR & TIM_IT_Update) != 0 ) && ((TIM7->DIER & TIM_IT_Update) != 0))
  { 
    if(Stepper1_Steps>0)//判断当前是否运行步进  并且 步进剩余步数是否不为0
    {
        Stepper1_State=~Stepper1_State;//步进脉冲
        Stepper1_Pulse(Stepper1_State);//步进脉冲
        Stepper1_Steps--;//剩余步数自减
        LED1_ON();//debug   
    }
		#ifdef UNLOCK
		else
		{       
				Stepper1_En(0);//走完之后关闭电流
		}
		#endif
    TIM7->SR = (uint16_t)~TIM_IT_Update; //清除中断位
  }     
}

void  TIM2_IRQHandler(void)//定时器2中断 控制Stepper2的脉冲
{
//判断是否是TIM2中断
 if (((TIM2->SR & TIM_IT_Update) != 0 ) && ((TIM2->DIER & TIM_IT_Update) != 0))
  { 
    if(Stepper2_Steps>0)//判断当前是否运行步进  并且 步进剩余步数是否不为0
    {
        Stepper2_State=~Stepper2_State;//步进脉冲
        Stepper2_Pulse(Stepper2_State);//步进脉冲
        Stepper2_Steps--;//剩余步数自减
        LED1_ON();//debug 
    }  
		#ifdef UNLOCK
		else
		{
			Stepper2_En(0);//走完之后关闭电流
		}
		#endif
    TIM2->SR = (uint16_t)~TIM_IT_Update; //清除中断位  
  }     
}

