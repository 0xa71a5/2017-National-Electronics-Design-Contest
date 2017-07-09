#include "tm1638.h"
#include "stdint.h"
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const uint8_t tab2[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void TM1638_Write(unsigned char DATA)   //写数据函数
{
	unsigned char i;
	DIO_OUT;
	for(i=0;i<8;i++)
	{
	  CLK_low;
	  if(DATA & 0x01){
		  DIO_high;
	  }else{
		  DIO_low;
	  }
	  CLK_high;
	  DATA>>=1;
	}
}

unsigned char TM1638_Read(void)     //读数据函数
{
	unsigned char i;
	unsigned char temp=0;
	DIO_IN;
	STB_low;
	for(i=0;i<8;i++)
	{
		  temp>>=1;
		  CLK_low;
		  delay_us(100);
		  if(DIO_READ)
			  temp |= 0x80;
		  else
			  temp &= 0x7f;
		  CLK_high;
	}
	return temp;
}

void Write_COM(unsigned char cmd)  //发送命令字
{
	
	CLK_high;

	STB_high;
	STB_low;
	
	TM1638_Write(cmd);
	STB_high;
}

/*!
*函数功能：读取按键值
*输入参数：无
*输出参数：无
*返回值：  无
*/
unsigned char Read_key(void)
{
	unsigned char c[4];
	uint32_t i=0;
	STB_high;
	STB_low;
	TM1638_Write(0x42);
	delay_us(1);
	for(i=0;i<4;i++)
		c[i]=TM1638_Read();
	STB_high;
	
	i=(uint32_t)c[3]<<24|(uint32_t)c[2]<<16|(uint32_t)c[1]<<8|(uint32_t)c[0];
	
	switch(i)
	{
		case 0x00020000:return 1;break;
		case 0x00200000:return 2;break;
		case 0x02000000:return 3;break;
		case 0x20000000:return 4;break;
		
		case 0x00000002:return 5;break;
		case 0x00000020:return 6;break;
		case 0x00000200:return 7;break;
		case 0x00002000:return 8;break;
		
		case 0x00040000:return 9;break;
		case 0x00400000:return 10;break;
		case 0x04000000:return 11;break;
		case 0x40000000:return 12;break;
		
		case 0x00000004:return 13;break;
		case 0x00000040:return 14;break;
		case 0x00000400:return 15;break;
		case 0x00004000:return 16;break;
	}
}

void Write_DATA(unsigned char add,unsigned char DATA)  //指定地址写入数据
{
	Write_COM(0x44);
	STB_low;
	TM1638_Write(0xc0|add);
	TM1638_Write(DATA);
	STB_high;
}

void init_TM1638(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);			
	GPIO_SetBits(GPIOE,GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
	Write_allLED(0xff);
	delay(100);
	Write_allLED(0);
}

void  Write_allLED(unsigned char DATA){
	unsigned char i;
	Write_COM(0x8F);//显示控制
	Write_COM(0x40); //数据命令
	STB_low;
	TM1638_Write(0xc0);
	for(i=0;i<16;i++)
    TM1638_Write(DATA);
	STB_high;
}
