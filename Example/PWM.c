#include "stm32f4xx.h"
#include "PWM.h"

/**********************************************************************************************************
函数名称：TIM3_PWM_Init
输入参数：自动装载值
输出参数：时钟分频数
函数返回：无
函数说明：右前轮电机PWM初始化
硬件定义：
TIM3_CH3    PB0     右前轮A
**********************************************************************************************************/
void TIM3_PWM_Init(unsigned int arr, unsigned int psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    //  TIM3 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    //  GPIOB clock enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    //  GPIOB Configuration: TIM3 CH3 (PB0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    //  Connect TIM3 pins to AF2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

    //  Time base configuration
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //  PWM1 Mode configuration: Channel1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                                  //  初值为0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    //  TIM3 enable counter
    TIM_Cmd(TIM3, ENABLE);
}

