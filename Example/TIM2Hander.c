#include "include.h"

/**********************************************************************************************************
�������ƣ���ʱ����ʼ������
�����������
�����������
��ʱ����TIM2
**********************************************************************************************************/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


    /* Timer2�ж�*/
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* ��������*/
    TIM_TimeBaseStructure.TIM_Period = 1000;							//	����ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 36000;						//	Ԥ��Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  					
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//	���ϼ���

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


    /*ʹ��Ԥװ��*/
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    /*Ԥ����������ж�λ*/
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

    /* ��������ж�*/
    TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);


    /* ����TIM2��ʼ���� */
    TIM_Cmd(TIM2, ENABLE);
}

/*******************�жϷ�����***************************/
void TIM2_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);        
		
/*�û�����*/
	}
	
}

