#include "stepper.h"
#include "bsp_led.h"
uint8_t Stepper1_State=0;
int32_t Stepper1_Steps=0;
uint8_t Stepper1_RunFlag=0;


uint8_t Stepper2_State=0;
int32_t Stepper2_Steps=0;
uint8_t Stepper2_RunFlag=0;

void Stepper_Init()
{
    //��ʼ��6�����ţ�ȫ����GPIO�������
    //���ڲ������1 �� En->PD7  Dir->PD3  Pulse->PE2
    //���ڲ������2 :  En->PA3  Dir->PA2  Pulse->PB0
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC| RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE); 
    
    /* Stepper1 gpio init */
    GPIO_InitStructure.GPIO_Pin = Stepper1_En_Pin;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(Stepper1_En_Port, &GPIO_InitStructure);   
    
    GPIO_InitStructure.GPIO_Pin = Stepper1_Dir_Pin;
    GPIO_Init(Stepper1_Dir_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Stepper1_Pulse_Pin;
    GPIO_Init(Stepper1_Pulse_Port, &GPIO_InitStructure);


    /* Stpper2 gpio init */
    
    GPIO_InitStructure.GPIO_Pin = Stepper2_En_Pin;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(Stepper2_En_Port, &GPIO_InitStructure);   
    GPIO_InitStructure.GPIO_Pin = Stepper2_Dir_Pin;
    GPIO_Init(Stepper2_Dir_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = Stepper2_Pulse_Pin;
    GPIO_Init(Stepper2_Pulse_Port, &GPIO_InitStructure);    
    
    
    Stepper1_Dir(1);
    Stepper1_En(0);//ʧ�ܲ������1
    Stepper1_Pulse(0);


    Stepper2_Dir(1);
    Stepper2_En(0);//ʧ�ܲ������2
    Stepper2_Pulse(0);

    TIM7_Configuration();//��ʼ����ʱ��7 ���Ʋ������1
    TIM2_Configuration();//��ʼ����ʱ��2 ���Ʋ������2
    TIM7->DIER &= (uint16_t)~TIM_IT_Update;//��ֹ�ж�7����
    TIM2->DIER &= (uint16_t)~TIM_IT_Update;//��ֹ�ж�2����
}

void Stepper1_SetSpeed(u32 steps,u32 speed)
{
    //Stepper1_Speed(speed);
    TIM7->ARR=speed;//����ת��Ϊspeed ��λus
    Stepper1_Steps=steps;//����ʣ�ಽ��Ϊsteps
    TIM7->DIER |= TIM_IT_Update;//ʹ���ж�7����
}

void Stepper2_SetSpeed(u32 steps,u32 speed)
{
    TIM2->ARR=speed;//����ת��Ϊspeed ��λus
    Stepper2_Steps=steps;//����ʣ�ಽ��Ϊsteps
    TIM2->DIER |= TIM_IT_Update;//ʹ���ж�2����
}

void Stepper_Wait()
{
    while(Stepper1_Steps>0||Stepper2_Steps>0)
    {
        //printf("M Steps1=%d ;Steps2=%d\n",Stepper1_Steps,Stepper2_Steps);
        delay_us(50);
    }
    //TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE);
    TIM7->DIER &= (uint16_t)~TIM_IT_Update;//��ֹ�ж�7����
}


void TIM7_IRQHandler(void)//��ʱ��7�ж� ����Stepper1������
{
 //�ж��Ƿ���TIM7�ж�
 if (((TIM7->SR & TIM_IT_Update) != 0 ) && ((TIM7->DIER & TIM_IT_Update) != 0))
  { 
    if(Stepper1_Steps>0)//�жϵ�ǰ�Ƿ����в���  ���� ����ʣ�ಽ���Ƿ�Ϊ0
    {
        Stepper1_State=~Stepper1_State;//��������
        Stepper1_Pulse(Stepper1_State);//��������
        Stepper1_Steps--;//ʣ�ಽ���Լ�
        LED1_ON();//debug   
    }   
    TIM7->SR = (uint16_t)~TIM_IT_Update; //����ж�λ
  }     
}

void  TIM2_IRQHandler(void)//��ʱ��2�ж� ����Stepper2������
{
//�ж��Ƿ���TIM2�ж�
 if (((TIM2->SR & TIM_IT_Update) != 0 ) && ((TIM2->DIER & TIM_IT_Update) != 0))
  { 
    if(Stepper2_Steps>0)//�жϵ�ǰ�Ƿ����в���  ���� ����ʣ�ಽ���Ƿ�Ϊ0
    {
        Stepper2_State=~Stepper2_State;//��������
        Stepper2_Pulse(Stepper2_State);//��������
        Stepper2_Steps--;//ʣ�ಽ���Լ�
        LED1_ON();//debug 
    }   
    TIM2->SR = (uint16_t)~TIM_IT_Update; //����ж�λ  
  }     
}

