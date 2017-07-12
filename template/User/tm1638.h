#ifndef _TM1638_H
#define _TM1638_H

#include "stm32f4xx.h"
//DIO PB9
//CLK PB12
//STB PC0 //PB14有问题

#define DIO_Pin     GPIO_Pin_9
#define DIO_Clock   RCC_AHB1Periph_GPIOB
#define DIO_Port    GPIOB
#define DIO_high	PBout(9)=1;delay_us(2)
#define DIO_low		PBout(9)=0;delay_us(2)
#define DIO_IN      {DIO_Port->MODER&=~(3<<(9*2));DIO_Port->MODER|=0<<9*2;}//输入状态
#define DIO_OUT     {DIO_Port->MODER&=~(3<<(9*2));DIO_Port->MODER|=1<<9*2;}//输出状态
#define DIO_READ    PBin(9)  //读引脚电平

#define CLK_Pin     GPIO_Pin_12
#define CLK_Clock   RCC_AHB1Periph_GPIOB
#define CLK_Port    GPIOB
#define CLK_high	PBout(12)=1;delay_us(2)
#define CLK_low		PBout(12)=0;delay_us(2)


#define STB_Pin     GPIO_Pin_0
#define STB_Clock   RCC_AHB1Periph_GPIOC
#define STB_Port    GPIOC
#define STB_high    PCout(0)=1;delay_us(2)
#define STB_low		PCout(0)=0;delay_us(2)




#define BIT0 0x0001
#define BIT1 0x0002
#define BIT2 0x0004
#define BIT3 0x0008
#define BIT4 0x0010
#define BIT5 0x0020
#define BIT6 0x0040
#define BIT7 0x0080
#define BIT8 0x0100
#define BIT9 0x0200
#define BITA 0x0400
#define BITB 0x0800
#define BITC 0x1000
#define BITD 0x2000
#define BITE 0x4000
#define BITF 0x8000

void TM1638_Write(unsigned char DATA);   //写数据函数
unsigned char TM1638_Read(void);         //读数据函数
void Write_COM(unsigned char cmd);       //发送命令字
unsigned char Read_key(void);
void Write_DATA(unsigned char add,unsigned char DATA);  //指定地址写入数据
void Write_allLED(unsigned char flag);     //控制全部LED函数，flag表示LED状态
void init_TM1638(void);
#endif
