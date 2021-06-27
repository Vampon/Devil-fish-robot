#include "motor.h"

int16_t Motor_PWM[4];
int16_t Last_PWM[4];
//72M------71/19999
void Motor_PWM_Init(u16 arr,u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  //Enable clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  //PWM GPIO Init
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);
  GPIO_ResetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);

  //Timer Init
  TIM_TimeBaseStructure.TIM_Period = arr;
  TIM_TimeBaseStructure.TIM_Prescaler =psc;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  //Set PWM mode & ch 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  //Enable ch
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  //Enable timer
  TIM_Cmd(TIM3, ENABLE);
  //Motor_PWM_Set(0,0,0,0);
    
}


void Motor_PWM_Set(int16_t motor1 , int16_t motor2 , int16_t motor3 , int16_t motor4)
{   
//   
//    if(motor1>500)motor1=500;
//    if(motor1<0)motor1=0;
//    
//    if(motor2>500)motor2=500;
//    if(motor2<0)motor2=0;
//    
//    if(motor3>500)motor3=500;
//    if(motor3<0)motor3=0;
//    
//    if(motor4>500)motor4=500;
//    if(motor4<0)motor4=0;

  
    TIM_SetCompare1(TIM3,motor1);
    TIM_SetCompare2(TIM3,motor2);
    TIM_SetCompare3(TIM3,motor3);
    TIM_SetCompare4(TIM3,motor4);
  
    Last_PWM[0]=motor1;
    Last_PWM[1]=motor2;
    Last_PWM[2]=motor3;
    Last_PWM[3]=motor4;
  
}
