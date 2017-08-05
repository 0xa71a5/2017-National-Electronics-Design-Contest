#include "tm1638.h"
#include "bsp_ili9806g_lcd.h"
#include "bsp_SysTick.h"
#include "serial.h"
#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#define MAX(a,b)  (a>b)?2:1
char command[20] = {'0'};
void Key2char()
{
	ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //Clear
	int index = 0;
	int temp = 0;	
	int i = 0;
	int key = 0;
	for(i = 0;i<20;i++)
	{
		command[i] = '0';
	}
	command[0] = '<';
	key = Read_key();
	while((key!=15)&&(key!=16))
	{
		if((key!=0)&&(index<20))
		{
			ILI9806G_DispChar_EN(80,100,'<');
			temp = key;
			if(key == 11)
			{
				ILI9806G_DispChar_EN(100+index*20,100,'.');
				index = index + 1;command[index] = '/';				
			}
			else if(key == 13)
			{
				ILI9806G_DispChar_EN(100+index*20,100,',');
				index = index + 1;command[index] = ',';
				index = index + 1;command[index] = ' ';
			}
			else if(key == 14)
			{
				ILI9806G_DispChar_EN(100+index*20,100,'>');
				index = index + 1;command[index] = '>';
			}
			else if(key == 10)
			{
				ILI9806G_DispChar_EN(100+index*20,100,0+'0');
				index = index + 1;command[index] = '0';
			}
			else if(key == 12)
			{
				command[index] = '0';
				index = index - 1;
				ILI9806G_DispChar_EN(100+index*20,100,' ');
			}
			else
			{	
				ILI9806G_DispChar_EN(100+index*20,100,key+'0');
				index = index + 1;command[index] = key + '0';
			}
		}	
		delay(150);
		key = Read_key();
	}
	//printf("command is :%s",command);
}
void Char2int(char*a)
{
	double buff_int[4]={0};
	double num[2] = {0.0};
	int i = 0;
	char buf1[2] = {'0'};
	char buf2[2] = {'0'};
	char buf3[2] = {'0'};
	char buf4[2] = {'0'};
	int flag1 = 0;int flag2 = 0;
	for(i=0;i<11;i++)
	{
		a[i] = a[i+1];
	}
	//第一个数的十位
	sscanf(a,"%[^/]", buf1);
	buff_int[0] = atoi(buf1);
	//printf("%d\n",buff_int[0]);
	//第二个数的十位
	sscanf(a,"%*s%s", buf2);
	sscanf(buf2,"%[^/]", buf2);
	buff_int[2] = atoi(buf2);
	//printf("%d\n",buff_int[2]);
	//第一个数的小数
	sscanf(a, "%*[^/]/%[^,]", buf3);
	if(buf3[0]=='0')
	{
		if(buf3[1] == '0')
		{
			buff_int[1] = atoi(buf3)*0.01;
		}
		buff_int[1] = atoi(buf3)*0.1;
		flag1 = 1;
	}
	printf("%lf\n",buff_int[1]);
	//第二个数的小数
	sscanf(a,"%*s%s", buf4);
	sscanf(buf4, "%*[^/]/%[^>]", buf4);
	if(buf4[0]=='0')
	{
		if(buf4[1] == '0')
		{
			buff_int[3] = atoi(buf4)*0.01;
		}
		buff_int[3] = atoi(buf4)*0.1;
		flag2 = 1;
	}
	printf("%lf\n",buff_int[3]);
	
	int k1 = MAX(buff_int[1],10);
	int k2 = MAX(buff_int[3],10);
	if(flag1 == 0)
	{
		num[0] = buff_int[0]+pow(10,-k1)*buff_int[1];
	}
	else
	{
		num[0] = buff_int[0]+buff_int[1];
	}
	if(flag2 == 0)
	{
		num[1] = buff_int[2]+pow(10,-k1)*buff_int[3];
	}
	else
	{
		num[1] = buff_int[2]+buff_int[3];
	}
	printf("%lf\n",num[0]);
	printf("%lf\n",num[1]);
}