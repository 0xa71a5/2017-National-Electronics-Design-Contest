#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void delay_us(__IO u32 nTime);
void delay(__IO u32 nTime);
void SysTick_Handler(void);
#endif /* __SYSTICK_H */
