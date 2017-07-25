#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f4xx.h"
#include <stdio.h>


/* added setup */
#define ElemType       uint8_t
#define QueueSize      1000
#define QueueFull      0  
#define QueueEmpty     1  
#define QueueOperateOk 2  

struct FifoQueue
{
    uint16_t front;     
    uint16_t rear;        
    uint16_t count;       
    ElemType dat[QueueSize];
};

void    QueueInit(struct FifoQueue *Queue);
uint8_t QueueIn(struct FifoQueue *Queue,ElemType sdat);
uint8_t QueueOut(struct FifoQueue *Queue,ElemType *sdat);
void    Serial_Init(uint32_t);
uint8_t Serial_available(void);
uint8_t Serial_read(void);
void    Serial_print(char *input);
int     fputc(int ch, FILE *f);
void    USART1_IRQHandler(void);

void    Serial2_Init(uint32_t);
uint8_t Serial2_available(void);
uint8_t Serial2_read(void);
void    Serial2_print(char *input);
void    USART2_IRQHandler(void);


/*******************************************************/
#define RS232_USART                             USART3
#define RS232_USART_CLK                         RCC_APB1Periph_USART3

#define RS232_USART_RX_GPIO_PORT                GPIOB
#define RS232_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define RS232_USART_RX_PIN                      GPIO_Pin_11
#define RS232_USART_RX_AF                       GPIO_AF_USART3
#define RS232_USART_RX_SOURCE                   GPIO_PinSource11

#define RS232_USART_TX_GPIO_PORT                GPIOB
#define RS232_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define RS232_USART_TX_PIN                      GPIO_Pin_10
#define RS232_USART_TX_AF                       GPIO_AF_USART3
#define RS232_USART_TX_SOURCE                   GPIO_PinSource10


#define RS232_USART_IRQHandler                   USART3_IRQHandler
#define RS232_USART_IRQ                                     USART3_IRQn
/************************************************************/

/*
#define RS232_USART                             USART2
#define RS232_USART_CLK                         RCC_APB1Periph_USART2

#define RS232_USART_RX_GPIO_PORT                GPIOA
#define RS232_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define RS232_USART_RX_PIN                      GPIO_Pin_3
#define RS232_USART_RX_AF                       GPIO_AF_USART2
#define RS232_USART_RX_SOURCE                   GPIO_PinSource3

#define RS232_USART_TX_GPIO_PORT                GPIOA
#define RS232_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define RS232_USART_TX_PIN                      GPIO_Pin_2
#define RS232_USART_TX_AF                       GPIO_AF_USART2
#define RS232_USART_TX_SOURCE                   GPIO_PinSource2


#define RS232_USART_IRQHandler                   USART2_IRQHandler
#define RS232_USART_IRQ                                     USART2_IRQn
*/


#define RS232_USART_BAUDRATE                    115200
#endif /* __USART1_H */
