#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "bsp_ili9806g_lcd.h"
#include "stdio.h"
#include "bsp_led.h"
#include "bsp_tim2.h"
#include "bsp_tim3_pwm.h"

u32 i;
char display[20];


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Init nvic priority group.16 preemption priorities.
	SysTick_Init();//1us precision
	ILI9806G_Init();//Init lcd screen
	LED_Init();
	//TIM7_Configuration();
	EXTI_Config();
	//TIM3_Configuration();
	TIM3_PWM_Configuration();
	while(1)
	{
		for(i=0;i<999;i+=10)
		{
			TIM3->CCR1=i;
			delay(50);
		}
	}
} 

/*********************************************END OF FILE**********************/

