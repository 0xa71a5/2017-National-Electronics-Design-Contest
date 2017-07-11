#ifndef __STEPPER_H
#define __STEPPER_H

#include "stm32f4xx.h"

void Stepper_Init(void);
void Stepper1_SetSpeed(s32 steps,u32 speed);
void Stepper2_SetSpeed(s32 steps,u32 speed);
void Stepper_Wait();

/* 步进电机1 引脚定义 */
#define Stepper1_Dir(x) PDout(3)=x
#define Stepper1_En(x) PDout(7)=x
#define Stepper1_Pulse(x) PEout(2)=x


#define Stepper1_Dir_Clock RCC_AHB1Periph_GPIOD
#define Stepper1_Dir_Pin     GPIO_Pin_3
#define Stepper1_Dir_Port  GPIOD

#define Stepper1_En_Clock RCC_AHB1Periph_GPIOD
#define Stepper1_En_Pin     GPIO_Pin_7
#define Stepper1_En_Port  GPIOD

#define Stepper1_Pulse_Clock RCC_AHB1Periph_GPIOE
#define Stepper1_Pulse_Pin   GPIO_Pin_2
#define Stepper1_Pulse_Port  GPIOE

/* 步进电机2 引脚定义 */
#define Stepper2_Dir(x) PAout(2)=x
#define Stepper2_En(x)  PAout(3)=x
#define Stepper2_Pulse(x) PBout(15)=x

#define Stepper2_Dir_Clock RCC_AHB1Periph_GPIOA
#define Stepper2_Dir_Pin     GPIO_Pin_2
#define Stepper2_Dir_Port  GPIOA

#define Stepper2_En_Clock RCC_AHB1Periph_GPIOA
#define Stepper2_En_Pin     GPIO_Pin_3
#define Stepper2_En_Port  GPIOA

#define Stepper2_Pulse_Clock RCC_AHB1Periph_GPIOB
#define Stepper2_Pulse_Pin   GPIO_Pin_15
#define Stepper2_Pulse_Port  GPIOB


#endif 