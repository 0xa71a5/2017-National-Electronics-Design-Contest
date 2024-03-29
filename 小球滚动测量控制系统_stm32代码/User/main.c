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
#include "tool.h"
#include "stdlib.h"

#define FLAG_ERROR 0xfe
#define FLAG_POINT 'P'
#define FLAG_START 'S'
#define FLAG_END   'E'

u8 key;
u32 i;
char display[20];
char buffer[20];
extern char command[20];
char Flag = '0';


extern int32_t Stepper1_Steps;
extern int32_t Stepper2_Steps;
extern uint8_t ledState;

double buff_x[300];
double buff_y[300];//??300?buffer???????

double stepsPerCm=256.4;//64 steps goes 1 cm
double mmPerStep=0.6199628;//0.6165mm per step

#define MinPeriod 300
#define MaxPeriod 2500
#define moveTimes 25

//min 1000
//max 3000
double currentX;
double currentY;
double PointX;
double PointY;
void menu_Track()
{
	int i = 0;
	LCD_SetColors(BLUE,BLACK);
	ILI9806G_DrawRectangle (0,454,320,400,0);
	for(i = 0;i<400;i = i+4)
	{
		ILI9806G_DrawLine(0,454+i,320,454+i);
	}
	for(i = 0;i<320;i = i + 4)
	{
		ILI9806G_DrawLine(0+i,454,0+i,854);
	}
}
void MoveP2P(double X0,double Y0,double X1,double Y1)
{
	//menu_Track();
	//LCD_SetColors(RED,BLACK);
	double L1=0;
	double L2=0;
	double L1_=0;
	double L2_=0;
	double Xa=-15.0,Ya=115;
	double Xb=95,Yb=115;
	char disparr[20];
	char disparr2[20];
	double deltL1=0;
	double deltL2=0;
	
	currentX=X0;
	currentY=Y0;
	
	
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
		
		//printf("Max_LeftQ1=%f Min_RightQ2=%f\n",Max_LeftQ1,Min_RightQ2);
		
		
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
			//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			Stepper1_Speed=3000;
			Stepper2_Speed=MinPeriod;
		}
		else
		{
			//printf("**********************************\n");
			Stepper1_Speed=3000;
			Stepper2_Speed=3000;//问题在这儿
		}
	}
	
	Stepper1_SetSpeed(steps1,Stepper1_Speed);
	Stepper2_SetSpeed(steps2,Stepper2_Speed);
	
	Stepper_Wait();
	
	//ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
	
	/*
	ILI9806G_DispStringLine_EN(LINE(3), "X :");
	sprintf(disparr, "Count : %d ",(int)currentX);
	ILI9806G_DispStringLine_EN(LINE(4),disparr);
	sprintf(disparr2, "Count : %d ",(int)currentY);
	ILI9806G_DispStringLine_EN(LINE(5), "Y :");
	ILI9806G_DispStringLine_EN(LINE(6),disparr2);
	ILI9806G_DrawLine(currentX*4,854-currentY*4,X1*4,854-Y1*4);
	*/
	currentX=X1;
	currentY=Y1;
}

void drawLine(double startX,double startY,double endX,double endY)
{
	int i;
	MoveP2P(currentX,currentY,startX,startY);//先等到移动到线段起点
	
	double deltX=(endX-startX)/moveTimes;
	double deltY=(endY-startY)/moveTimes;
	//printf("Move from (%f,%f) to (%f,%f) \n",startX,startY,endX,endY);
	for(i=0;i<moveTimes;i++)
	{
		//printf("(%f,%f) -> (%f,%f)\n",currentX,currentY,currentX+deltX,currentY+deltY);
		MoveP2P(currentX,currentY,currentX+deltX,currentY+deltY);
		Stepper_Wait();
		//delay(5);
	}
	MoveP2P(currentX,currentY,endX,endY);
	//printf("Current Position=(%f,%f)\n",currentX,currentY);
}

//121个点试试
//const double circle1_X[]={25.0, 24.9961923789, 24.9847706755, 24.9657383689, 24.9391012565, 24.9048674523, 24.8630473842, 24.813653791, 24.7567017185, 24.6922085149, 24.6201938253, 24.5406795862, 24.4536900183, 24.3592516196, 24.2573931569, 24.1481456572, 24.0315423985, 23.9076188991, 23.7764129074, 23.63796439, 23.4923155196, 23.3395106624, 23.1795963642, 23.0126213363, 22.8386364411, 22.6576946759, 22.4698511575, 22.2751631047, 22.0736898215, 21.8654926785, 21.6506350946, 21.4291825176, 21.2012024039, 20.9667641986, 20.7259393139, 20.4788011072, 20.2254248594, 19.9658877512, 19.7002688402, 19.4286490364, 19.151111078, 18.8677395056, 18.5786206369, 18.2838425405, 17.9834950085, 17.6776695297, 17.3664592615, 17.0499590016, 16.728265159, 16.4014757248, 16.0696902422, 15.7330097762, 15.3915368831, 15.0453755788, 14.6946313073, 14.3394109088, 13.9798225868, 13.6159758754, 13.2479816058, 12.8759518728, 12.5, 12.1202405062, 11.7367890696, 11.3497624935, 10.9592786697, 10.5654565435, 10.1684160769, 9.76827821223, 9.3651648354, 8.95919873863, 8.55050358314, 8.13920386143, 7.72542485937, 7.30929261807, 6.89093389542, 6.47047612756, 6.04804738999, 5.6237763586, 5.19779227044, 4.77022488441, 4.34120444167, 3.91086162601, 3.479327524, 3.04673358513, 2.61321158169, 2.17889356869, 1.7439118436, 1.30839890607, 0.872487417563, 0.436310160932, 0, -0.436310160932, -0.872487417563, -1.30839890607, -1.7439118436, -2.17889356869, -2.61321158169, -3.04673358513, -3.479327524, -3.91086162601, -4.34120444167, -4.77022488441, -5.19779227044, -5.6237763586, -6.04804738999, -6.47047612756, -6.89093389542, -7.30929261807, -7.72542485937, -8.13920386143, -8.55050358314, -8.95919873863, -9.3651648354, -9.76827821223, -10.1684160769, -10.5654565435, -10.9592786697, -11.3497624935, -11.7367890696, -12.1202405062, -12.5, -12.8759518728, -13.2479816058, -13.6159758754, -13.9798225868, -14.3394109088, -14.6946313073, -15.0453755788, -15.3915368831, -15.7330097762, -16.0696902422, -16.4014757248, -16.728265159, -17.0499590016, -17.3664592615, -17.6776695297, -17.9834950085, -18.2838425405, -18.5786206369, -18.8677395056, -19.151111078, -19.4286490364, -19.7002688402, -19.9658877512, -20.2254248594, -20.4788011072, -20.7259393139, -20.9667641986, -21.2012024039, -21.4291825176, -21.6506350946, -21.8654926785, -22.0736898215, -22.2751631047, -22.4698511575, -22.6576946759, -22.8386364411, -23.0126213363, -23.1795963642, -23.3395106624, -23.4923155196, -23.63796439, -23.7764129074, -23.9076188991, -24.0315423985, -24.1481456572, -24.2573931569, -24.3592516196, -24.4536900183, -24.5406795862, -24.6201938253, -24.6922085149, -24.7567017185, -24.813653791, -24.8630473842, -24.9048674523, -24.9391012565, -24.9657383689, -24.9847706755, -24.9961923789, -25.0, -24.9961923789, -24.9847706755, -24.9657383689, -24.9391012565, -24.9048674523, -24.8630473842, -24.813653791, -24.7567017185, -24.6922085149, -24.6201938253, -24.5406795862, -24.4536900183, -24.3592516196, -24.2573931569, -24.1481456572, -24.0315423985, -23.9076188991, -23.7764129074, -23.63796439, -23.4923155196, -23.3395106624, -23.1795963642, -23.0126213363, -22.8386364411, -22.6576946759, -22.4698511575, -22.2751631047, -22.0736898215, -21.8654926785, -21.6506350946, -21.4291825176, -21.2012024039, -20.9667641986, -20.7259393139, -20.4788011072, -20.2254248594, -19.9658877512, -19.7002688402, -19.4286490364, -19.151111078, -18.8677395056, -18.5786206369, -18.2838425405, -17.9834950085, -17.6776695297, -17.3664592615, -17.0499590016, -16.728265159, -16.4014757248, -16.0696902422, -15.7330097762, -15.3915368831, -15.0453755788, -14.6946313073, -14.3394109088, -13.9798225868, -13.6159758754, -13.2479816058, -12.8759518728, -12.5, -12.1202405062, -11.7367890696, -11.3497624935, -10.9592786697, -10.5654565435, -10.1684160769, -9.76827821223, -9.3651648354, -8.95919873863, -8.55050358314, -8.13920386143, -7.72542485937, -7.30929261807, -6.89093389542, -6.47047612756, -6.04804738999, -5.6237763586, -5.19779227044, -4.77022488441, -4.34120444167, -3.91086162601, -3.479327524, -3.04673358513, -2.61321158169, -2.17889356869, -1.7439118436, -1.30839890607, -0.872487417563, -0.436310160932,0, 0.436310160932, 0.872487417563, 1.30839890607, 1.7439118436, 2.17889356869, 2.61321158169, 3.04673358513, 3.479327524, 3.91086162601, 4.34120444167, 4.77022488441, 5.19779227044, 5.6237763586, 6.04804738999, 6.47047612756, 6.89093389542, 7.30929261807, 7.72542485937, 8.13920386143, 8.55050358314, 8.95919873863, 9.3651648354, 9.76827821223, 10.1684160769, 10.5654565435, 10.9592786697, 11.3497624935, 11.7367890696, 12.1202405062, 12.5, 12.8759518728, 13.2479816058, 13.6159758754, 13.9798225868, 14.3394109088, 14.6946313073, 15.0453755788, 15.3915368831, 15.7330097762, 16.0696902422, 16.4014757248, 16.728265159, 17.0499590016, 17.3664592615, 17.6776695297, 17.9834950085, 18.2838425405, 18.5786206369, 18.8677395056, 19.151111078, 19.4286490364, 19.7002688402, 19.9658877512, 20.2254248594, 20.4788011072, 20.7259393139, 20.9667641986, 21.2012024039, 21.4291825176, 21.6506350946, 21.8654926785, 22.0736898215, 22.2751631047, 22.4698511575, 22.6576946759, 22.8386364411, 23.0126213363, 23.1795963642, 23.3395106624, 23.4923155196, 23.63796439, 23.7764129074, 23.9076188991, 24.0315423985, 24.1481456572, 24.2573931569, 24.3592516196, 24.4536900183, 24.5406795862, 24.6201938253, 24.6922085149, 24.7567017185, 24.813653791, 24.8630473842, 24.9048674523, 24.9391012565, 24.9657383689, 24.9847706755, 24.9961923789, 25.0};
//const double circle1_Y[]={0.0, 0.436310160932, 0.872487417563, 1.30839890607, 1.7439118436, 2.17889356869, 2.61321158169, 3.04673358513, 3.479327524, 3.91086162601, 4.34120444167, 4.77022488441, 5.19779227044, 5.6237763586, 6.04804738999, 6.47047612756, 6.89093389542, 7.30929261807, 7.72542485937, 8.13920386143, 8.55050358314, 8.95919873863, 9.3651648354, 9.76827821223, 10.1684160769, 10.5654565435, 10.9592786697, 11.3497624935, 11.7367890696, 12.1202405062, 12.5, 12.8759518728, 13.2479816058, 13.6159758754, 13.9798225868, 14.3394109088, 14.6946313073, 15.0453755788, 15.3915368831, 15.7330097762, 16.0696902422, 16.4014757248, 16.728265159, 17.0499590016, 17.3664592615, 17.6776695297, 17.9834950085, 18.2838425405, 18.5786206369, 18.8677395056, 19.151111078, 19.4286490364, 19.7002688402, 19.9658877512, 20.2254248594, 20.4788011072, 20.7259393139, 20.9667641986, 21.2012024039, 21.4291825176, 21.6506350946, 21.8654926785, 22.0736898215, 22.2751631047, 22.4698511575, 22.6576946759, 22.8386364411, 23.0126213363, 23.1795963642, 23.3395106624, 23.4923155196, 23.63796439, 23.7764129074, 23.9076188991, 24.0315423985, 24.1481456572, 24.2573931569, 24.3592516196, 24.4536900183, 24.5406795862, 24.6201938253, 24.6922085149, 24.7567017185, 24.813653791, 24.8630473842, 24.9048674523, 24.9391012565, 24.9657383689, 24.9847706755, 24.9961923789, 25.0, 24.9961923789, 24.9847706755, 24.9657383689, 24.9391012565, 24.9048674523, 24.8630473842, 24.813653791, 24.7567017185, 24.6922085149, 24.6201938253, 24.5406795862, 24.4536900183, 24.3592516196, 24.2573931569, 24.1481456572, 24.0315423985, 23.9076188991, 23.7764129074, 23.63796439, 23.4923155196, 23.3395106624, 23.1795963642, 23.0126213363, 22.8386364411, 22.6576946759, 22.4698511575, 22.2751631047, 22.0736898215, 21.8654926785, 21.6506350946, 21.4291825176, 21.2012024039, 20.9667641986, 20.7259393139, 20.4788011072, 20.2254248594, 19.9658877512, 19.7002688402, 19.4286490364, 19.151111078, 18.8677395056, 18.5786206369, 18.2838425405, 17.9834950085, 17.6776695297, 17.3664592615, 17.0499590016, 16.728265159, 16.4014757248, 16.0696902422, 15.7330097762, 15.3915368831, 15.0453755788, 14.6946313073, 14.3394109088, 13.9798225868, 13.6159758754, 13.2479816058, 12.8759518728, 12.5, 12.1202405062, 11.7367890696, 11.3497624935, 10.9592786697, 10.5654565435, 10.1684160769, 9.76827821223, 9.3651648354, 8.95919873863, 8.55050358314, 8.13920386143, 7.72542485937, 7.30929261807, 6.89093389542, 6.47047612756, 6.04804738999, 5.6237763586, 5.19779227044, 4.77022488441, 4.34120444167, 3.91086162601, 3.479327524, 3.04673358513, 2.61321158169, 2.17889356869, 1.7439118436, 1.30839890607, 0.872487417563, 0.436310160932, 0, -0.436310160932, -0.872487417563, -1.30839890607, -1.7439118436, -2.17889356869, -2.61321158169, -3.04673358513, -3.479327524, -3.91086162601, -4.34120444167, -4.77022488441, -5.19779227044, -5.6237763586, -6.04804738999, -6.47047612756, -6.89093389542, -7.30929261807, -7.72542485937, -8.13920386143, -8.55050358314, -8.95919873863, -9.3651648354, -9.76827821223, -10.1684160769, -10.5654565435, -10.9592786697, -11.3497624935, -11.7367890696, -12.1202405062, -12.5, -12.8759518728, -13.2479816058, -13.6159758754, -13.9798225868, -14.3394109088, -14.6946313073, -15.0453755788, -15.3915368831, -15.7330097762, -16.0696902422, -16.4014757248, -16.728265159, -17.0499590016, -17.3664592615, -17.6776695297, -17.9834950085, -18.2838425405, -18.5786206369, -18.8677395056, -19.151111078, -19.4286490364, -19.7002688402, -19.9658877512, -20.2254248594, -20.4788011072, -20.7259393139, -20.9667641986, -21.2012024039, -21.4291825176, -21.6506350946, -21.8654926785, -22.0736898215, -22.2751631047, -22.4698511575, -22.6576946759, -22.8386364411, -23.0126213363, -23.1795963642, -23.3395106624, -23.4923155196, -23.63796439, -23.7764129074, -23.9076188991, -24.0315423985, -24.1481456572, -24.2573931569, -24.3592516196, -24.4536900183, -24.5406795862, -24.6201938253, -24.6922085149, -24.7567017185, -24.813653791, -24.8630473842, -24.9048674523, -24.9391012565, -24.9657383689, -24.9847706755, -24.9961923789, -25.0, -24.9961923789, -24.9847706755, -24.9657383689, -24.9391012565, -24.9048674523, -24.8630473842, -24.813653791, -24.7567017185, -24.6922085149, -24.6201938253, -24.5406795862, -24.4536900183, -24.3592516196, -24.2573931569, -24.1481456572, -24.0315423985, -23.9076188991, -23.7764129074, -23.63796439, -23.4923155196, -23.3395106624, -23.1795963642, -23.0126213363, -22.8386364411, -22.6576946759, -22.4698511575, -22.2751631047, -22.0736898215, -21.8654926785, -21.6506350946, -21.4291825176, -21.2012024039, -20.9667641986, -20.7259393139, -20.4788011072, -20.2254248594, -19.9658877512, -19.7002688402, -19.4286490364, -19.151111078, -18.8677395056, -18.5786206369, -18.2838425405, -17.9834950085, -17.6776695297, -17.3664592615, -17.0499590016, -16.728265159, -16.4014757248, -16.0696902422, -15.7330097762, -15.3915368831, -15.0453755788, -14.6946313073, -14.3394109088, -13.9798225868, -13.6159758754, -13.2479816058, -12.8759518728, -12.5, -12.1202405062, -11.7367890696, -11.3497624935, -10.9592786697, -10.5654565435, -10.1684160769, -9.76827821223, -9.3651648354, -8.95919873863, -8.55050358314, -8.13920386143, -7.72542485937, -7.30929261807, -6.89093389542, -6.47047612756, -6.04804738999, -5.6237763586, -5.19779227044, -4.77022488441, -4.34120444167, -3.91086162601, -3.479327524, -3.04673358513, -2.61321158169, -2.17889356869, -1.7439118436, -1.30839890607, -0.872487417563, -0.436310160932, 0};
const double circle1_X[]={25.0, 24.9657383689, 24.8630473842, 24.6922085149, 24.4536900183, 24.1481456572, 23.7764129074, 23.3395106624, 22.8386364411, 22.2751631047, 21.6506350946, 20.9667641986, 20.2254248594, 19.4286490364, 18.5786206369, 17.6776695297, 16.728265159, 15.7330097762, 14.6946313073, 13.6159758754, 12.5, 11.3497624935, 10.1684160769, 8.95919873863, 7.72542485937, 6.47047612756, 5.19779227044, 3.91086162601, 2.61321158169, 1.30839890607, 0, -1.30839890607, -2.61321158169, -3.91086162601, -5.19779227044, -6.47047612756, -7.72542485937, -8.95919873863, -10.1684160769, -11.3497624935, -12.5, -13.6159758754, -14.6946313073, -15.7330097762, -16.728265159, -17.6776695297, -18.5786206369, -19.4286490364, -20.2254248594, -20.9667641986, -21.6506350946, -22.2751631047, -22.8386364411, -23.3395106624, -23.7764129074, -24.1481456572, -24.4536900183, -24.6922085149, -24.8630473842, -24.9657383689, -25.0, -24.9657383689, -24.8630473842, -24.6922085149, -24.4536900183, -24.1481456572, -23.7764129074, -23.3395106624, -22.8386364411, -22.2751631047, -21.6506350946, -20.9667641986, -20.2254248594, -19.4286490364, -18.5786206369, -17.6776695297, -16.728265159, -15.7330097762, -14.6946313073, -13.6159758754, -12.5, -11.3497624935, -10.1684160769, -8.95919873863, -7.72542485937, -6.47047612756, -5.19779227044, -3.91086162601, -2.61321158169, -1.30839890607, 0, 1.30839890607, 2.61321158169, 3.91086162601, 5.19779227044, 6.47047612756, 7.72542485937, 8.95919873863, 10.1684160769, 11.3497624935, 12.5, 13.6159758754, 14.6946313073, 15.7330097762, 16.728265159, 17.6776695297, 18.5786206369, 19.4286490364, 20.2254248594, 20.9667641986, 21.6506350946, 22.2751631047, 22.8386364411, 23.3395106624, 23.7764129074, 24.1481456572, 24.4536900183, 24.6922085149, 24.8630473842, 24.9657383689, 25.0};
const double circle1_Y[]={0.0, 1.30839890607, 2.61321158169, 3.91086162601, 5.19779227044, 6.47047612756, 7.72542485937, 8.95919873863, 10.1684160769, 11.3497624935, 12.5, 13.6159758754, 14.6946313073, 15.7330097762, 16.728265159, 17.6776695297, 18.5786206369, 19.4286490364, 20.2254248594, 20.9667641986, 21.6506350946, 22.2751631047, 22.8386364411, 23.3395106624, 23.7764129074, 24.1481456572, 24.4536900183, 24.6922085149, 24.8630473842, 24.9657383689, 25.0, 24.9657383689, 24.8630473842, 24.6922085149, 24.4536900183, 24.1481456572, 23.7764129074, 23.3395106624, 22.8386364411, 22.2751631047, 21.6506350946, 20.9667641986, 20.2254248594, 19.4286490364, 18.5786206369, 17.6776695297, 16.728265159, 15.7330097762, 14.6946313073, 13.6159758754, 12.5, 11.3497624935, 10.1684160769, 8.95919873863, 7.72542485937, 6.47047612756, 5.19779227044, 3.91086162601, 2.61321158169, 1.30839890607, 0, -1.30839890607, -2.61321158169, -3.91086162601, -5.19779227044, -6.47047612756, -7.72542485937, -8.95919873863, -10.1684160769, -11.3497624935, -12.5, -13.6159758754, -14.6946313073, -15.7330097762, -16.728265159, -17.6776695297, -18.5786206369, -19.4286490364, -20.2254248594, -20.9667641986, -21.6506350946, -22.2751631047, -22.8386364411, -23.3395106624, -23.7764129074, -24.1481456572, -24.4536900183, -24.6922085149, -24.8630473842, -24.9657383689, -25.0, -24.9657383689, -24.8630473842, -24.6922085149, -24.4536900183, -24.1481456572, -23.7764129074, -23.3395106624, -22.8386364411, -22.2751631047, -21.6506350946, -20.9667641986, -20.2254248594, -19.4286490364, -18.5786206369, -17.6776695297, -16.728265159, -15.7330097762, -14.6946313073, -13.6159758754, -12.5, -11.3497624935, -10.1684160769, -8.95919873863, -7.72542485937, -6.47047612756, -5.19779227044, -3.91086162601, -2.61321158169, -1.30839890607, 0};

void drawCircle(double x,double y)
{
	int i;
	double deltX;
	double deltY;
	double theta;
	double radian;
	int _times;
	#define PI 3.141592657
	MoveP2P(currentX,currentY,x+25,y);//移动到X轴方向 距离圆心一个R的地方
	//printf("Current Position=(%f,%f)\n",currentX,currentY);
//	printf("Size of cir X:%d and double:%d \n",sizeof(circle1_X),sizeof(double));
	//printf("Ready to go\n");
	delay(2000);
	
	_times=sizeof(circle1_X)/sizeof(double);

	/*
	for(i=0;i<_times;i++)
	{
		deltX=circle1_X[i];
		deltY=circle1_Y[i];
		//printf("%d (%f,%f) -> (%f,%f)\n",(u32)theta,currentX,currentY,x+deltX,y+deltY);
		MoveP2P(currentX,currentY,x+deltX,y+deltY);
		delay(30);
		//printf("(%f,%f)\n",currentX,currentY);
		//delay(50);
	}
	*/
	
	
	for(theta=0;theta<362;theta++)
	{
		double radian=theta*0.017453292519943295;
		//printf("%f\n",theta);
		deltX=25*cos(radian);
		deltY=25*sin(radian);
		
		//printf("%d (%f,%f) -> (%f,%f)\n",(u32)theta,currentX,currentY,x+deltX,y+deltY);
		MoveP2P(currentX,currentY,x+deltX,y+deltY);
		//printf("(%f,%f)\n",currentX,currentY);
		//delay(30);
	}
	
	
	//printf("Draw circle complete!\n");

}

void delay_nothing(u32 us)
{
	volatile u32 i,j;
	for(i=0;i<us;i++)
	{
		for(j=0;j<20;j++)//约为1us
			{}
	}
}

void menu_All()
{
	ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
	ILI9806G_DispStringLine_EN(LINE(0), "Suspension control system  45");
	ILI9806G_DispStringLine_EN(LINE(2), "Function: ");
	ILI9806G_DispStringLine_EN(LINE(3), "  1: Reset  2: FreeStyle");
	ILI9806G_DispStringLine_EN(LINE(4), "  3: P2P    4: Circle");
	ILI9806G_DispStringLine_EN(LINE(5), "  5: Track  6: Rabit");
	ILI9806G_DispStringLine_EN(LINE(6), "  7: Back");
	ILI9806G_DispStringLine_EN(LINE(7), "  15: OK    16: Quit");
}
void menu_FreeStyle()
{
	ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
	ILI9806G_DispStringLine_EN(LINE(0), " Follow the tune");
	ILI9806G_DispStringLine_EN(LINE(10), " 2:UP   5:DOWN");
	ILI9806G_DispStringLine_EN(LINE(11), " 5:LEFT 7:RIGHT");
	ILI9806G_DispStringLine_EN(LINE(11), " 13:Release 4:Circle");
}

char Command_Handler(char* command,u8 size_,double *x_,double *y_)
{
	//printf("Catch a size[%d] command:%s\n",size_, command);
	char header=0x00;
	double num=0;
	u32 point_pos=0;
	u8  point_valid=0;
	u8 current_item=0;
	if(command[0]!='<'&&command[size_-1]!='>'&&size_<7)return FLAG_ERROR;
	header=command[1];//???????
	for(int i=3;i<size_;i++)
	{
		if(command[i]=='>'||command[i]==',')
		{
			num=num/pow(10,point_pos);
			if(current_item==0)
				*x_=num;
			else if(current_item==1)
				*y_=num;
			current_item++;
			num=0;//??
			point_valid=0;//?????????
			continue;
		}
		if(command[i]>='0'&&command[i]<='9')
		{
			num*=10;	
			num+=command[i]-'0';
			if(point_valid!=0)
				point_pos++;
		}
		else if(command[i]=='.')
		{
			point_pos=0;//????????0
			point_valid=1;//????????
		}
	}
	//

    //printf("M (x,y)=(%f,%f)\n",*x_,*y_);
	return header;
}

void MoveP2P_FromBuffer(double buff_x[],double buff_y[],u32 point_index)
{
	//printf("MoveP2P from buffer...\n");

	MoveP2P(currentX,currentY,buff_x[0],buff_y[0]);
	delay(1000);
	for(int i=1;i<point_index;i++)
	{
		//printf("(%f,%f) -> (%f,%f)\n",currentX,currentY,buff_x[i],buff_y[i]);
		MoveP2P(currentX,currentY,buff_x[i],buff_y[i]);
	}
	//printf("Complete MoveP2Ps!\n");
	
}
void Serial2char()
{//225 pair
	
	u32 point_index=0;
	int end=0;
	char buff_char[20] = {'0'};//???????

	PointX =0.0; PointY = 0.0;
	//printf("Enter Serial2char mode\n");

	ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
	ILI9806G_DispStringLine_EN(LINE(0), "Input Serial,16 can quit the loop");
	
	key = Read_key();
	
	#define SERIAL_DELAY 50
	while(key != 16)//????????
	{
		while(Serial2_available()!= 0)
		{
			char data=Serial2_read();
			double x_=-1,y_=-1;
			char flag_=0x00;
			//delay_us(SERIAL_DELAY);
			if(data=='<')//???????
			{
				u16 char_index=0;
				buff_char[char_index++]=data;
				while(data!='>')
				{
					if(Serial2_available())
					{
						data=Serial2_read();
						buff_char[char_index++]=data;
					}
					delay_us(SERIAL_DELAY);
				}
				buff_char[char_index]='\0';
				flag_=Command_Handler(buff_char,char_index,&x_,&y_);//??

				if(flag_==FLAG_START)
				{
					//printf("Flag=Start\n");
					point_index=0;
				}
				else if(flag_==FLAG_END)
				{
					//printf("Flag=End index=%d\n",point_index);
					MoveP2P_FromBuffer(buff_x,buff_y,point_index);
					end=1;//?????
					break;
				}
				else if(flag_==FLAG_POINT)
				{
					//printf("Flag=Point%d\n",point_index);
					buff_x[point_index]=x_;
					buff_y[point_index]=y_;
					//printf("___ x_=%f y_=%f  ;buff %d -> x_=%f y_=%f \n",x_,y_,point_index,buff_x[point_index],buff_y[point_index]);
					point_index++;
				}
				//if(flag==NONUSE)
			}
		}
		key = Read_key();
		if(end==1)break;//??????
	}
	//printf("Exit from Serial2char mode");
	//ILI9806G_DispStringLine_EN(LINE(20), buff);
	//GetApart(buff,index);
	delay(1000);
}

void drawRabit()
{
		int pointx[47] ={47,45,45,47,48,52,53,56,56,57,57,56,49,25,22,20,19,19,20,22,22,23,23,25,26,27,33,36,37,37,36,36,35,32,36,40,48,51,57,57,58,59,62,64,64,62,56};
 
		int pointy[47] ={27,29,34,36,37,37,36,33,31,30,26,25,23,23,24,26,28,23,36,38,43,46,48,52,54,56,56,54,52,48,47,45,44,40,43,45,45,44,40,39,37,33,33,30,28,25,25};
	
		ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
		
		ILI9806G_DispStringLine_EN(LINE(2), "Draw a Rabit");
		
		int i = 0;
		for(i = 0;i<47;i++)
		{
			drawLine(currentX,currentY,pointx[i],pointy[i]);
		}
		delay(500);
	
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
	init_TM1638();//init keyboard
	Stepper_Init();
  Serial_Init(115200);	
	Serial2_Init(115200);
	printf("Running...\n");
	menu_All();
	
	while(Read_key()!= 16)
	{
		key=Read_key();
		//printf("%d\n",key);
		if(key == 1)
		{
			ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
			currentX=0;currentY=0;
			ILI9806G_DispStringLine_EN(LINE(12), "Please move to (0,0) by hand");
			delay(5000);
			menu_All();
			key = 0;
		}
		else if(key == 2)
		{
			menu_FreeStyle();
			while(key != 16)
			{
				key=Read_key();
				if(key==0)
					delay_nothing(5000);
				else
				{
					#define STEP__ 1
					switch(key)
					{
						case 2://左边电机向上
							Stepper1_SetSpeed(-STEP__,600);
							Stepper_Wait();
							//drawLine(currentX,currentY,currentX,currentY+1);
							
							break;
						
						case 10://左边电机向下
							Stepper1_SetSpeed(STEP__,600);
							Stepper_Wait();
							//drawLine(currentX,currentY,currentX,currentY-1);
							break;
						
						case 5://右边电机向左
							Stepper2_SetSpeed(-STEP__,600);
							Stepper_Wait();
							//drawLine(currentX,currentY,currentX-1,currentY);
							break;
						
						case 7://右边电机向右
							Stepper2_SetSpeed(STEP__,600);
							Stepper_Wait();
							//drawLine(currentX,currentY,currentX+1,currentY);
							break;
						case 13://停止锁定
							Stepper1_En((!PDin(7)));
							Stepper2_En((PDin(7)));
							delay(500);
							break;

						case 4://画圆
							drawCircle(40,50);
							break;
						
						case 15:
							delay(500);
							if(Read_key()==15)
								MoveP2P(65,50,10,10);
							break;

						case 16:
							delay(500);
							if(Read_key()==16)
								MoveP2P(10,10,65,50);
							break;
						
						default:
							break;
					}
			
				}
			}
			menu_All();
			key = 0;
		}
		else if(key == 3)
		{
			delay(300);
			Key2char();
			Char2int(command);
			drawLine(currentX,currentY,PointX,PointY);
			delay(500);
			menu_All();
			key = 0;
		}
		else if(key == 4)
		{
			delay(300);
			Key2char();
			drawCircle(PointX,PointY);
			delay(500);
			menu_All();
			key = 0;
		}
		else if(key == 5)
		{
			delay(300);
			Serial2char();
			menu_All();
			key = 0;
		}
		else if(key == 6)
		{
			delay(300);
			drawRabit();
			menu_All();
			key = 0;
		}
		else if(key == 7)
		{
			delay(300);
			drawLine(currentX,currentY,0,0);
			menu_All();
			key = 0;
		}
		else if(key == 8)
		{
			delay(300);
			//Stepper1_SetSpeed(1000,200);
			Stepper2_SetSpeed(1000,200);
			menu_All();
			key = 0;
		}
	}	
}
/*********************************************END OF FILE**********************/

