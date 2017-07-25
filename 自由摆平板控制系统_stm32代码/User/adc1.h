#ifndef __ADC1_H
#define __ADC1_H


#include "stm32f4xx.h"


#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC1+0x4c)


#define RHEOSTAT_ADC_GPIO_PORT    GPIOB
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_1
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOB


#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_9


#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0   

void ADC1_Init(void);


#endif /* __ADC1_H */

