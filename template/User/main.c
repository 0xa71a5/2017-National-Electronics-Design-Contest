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
#include "math.h"



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
float stepsPerCm=16.2206;//16 steps goes 1 cm
float mmPerStep=0.6165;//0.6165mm per step

#define MinPeriod 1000
#define MaxPeriod 3000

void MoveP2P(double X0,double Y0,double X1,double Y1)
{
	double L1=0;
	double L2=0;
	double L1_=0;
	double L2_=0;
	double Xa=-14,Ya=115;
	double Xb=95,Yb=115;
	
	double deltL1=0;
	double deltL2=0;
	
	L1=sqrt((X0-Xa)*(X0-Xa)+(Y0-Ya)*(Y0-Ya));
	L2=sqrt((X0-Xb)*(X0-Xb)+(Y0-Yb)*(Y0-Yb));
	
	L1_=sqrt((X1-Xa)*(X1-Xa)+(Y1-Ya)*(Y1-Ya));
	L2_=sqrt((X1-Xb)*(X1-Xb)+(Y1-Yb)*(Y1-Yb));
	
	deltL1=L1_-L1;
	deltL2=L2_-L2;
	
	s32 steps1=deltL1*stepsPerCm;
	s32 steps2=deltL2*stepsPerCm;
	u32 Stepper1_Speed=1200;
	u32 Stepper2_Speed=1200;
	if(deltL1!=0)
	{
		double ratio=deltL2/deltL1;
		double Q1=0;//ratio*MinPeriod;
		double Q2=0;//ratio*MaxPeriod;
		double Max_LeftQ1,Min_RightQ2;
		if(ratio<0)
			ratio=-ratio;
		Q1=ratio*MinPeriod;
		Q2=ratio*MaxPeriod;
		
		if(Q1<MinPeriod)//找到Q1和MinPeriod中大的那一个
			Max_LeftQ1=MinPeriod;
		else 
			Max_LeftQ1=Q1;
		
		if(Q2<MaxPeriod)//找到Q2和MaxPeriod中小的那一个
			Min_RightQ2=Q2;
		else
			Min_RightQ2=MaxPeriod;
		
		printf("Max_LeftQ1=%f Min_RightQ2=%f\n",Max_LeftQ1,Min_RightQ2);
		
		
		if(Max_LeftQ1<=Min_RightQ2)//最小值中的最大值 小于等于  最大值中的最小值 ，理想状态
		{
			Stepper1_Speed=Max_LeftQ1;
			Stepper2_Speed=Max_LeftQ1/ratio;
		}         
		else//最小值中的最大值 小于等于  最大值中的最小值 ，速度恐慌状态， 大小判断后各取极值
		{
			if(abs((s32)deltL1)<abs((s32)deltL2))//deltL1小于deltL2 所以L1 speed period取最大值  L2 speed period取最小值
			{
				Stepper1_Speed=MaxPeriod;
				Stepper2_Speed=MinPeriod;
			}
			else//deltL1大于deltL2 所以L1 speed period取最小值  L2 speed period取最大值
			{
				Stepper1_Speed=MinPeriod;
				Stepper2_Speed=MaxPeriod;
			}
		}
	}
	else//deltL1=0说明L1长度不变，那么不设置stepper1的速度
	{
		if(deltL2!=0)
		{
			Stepper1_Speed=0;
			Stepper2_Speed=MinPeriod;
		}
		else
		{
			Stepper1_Speed=0;
			Stepper2_Speed=0;
		}
	}
	
	Stepper1_SetSpeed(steps1,Stepper1_Speed);
	Stepper2_SetSpeed(steps2,Stepper2_Speed);
	
	printf("\n(X0,Y0)=(%d,%d) (L1,L2)=(%f,%f) \n",(int)X0, (int)Y0,L1,L2);
	printf("(X1,Y1)=(%d,%d) (L1_,L2_)=(%f,%f) \n",(int)X1, (int)Y1,L1_,L2_);
	printf("deltL1=%f steps1=%d deltL2=%f steps2=%d\n",deltL1,steps1,deltL2,steps2);
	printf("Period 1:%d  Period 2:%d\n",Stepper1_Speed,Stepper2_Speed);
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
	float steps=40*stepsPerCm;
	printf("Steps=%d\n",(s32)steps);
	
	
	//Stepper1_SetSpeed((s32)steps,1400);
	Stepper2_SetSpeed(300,1000);
	//MoveP2P(10,50,20,50);
	MoveP2P(65,50,10,30);
	printf("Running...");
	while(1)
	{
		LED1_OFF();
		delay(50);
	}
}

/*

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


*/


/*********************************************END OF FILE**********************/

