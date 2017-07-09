#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "bsp_ili9806g_lcd.h"
#include "stdio.h"
#include "bsp_led.h"
#include "bsp_tim2.h"
#include "bsp_tim3.h"
#include "bsp_pwm.h"
#include "bsp_usart.h"
#include "tm1638.h"

u32 i;
char display[20];

#define Stepper_Dir(x)	PDout(3)=x
#define Stepper_En(x) PDout(7)=x

void Stepper_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC| RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_7;	
	/*设置引脚模式为输出模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	/*设置引脚为上拉模式*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*设置引脚速率为2MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	/*设置引脚为上拉模式*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*设置引脚速率为2MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);	//PE2
	Stepper_Dir(1);
	Stepper_En(0);
}



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Init nvic priority group.16 preemption priorities.
	SysTick_Init();//1us precision
	ILI9806G_Init();//Init lcd screen
	LED_Init();
	//TIM7_Configuration();
	//EXTI_Config();
	//TIM3_Configuration();
	Stepper_Init();
	//TIM3_PWM_Configuration();
	//TIM4_PWM_Configuration();
	Serial_Init(115200);
	Stepper_En(1);
	Stepper_Dir(1);
	printf("Running...");
	for(i=0;i<2000;i++)
	{
		PEout(2)=0;
		delay_us(500);
		PEout(2)=1;
		delay_us(500);	
	}
	Stepper_En(0);
	while(1)
	{

	}
} 

/*********************************************END OF FILE**********************/

