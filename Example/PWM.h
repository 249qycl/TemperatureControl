#ifndef _PWM_H_
#define _PWM_H_

#include "stm32f4xx.h"

#define  	PWM_OFF      	GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define  	PWM_ON      	GPIO_SetBits(GPIOB, GPIO_Pin_6)

//  º¯ÊýÉùÃ÷
void TIM3_PWM_Init(unsigned int arr, unsigned int psc);
    
#endif
