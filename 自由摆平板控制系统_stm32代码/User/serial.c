#include "serial.h"
struct FifoQueue MyQueue;
struct FifoQueue MyQueue2;

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

  /* ?? USART ?? */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* GPIO??? */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ??Tx???????  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9  ;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ??Rx??????? */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
 /* ?? PXx ? USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

  /*  ?? PXx ? USARTx__Rx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
  
  /* ???USART1 ?? */
  /* ?????:115200 */
  USART_InitStructure.USART_BaudRate = speed;
  /* ??(???+???):8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* ???:1???? */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* ?????:????? */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* ?????:?????? */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART????:????????? */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ??USART????? */
  USART_Init(USART1, &USART_InitStructure); 
  
  /* ?????????NVIC?? */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* ??????1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ?????1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* ???? */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ?????NVIC */
  NVIC_Init(&NVIC_InitStructure);
  
  /* ???????? */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
  /* ???? */
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

///???c???printf???,???????printf??
int fputc(int ch, FILE *f)
{
    /* ??????????? */
    USART_SendData(USART1, (uint8_t) ch);
    
    /* ?????? */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);   
  
    return (ch);
}

///???c???scanf???,???????scanf?getchar???
int fgetc(FILE *f)
{
    /* ???????? */
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

//PA2 tx2
//PA3 rx2

/************************************Serial 2**********************************/
void  Serial2_Init(uint32_t speed)
{
  
   GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
     NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd( RS232_USART_RX_GPIO_CLK|RS232_USART_TX_GPIO_CLK, ENABLE);


  RCC_APB1PeriphClockCmd(RS232_USART_CLK, ENABLE);
  
  
  GPIO_PinAFConfig(RS232_USART_RX_GPIO_PORT,RS232_USART_RX_SOURCE, RS232_USART_RX_AF);

 
  GPIO_PinAFConfig(RS232_USART_TX_GPIO_PORT,RS232_USART_TX_SOURCE,RS232_USART_TX_AF);

 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = RS232_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS232_USART_TX_GPIO_PORT, &GPIO_InitStructure);

 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = RS232_USART_RX_PIN;
  GPIO_Init(RS232_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = speed;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(RS232_USART, &USART_InitStructure); 
  
 

  NVIC_InitStructure.NVIC_IRQChannel = RS232_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(RS232_USART, USART_IT_RXNE, ENABLE);
  
  USART_Cmd(RS232_USART, ENABLE);
  
}

uint8_t Serial2_available()
{
  if(MyQueue2.count==0&&(MyQueue2.front==MyQueue2.rear))
    return 0;
  else
    return 1;
}

uint8_t Serial2_read()
{
  QueueOut(&MyQueue2,&data);
  return data;
}

void Serial2_print(char *input)
{
  uint16_t i=0;
  while(input[i]!='\0')
  {
    USART_SendData(USART2, (uint8_t)input[i]);
    while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);    
    i++;
  }
}

extern int Rxflag;
extern u8 ucTemp;
void RS232_USART_IRQHandler(void)
{
  uint8_t data;
  if(USART_GetITStatus( RS232_USART, USART_IT_RXNE ) != RESET)
  {   
   
    data = USART_ReceiveData( RS232_USART );
    QueueIn(&MyQueue2,data);
  } 
} 



/*********************************************END OF FILE**********************/
