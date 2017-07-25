#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

void EXTI_Config(void);
void EXTI0_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

#endif /* __EXTI_H */
