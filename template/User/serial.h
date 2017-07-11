#ifndef __SERIAL_H
#define	__SERIAL_H

#include "stm32f4xx.h"
#include <stdio.h>


/* added setup */
#define ElemType       uint8_t
#define QueueSize      200
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
void 		Serial_print(char *input);
int 		fputc(int ch, FILE *f);
void		USART1_IRQHandler(void);


#endif /* __USART1_H */
