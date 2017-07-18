#ifndef __PWM_H
#define	__PWM_H

#include "stm32f4xx.h"

void TIM3_PWM_Configuration(void);
void TIM4_PWM_Configuration(void);

/* PCXpwm(x) x范围 0-999 */
#define PC6pwm(x)   TIM3->CCR1=x;
#define PC7pwm(x)   TIM3->CCR2=x;
#define PC8pwm(x)   TIM3->CCR3=x;
#define PC9pwm(x)   TIM3->CCR4=x;

#define PB6pwm(x)   TIM4->CCR1=x;
#define PB7pwm(x)   TIM4->CCR2=x;
#define PB8pwm(x)   TIM4->CCR3=x;
#define PB9pwm(x)   TIM4->CCR4=x;

#endif /* __GENERAL_TIM_H */

