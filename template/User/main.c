#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "bsp_ili9806g_lcd.h"
#include "stdio.h"
#include "bsp_led.h"
#include "bsp_tim2.h"
#include "bsp_tim3.h"
#include "bsp_pwm.h"
#include "bsp_usart.h"

u32 i;
char display[20];
volatile u8 num1=1;
volatile u8 num2=2;
volatile u8 num3=3;
volatile u32 num4=4;
volatile u8 num5=5;

u8 * address=0;
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Init nvic priority group.16 preemption priorities.
	SysTick_Init();//1us precision
	ILI9806G_Init();//Init lcd screen
	LED_Init();
	//TIM7_Configuration();
	//EXTI_Config();
	//TIM3_Configuration();
	TIM3_PWM_Configuration();
	Serial_Init(115200);
	while(1)
	{

	}
} 

/*********************************************END OF FILE**********************/

