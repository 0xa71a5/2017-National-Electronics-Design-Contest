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
#include "adc1.h"

u8 key;

char display[20];
s32 Stepper_Steps=0;

void delay_nothing(u32 us)
{
	volatile u32 i,j;
	for(i=0;i<us;i++)
	{
		for(j=0;j<20;j++)
			{}
	}
}
extern __IO uint16_t ADC_ConvertedValue;

#define S_En(x) PBout(5)=x
#define S_Dir(x) PBout(4)=x
#define S_Pulse(x) PBout(3)=x
#define S_Step_One(x)  							S_Pulse(0);\
												delay_us(x);\
												S_Pulse(1);\
												delay_us(x);

#define S_Step_Half()					S_Pulse(Pulse_State);\
												Pulse_State=!Pulse_State;

#define Infrared_In()   PEin(2)

void Main_Stepper_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC| RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//PE2 input
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure); 

  	S_En(0);	
}


u32 Pulse_State=0;
void stepper_test()
{
	
}

void AngleInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC| RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;  //Encoder_CSN Encoder_CLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  //Data
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
}
#define Encoder_CSN(x) PBout(8)=x
#define Encoder_CLK(x) PBout(9)=x
#define Encoder_DataBit() PDin(7)
#define delay_time 2

float Encoder_ReadAngle()//120us
{
	char i;
	unsigned long ulong=0;
	
	Encoder_CLK(1);
	Encoder_CSN(1);
	delay_us(delay_time);
	Encoder_CSN(0);
	delay_us(delay_time);
	Encoder_CLK(1);
	delay_us(delay_time);

	for(i=0;i<18;i++)
	{
			Encoder_CLK(0);
			delay_us(delay_time);
			Encoder_CLK(1);
			delay_us(delay_time);
			ulong|=Encoder_DataBit(); 		
			if(i)
			{
				ulong<<=1;
				delay_us(delay_time);
			}
	}
	delay_us(delay_time);
	Encoder_CSN(1);
	u32 CurrentCodeAngle=(unsigned int)(ulong>>6);
	
	float angle=(float)CurrentCodeAngle*180/4096;
	return angle;
}

u32 millis=0;

void TIM7_IRQHandler(void)//
{
 
 if (((TIM7->SR & TIM_IT_Update) != 0 ) && ((TIM7->DIER & TIM_IT_Update) != 0))
  { 
    millis++;
    TIM7->SR = (uint16_t)~TIM_IT_Update; 
  }     
}

void TIM6_DAC_IRQHandler(void)
{

 if (((TIM6->SR & TIM_IT_Update) != 0 ) && ((TIM6->DIER & TIM_IT_Update) != 0))
  { 
  	if(Stepper_Steps>0)
	{
		S_Pulse(!PBin(3));
		Stepper_Steps--;
	}
	TIM6->SR = (uint16_t)~TIM_IT_Update; 
  }     
}

void Mission_One(void)
{
	float minAngle=1000;
	float maxAngle=0;
	float lastAngle=0;
	#define DIR_LEFT 0
	#define DIR_RIGHT 1
	u8 direction=DIR_RIGHT;
	
	lastAngle=10000;
	while(1)
	{
		float angle=Encoder_ReadAngle();
		if(angle>lastAngle+0.3)
		{
			lastAngle=angle;
			break;
		}
		printf("%f\n",angle);
		lastAngle=angle;
		
		delay(20);
	}
	printf("Loose!\n");
	
	int speed_=1000;
	int steps_to_go=400;
	float deltHalfPeriod=950;//ms
	float deltTimeEachStep=deltHalfPeriod*1000/steps_to_go/2;//durtion between steps
	
	S_En(1);
	S_Dir(1);
	while(1)
	{
		u32 startRecordTime=millis;
		while(1)
		{
			float angle=Encoder_ReadAngle();
			if(angle<lastAngle-0.1)
			{
				lastAngle=angle;
				if(millis- startRecordTime >300)
					break;
			}
			delay(1);
			lastAngle=angle;
		}
		u32 durtion=millis - startRecordTime;
		deltHalfPeriod=durtion;
		deltTimeEachStep=deltHalfPeriod*1000/steps_to_go/2;
		
		TIM6->DIER &= (uint16_t)~TIM_IT_Update;
		TIM6->ARR=(u16)deltTimeEachStep;
		TIM6->DIER |= TIM_IT_Update;
		printf("0.5period= %d ms ; dt/stp=%f \n",durtion,deltTimeEachStep);
		
		
		
		startRecordTime=millis;
		while(1)
		{
			float angle=Encoder_ReadAngle();
			if(angle>lastAngle+0.1)
			{
				lastAngle=angle;
				if(millis- startRecordTime >300)
					break;
			}
			delay(1);
			lastAngle=angle;
		}
		durtion=millis - startRecordTime;
		deltHalfPeriod=durtion;
		deltTimeEachStep=deltHalfPeriod*1000/steps_to_go/2;
	
		TIM6->DIER &= (uint16_t)~TIM_IT_Update;
		TIM6->ARR=(u16)deltTimeEachStep;
		TIM6->DIER |= TIM_IT_Update;
		printf("0.5period= %d ms ; dt/stp=%f \n",durtion,deltTimeEachStep);
	}
}


#define debugSend(x)     USART_SendData(USART1, x);\
					     while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);    


float currentMotorAngle=0;
#define SetCurrentMotorAngle(x)	currentMotorAngle=(x);


//1  1.8
//2  0.9
//4  0.45
//8  0.225
//16 0.1125
//32 0.05625
#define motorStepAngle			0.1125


#define Open_BackgroundTimer()	 	TIM7->DIER |= TIM_IT_Update;
#define Close_BackgroundTimer() 	TIM7->DIER &= ~TIM_IT_Update;
#define Open_StepperTimer()	        TIM6->DIER |= TIM_IT_Update;
#define Close_StepperTimer()	    TIM6->DIER &= ~TIM_IT_Update;

void MoveToAngle(float targetAngle,int eta)
{
	float stepsToGo=(targetAngle-currentMotorAngle)/motorStepAngle;
	//printf("tA=%.1f cA=%.1f motorStpAngle=%.4f stepsToGo=%.1f",targetAngle,currentMotorAngle,motorStepAngle,stepsToGo);
	float stepperGapTime=500;
	S_En(1);
	//Open_BackgroundTimer();
	s32 checkPoint=millis;
	s32 stepsToGo_int=stepsToGo;
	s32 stepsToGo_uint;
	if(stepsToGo>=0)
	{
		S_Dir(1);
		stepsToGo_uint=stepsToGo;
	}
	else
	{
		S_Dir(0);
		stepsToGo=-stepsToGo;
		stepsToGo_uint=stepsToGo;
	}
	if(stepsToGo!=0)
		stepperGapTime=((float)eta*950)/stepsToGo/2;
	if(stepperGapTime<200)stepperGapTime=200;
	if(stepperGapTime>5000)stepperGapTime=5000;

	//printf("+%.1f\n",stepperGapTime);
	for(int i=0;i<stepsToGo_uint;i++)
	{
		//debugSend('.');
		S_Pulse(!PBin(3));
		delay_us(stepperGapTime);
		S_Pulse(!PBin(3));
		delay_us(stepperGapTime);
	}
	s32 durtionTime=millis-checkPoint;
	//Close_BackgroundTimer();
	if(durtionTime<eta)
	{
		delay(eta-durtionTime);
	}
	currentMotorAngle=currentMotorAngle + (float)stepsToGo_int*motorStepAngle;
	//printf("%.2f\n",currentMotorAngle);
}


void Mission_Two(void)
{
	
	double Alpha_n=1,Alpha_n_1=1,Alpha_n_2=1;
	double g=9.7947;
	double deltT=1;
	double r=0.952;
	double B=0;
	double offsetAngle=87.1436;
	u32 currentTime=millis;
	u32 lastTime=currentTime-1;
	
	float targetMotorAngle=0;
	float A;
	float sign_=1;
	S_En(0);
	
	while(1)
	{
		currentTime=millis;
		float A=Encoder_ReadAngle();
		A=A-offsetAngle;
		if(A<-35)break;
		printf("%.4f\n",A);
		delay(20);
	}
	while(1)
	{
		currentTime=millis;
		A=Encoder_ReadAngle();
		A=A-offsetAngle;
		if(A >= -35)break;
		printf("Ready to go,%.4f\n",A);
		delay(10);
	}
	currentMotorAngle=A;
	lastTime=millis-20;
	S_En(1);
	{
		targetMotorAngle=30;
		float stepsToGo=(targetMotorAngle-currentMotorAngle)/motorStepAngle;
		float stepperGapTime=400;
		S_Dir(1);
		printf("Remain steps to Go=%d\n",(s32)stepsToGo);
		if(stepsToGo<0)stepsToGo=-stepsToGo;
		if(stepsToGo!=0)
		{
			for(int i=0;i<stepsToGo;i++)
			{
				S_Pulse(!PBin(3));
				delay_us(stepperGapTime);
				S_Pulse(!PBin(3));
				delay_us(stepperGapTime);
			}
		}
	}


	printf("Strick to 90 degree\n");
	while(1)
	{
		float A=Encoder_ReadAngle()-offsetAngle;
		printf(">%f\n",A);
		delay(50);
	}
}


void Mission_Three(void)
{
	
	S_En(1);
	delay(500);
	printf("MoveToAngle func\n");
	Open_BackgroundTimer();
	double offsetAngle=87.3193;
	float A=Encoder_ReadAngle()-offsetAngle;//Get current angle
	SetCurrentMotorAngle(A);//Set motor to be as pole
	printf("Start angle=%.4f\n",A);
	if(Infrared_In()==0)//if there being a block signal
	{
		printf("Continous blocking state\n");
		S_En(0);
		while(1);
	}
	
	while(1)
	{
		A=Encoder_ReadAngle()-offsetAngle;//Get current angle
		MoveToAngle(A,15);
		printf("%.2f %.2f\n",A,currentMotorAngle);
	}



	S_En(0);//Turn off motor
	while(1)
	{
		//printf("%d\n",Infrared_In());
		delay(100);
	}
}
int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//Init nvic priority group.16 preemption priorities.
	SysTick_Init();//1us precision
	//ILI9806G_Init();//Init lcd screen
	LED_Init();
	//init_TM1638();
	//Stepper_Init();
	Serial_Init(115200);
	Serial2_Init(115200);
  	//Serial2_print(" 123Test321\n");
	
	AngleInit();
	Main_Stepper_Init();
	TIM7_Configuration();
	TIM6_Configuration();
	printf("Running....\n");

	
	#define debug
	while(1)
	{
		#ifdef debug
			char serialData='3';
		#else
			while(Serial_available()==0)
			{
				delay_nothing(100);
			}
			char serialData=Serial_read()
		#endif

		switch(serialData)
		{
			case '1':
				printf("Execute mission one\n");
				delay(300);
				Mission_One();
			break;

			case '2':
				printf("Execute mission two\n");
				delay(300);
				Mission_Two();
			break;

			case '3':
				printf("Execute mission three\n");
				delay(300);
				Mission_Three();
			break;

			default:
				printf("Unknow command!\n");
				delay(300);
				break;
		}
	}	
}


/*********************************************END OF FILE**********************/

