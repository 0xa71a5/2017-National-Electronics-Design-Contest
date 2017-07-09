#include "bsp_usart.h"
struct FifoQueue MyQueue;

uint8_t data;

void QueueInit(struct FifoQueue *Queue)
{
    Queue->rear=0;
    Queue->front = Queue->rear;
    Queue->count = 0;   
}

uint8_t QueueIn(struct FifoQueue *Queue,ElemType sdat)
{
    if((Queue->front == Queue->rear) && (Queue->count == QueueSize))
    {    
        return QueueFull;    
    }else
    {  
        Queue->dat[Queue->rear] = sdat;
        Queue->rear = (Queue->rear + 1) % QueueSize;
        Queue->count = Queue->count + 1;
        return QueueOperateOk;
    }
}
// Queue Out
uint8_t QueueOut(struct FifoQueue *Queue,ElemType *sdat)
{
    if((Queue->front == Queue->rear) && (Queue->count == 0))
    {                   
        return QueueEmpty;
    }else
    {                 
        *sdat = Queue->dat[Queue->front];
        Queue->front = (Queue->front + 1) % QueueSize;
        Queue->count = Queue->count - 1;
        return QueueOperateOk;
    }
}

void  Serial_Init(uint32_t speed)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOA,ENABLE);

  /* 使能 USART 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9  ;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
  
  /* 配置串USART1 模式 */
  /* 波特率设置：115200 */
  USART_InitStructure.USART_BaudRate = 115200;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(USART1, &USART_InitStructure); 
  
  /* 嵌套向量中断控制器NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* 抢断优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
  
  /* 使能串口接收中断 */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
  /* 使能串口 */
  USART_Cmd(USART1, ENABLE);
}

uint8_t Serial_available()
{
  if(MyQueue.count==0&&(MyQueue.front==MyQueue.rear))
    return 0;
  else
    return 1;
}

uint8_t Serial_read()
{
  QueueOut(&MyQueue,&data);
  return data;
}

void Serial_print(char *input)
{
  uint16_t i=0;
  while(input[i]!='\0')
  {
    USART_SendData(USART1, (uint8_t)input[i]);
    while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);    
    i++;
  }
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

void USART1_IRQHandler(void)
{
  uint8_t data;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{		
		data = USART_ReceiveData( USART1 );
    QueueIn(&MyQueue,data);
	}	 
}	

/*********************************************END OF FILE**********************/
