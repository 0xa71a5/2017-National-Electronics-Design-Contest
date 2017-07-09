#ifndef _TM1638_H
#define _TM1638_H

#include "stm32f4xx.h"
//DIO pE9
//CLK PE10
//STB PE11


#define DIO_high	PEout(9)=1;delay_us(2)
#define DIO_low		PEout(9)=0;delay_us(2)
#define CLK_high	PEout(10)=1;delay_us(2);
#define CLK_low		PEout(10)=0;delay_us(2)
#define STB_high  PEout(11)=1;delay_us(2)
#define STB_low		PEout(11)=0;delay_us(2)

#define DIO_IN  	{GPIOE->MODER&=~(3<<(9*2));GPIOE->MODER|=0<<9*2;}//����״̬
#define DIO_OUT  	{GPIOE->MODER&=~(3<<(9*2));GPIOE->MODER|=1<<9*2;}//���״̬
#define DIO_READ 	PEin(9)	 //�����ŵ�ƽ


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

void TM1638_Write(unsigned char DATA);   //д���ݺ���
unsigned char TM1638_Read(void);         //�����ݺ���
void Write_COM(unsigned char cmd);       //����������
unsigned char Read_key(void);
void Write_DATA(unsigned char add,unsigned char DATA);  //ָ����ַд������
void Write_allLED(unsigned char flag);     //����ȫ��LED������flag��ʾLED״̬
void init_TM1638(void);
#endif
