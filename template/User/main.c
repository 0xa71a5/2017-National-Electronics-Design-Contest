#include "stm32f4xx.h"
#include "bsp_SysTick.h"
#include "bsp_ili9806g_lcd.h"
#include "stdio.h"
#include "bsp_led.h"
#include "bsp_tim2.h"
#include "bsp_tim3.h"
#include "bsp_pwm.h"
#include "serial.h"
#include "tm1638.h"
#include "stepper.h"

u32 i;
char display[20];
char buffer[20];

extern int32_t Stepper1_Steps;
extern int32_t Stepper2_Steps;
extern uint8_t ledState;

void Command_Handler(char* command)
{
	printf("Command=[%s]\n",command);
}


int main(void)
{
	char data;
	#define BUFFER_SIZE 20
	char buffer[BUFFER_SIZE];
	u16 b_index=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Init nvic priority group.16 preemption priorities.
	SysTick_Init();//1us precision
	ILI9806G_Init();//Init lcd screen
	LED_Init();
	
	Stepper_Init();
	Serial_Init(115200);
	while(1)
	{
		while(Serial_Available())
		{
			data=Serial_Read();
			delay(2);
			buffer[b_index++]=data;
		}
		if(b_index!=0)
		{
			buffer[b_index]='\0';
			Command_Handler(buffer);
			b_index=0;
		}
	}
}




/*********************************************END OF FILE**********************/

